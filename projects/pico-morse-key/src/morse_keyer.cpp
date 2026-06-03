#include "morse_keyer.hpp"

namespace {
constexpr uint16_t kMinWpm = 5;
constexpr uint16_t kMaxWpm = 60;

uint16_t clamp_wpm(uint16_t wpm) {
  if (wpm < kMinWpm) {
    return kMinWpm;
  }
  if (wpm > kMaxWpm) {
    return kMaxWpm;
  }
  return wpm;
}
}  // namespace

MorseKeyer::MorseKeyer(MorseKeyerConfig config) : config_(config) {
  set_wpm(config_.wpm);
}

void MorseKeyer::reset() {
  state_ = State::Idle;
  current_ = MorseElement::None;
  last_sent_ = MorseElement::Dash;
  remaining_ms_ = 0;
  dot_latched_ = false;
  dash_latched_ = false;
}

void MorseKeyer::set_wpm(uint16_t wpm) {
  config_.wpm = clamp_wpm(wpm);
  dot_ms_ = static_cast<uint16_t>(1200U / config_.wpm);
}

MorseKeyerOutput MorseKeyer::tick(bool dot_pressed, bool dash_pressed) {
  latch_inputs(dot_pressed, dash_pressed);

  const bool started = (state_ == State::Idle) && start_next_element();
  MorseKeyerOutput output{
      state_ == State::Keying,
      state_ == State::Keying ? current_ : MorseElement::None,
      started,
  };

  if (state_ == State::Keying || state_ == State::Gap) {
    if (remaining_ms_ > 0) {
      --remaining_ms_;
    }

    if (remaining_ms_ == 0) {
      if (state_ == State::Keying) {
        state_ = State::Gap;
        remaining_ms_ = dot_ms_;
      } else {
        state_ = State::Idle;
        current_ = MorseElement::None;
      }
    }
  }

  return output;
}

void MorseKeyer::latch_inputs(bool dot_pressed, bool dash_pressed) {
  if (state_ == State::Keying) {
    if (!config_.iambic_b) {
      return;
    }

    if (current_ != MorseElement::Dot && dot_pressed) {
      dot_latched_ = true;
    }
    if (current_ != MorseElement::Dash && dash_pressed) {
      dash_latched_ = true;
    }
    return;
  }

  if (dot_pressed) {
    dot_latched_ = true;
  }
  if (dash_pressed) {
    dash_latched_ = true;
  }
}

bool MorseKeyer::start_next_element() {
  MorseElement next = MorseElement::None;

  if (dot_latched_ && dash_latched_) {
    next = last_sent_ == MorseElement::Dot ? MorseElement::Dash : MorseElement::Dot;
  } else if (dot_latched_) {
    next = MorseElement::Dot;
  } else if (dash_latched_) {
    next = MorseElement::Dash;
  }

  if (next == MorseElement::None) {
    return false;
  }

  if (!config_.iambic_b && dot_latched_ && dash_latched_) {
    dot_latched_ = false;
    dash_latched_ = false;
  } else if (next == MorseElement::Dot) {
    dot_latched_ = false;
  } else {
    dash_latched_ = false;
  }

  state_ = State::Keying;
  current_ = next;
  last_sent_ = next;
  remaining_ms_ = duration_for(next);
  return true;
}

uint16_t MorseKeyer::duration_for(MorseElement element) const {
  return element == MorseElement::Dash ? static_cast<uint16_t>(dot_ms_ * 3U)
                                       : dot_ms_;
}
