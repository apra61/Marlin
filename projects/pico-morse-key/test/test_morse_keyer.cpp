#include "cs1237.hpp"
#include "morse_keyer.hpp"

#include <cstdlib>
#include <functional>
#include <iostream>
#include <string>
#include <utility>
#include <vector>

struct StartedElement {
  MorseElement element;
  int time_ms;
};

using PaddlePattern = std::function<std::pair<bool, bool>(int time_ms)>;

std::vector<StartedElement> simulate(MorseKeyer& keyer, int total_ms,
                                     const PaddlePattern& pattern,
                                     int* keyed_ms = nullptr) {
  std::vector<StartedElement> starts;
  int local_keyed_ms = 0;

  for (int t = 0; t < total_ms; ++t) {
    const auto [dot, dash] = pattern(t);
    const MorseKeyerOutput output = keyer.tick(dot, dash);
    if (output.key_down) {
      ++local_keyed_ms;
    }
    if (output.element_started) {
      starts.push_back({output.element, t});
    }
  }

  if (keyed_ms != nullptr) {
    *keyed_ms = local_keyed_ms;
  }
  return starts;
}

void require(bool condition, const std::string& message) {
  if (!condition) {
    std::cerr << "FAIL: " << message << '\n';
    std::exit(1);
  }
}

void require_start(const std::vector<StartedElement>& starts, size_t index,
                   MorseElement element, int time_ms) {
  require(index < starts.size(), "missing start at index " + std::to_string(index));
  require(starts[index].element == element, "unexpected element at index " + std::to_string(index));
  require(starts[index].time_ms == time_ms, "unexpected start time at index " + std::to_string(index));
}

void test_single_dot_timing() {
  MorseKeyer keyer({20, true});
  int keyed_ms = 0;
  const auto starts = simulate(
      keyer, 130, [](int t) { return std::make_pair(t == 0, false); }, &keyed_ms);

  require(starts.size() == 1, "single tap should start exactly one dot");
  require_start(starts, 0, MorseElement::Dot, 0);
  require(keyed_ms == 60, "20 WPM dot must key transmitter for 60 ms");
}

void test_held_dot_repeats_after_element_gap() {
  MorseKeyer keyer({20, true});
  const auto starts = simulate(
      keyer, 260, [](int) { return std::make_pair(true, false); });

  require(starts.size() == 3, "held dot should repeat at dot+gap cadence");
  require_start(starts, 0, MorseElement::Dot, 0);
  require_start(starts, 1, MorseElement::Dot, 120);
  require_start(starts, 2, MorseElement::Dot, 240);
}

void test_held_dash_repeats_after_element_gap() {
  MorseKeyer keyer({20, true});
  const auto starts = simulate(
      keyer, 500, [](int) { return std::make_pair(false, true); });

  require(starts.size() == 3, "held dash should repeat at dash+gap cadence");
  require_start(starts, 0, MorseElement::Dash, 0);
  require_start(starts, 1, MorseElement::Dash, 240);
  require_start(starts, 2, MorseElement::Dash, 480);
}

void test_iambic_squeeze_alternates() {
  MorseKeyer keyer({20, true});
  const auto starts = simulate(
      keyer, 600, [](int) { return std::make_pair(true, true); });

  require(starts.size() >= 4, "squeeze should alternate dot and dash");
  require_start(starts, 0, MorseElement::Dot, 0);
  require_start(starts, 1, MorseElement::Dash, 120);
  require_start(starts, 2, MorseElement::Dot, 360);
  require_start(starts, 3, MorseElement::Dash, 480);
}

void test_short_squeeze_has_one_opposite_memory_element() {
  MorseKeyer keyer({20, true});
  const auto starts = simulate(
      keyer, 400, [](int t) { return std::make_pair(t < 10, t < 10); });

  require(starts.size() == 2, "short squeeze should emit dot plus one remembered dash");
  require_start(starts, 0, MorseElement::Dot, 0);
  require_start(starts, 1, MorseElement::Dash, 120);
}

void test_iambic_a_does_not_add_release_memory() {
  MorseKeyer keyer({20, false});
  const auto starts = simulate(
      keyer, 400, [](int t) { return std::make_pair(t < 10, t < 10); });

  require(starts.size() == 1, "iambic A short squeeze should not add release memory");
  require_start(starts, 0, MorseElement::Dot, 0);
}

void test_cs1237_sign_extension() {
  require(Cs1237::sign_extend24(0x000000U) == 0, "CS1237 zero conversion failed");
  require(Cs1237::sign_extend24(0x000001U) == 1, "CS1237 positive LSB conversion failed");
  require(Cs1237::sign_extend24(0x7FFFFFU) == 8388607, "CS1237 max positive conversion failed");
  require(Cs1237::sign_extend24(0x800000U) == -8388608, "CS1237 min negative conversion failed");
  require(Cs1237::sign_extend24(0xFFFFFFU) == -1, "CS1237 negative LSB conversion failed");
}

int main() {
  test_single_dot_timing();
  test_held_dot_repeats_after_element_gap();
  test_held_dash_repeats_after_element_gap();
  test_iambic_squeeze_alternates();
  test_short_squeeze_has_one_opposite_memory_element();
  test_iambic_a_does_not_add_release_memory();
  test_cs1237_sign_extension();
  std::cout << "All host tests passed.\n";
  return 0;
}
