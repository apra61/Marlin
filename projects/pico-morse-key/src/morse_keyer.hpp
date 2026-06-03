#pragma once

#include <cstdint>

enum class MorseElement {
  None,
  Dot,
  Dash,
};

struct MorseKeyerConfig {
  uint16_t wpm = 20;
  bool iambic_b = true;
};

struct MorseKeyerOutput {
  bool key_down = false;
  MorseElement element = MorseElement::None;
  bool element_started = false;
};

class MorseKeyer {
 public:
  explicit MorseKeyer(MorseKeyerConfig config = {});

  void reset();
  void set_wpm(uint16_t wpm);
  uint16_t dot_ms() const { return dot_ms_; }

  MorseKeyerOutput tick(bool dot_pressed, bool dash_pressed);

 private:
  enum class State {
    Idle,
    Keying,
    Gap,
  };

  void latch_inputs(bool dot_pressed, bool dash_pressed);
  bool start_next_element();
  uint16_t duration_for(MorseElement element) const;

  MorseKeyerConfig config_;
  State state_ = State::Idle;
  MorseElement current_ = MorseElement::None;
  MorseElement last_sent_ = MorseElement::Dash;
  uint16_t dot_ms_ = 60;
  uint16_t remaining_ms_ = 0;
  bool dot_latched_ = false;
  bool dash_latched_ = false;
};
