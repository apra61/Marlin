#include <cstdio>

#include "hardware/pwm.h"
#include "cs1237.hpp"
#include "morse_keyer.hpp"
#include "pico/stdlib.h"

namespace {
constexpr uint kDotDoutGpio = 2;
constexpr uint kDotSclkGpio = 3;
constexpr uint kDashDoutGpio = 4;
constexpr uint kDashSclkGpio = 5;
constexpr uint kKeyOutGpio = 16;
constexpr uint kSidetoneGpio = 15;
constexpr uint kStatusLedGpio = 25;

constexpr uint16_t kWpm = 20;
constexpr int32_t kPressThresholdCounts = 80000;
constexpr int32_t kReleaseThresholdCounts = 45000;
constexpr uint16_t kCalibrationSamples = 64;
constexpr uint16_t kSidetoneHz = 700;
constexpr float kSignalAlpha = 0.35f;
constexpr float kZeroTrackingAlpha = 0.0005f;
constexpr uint8_t kCs1237Config = Cs1237::kChannelA | Cs1237::kGain128 |
                                  Cs1237::kRate640Hz | Cs1237::kRefOutputEnabled;

class Cs1237Paddle {
 public:
  Cs1237Paddle(uint dout_gpio, uint sclk_gpio, int direction)
      : adc_(dout_gpio, sclk_gpio), direction_(direction) {}

  bool init() {
    adc_.init();
    return adc_.write_config(kCs1237Config, 250000);
  }

  bool calibrate() {
    int64_t sum = 0;
    for (uint16_t i = 0; i < kCalibrationSamples; ++i) {
      int32_t raw = 0;
      if (!adc_.read_raw_blocking(&raw, 500000)) {
        return false;
      }
      sum += raw;
    }

    zero_counts_ = static_cast<float>(sum) / kCalibrationSamples;
    filtered_counts_ = zero_counts_;
    active_ = false;
    return true;
  }

  bool sample() {
    int32_t raw = 0;
    if (adc_.read_raw_nonblocking(&raw)) {
      last_raw_ = raw;
      filtered_counts_ += (static_cast<float>(raw) - filtered_counts_) * kSignalAlpha;
    }

    const float deflection = direction_ * (filtered_counts_ - zero_counts_);

    if (active_) {
      active_ = deflection > kReleaseThresholdCounts;
    } else {
      active_ = deflection > kPressThresholdCounts;
    }

    if (!active_) {
      zero_counts_ += (filtered_counts_ - zero_counts_) * kZeroTrackingAlpha;
    }

    last_deflection_ = static_cast<int32_t>(deflection);
    return active_;
  }

  int32_t last_raw() const { return last_raw_; }
  int32_t last_deflection() const { return last_deflection_; }

 private:
  Cs1237 adc_;
  int direction_;
  bool active_ = false;
  int32_t last_raw_ = 0;
  int32_t last_deflection_ = 0;
  float zero_counts_ = 0.0f;
  float filtered_counts_ = 0.0f;
};

uint sidetone_slice;
uint sidetone_channel;
uint16_t sidetone_wrap;

void setup_sidetone_pwm() {
  gpio_set_function(kSidetoneGpio, GPIO_FUNC_PWM);
  sidetone_slice = pwm_gpio_to_slice_num(kSidetoneGpio);
  sidetone_channel = pwm_gpio_to_channel(kSidetoneGpio);

  constexpr float kClockDiv = 64.0f;
  sidetone_wrap = static_cast<uint16_t>((125000000.0f / (kClockDiv * kSidetoneHz)) - 1.0f);
  pwm_set_clkdiv(sidetone_slice, kClockDiv);
  pwm_set_wrap(sidetone_slice, sidetone_wrap);
  pwm_set_chan_level(sidetone_slice, sidetone_channel, 0);
  pwm_set_enabled(sidetone_slice, true);
}

void set_outputs(bool key_down) {
  gpio_put(kKeyOutGpio, key_down ? 1 : 0);
  gpio_put(kStatusLedGpio, key_down ? 1 : 0);
  pwm_set_chan_level(sidetone_slice, sidetone_channel,
                     key_down ? sidetone_wrap / 2 : 0);
}
}  // namespace

int main() {
  stdio_init_all();

  gpio_init(kKeyOutGpio);
  gpio_set_dir(kKeyOutGpio, GPIO_OUT);
  gpio_put(kKeyOutGpio, 0);

  gpio_init(kStatusLedGpio);
  gpio_set_dir(kStatusLedGpio, GPIO_OUT);
  gpio_put(kStatusLedGpio, 0);

  setup_sidetone_pwm();

  Cs1237Paddle dot_paddle(kDotDoutGpio, kDotSclkGpio, 1);
  Cs1237Paddle dash_paddle(kDashDoutGpio, kDashSclkGpio, 1);

  sleep_ms(1000);
  printf("Pico CS1237 strain-gauge Morse keyer: keep both paddles released.\n");
  const bool dot_configured = dot_paddle.init();
  const bool dash_configured = dash_paddle.init();
  printf("CS1237 config: dot=%s dash=%s, target config=0x%02x\n",
         dot_configured ? "ok" : "timeout",
         dash_configured ? "ok" : "timeout",
         kCs1237Config);

  if (!dot_paddle.calibrate() || !dash_paddle.calibrate()) {
    printf("Calibration failed: check CS1237 wiring and bridge power.\n");
    while (true) {
      gpio_put(kStatusLedGpio, 1);
      sleep_ms(100);
      gpio_put(kStatusLedGpio, 0);
      sleep_ms(900);
    }
  }
  printf("Calibration done. WPM=%u, dot=%u ms\n", kWpm, MorseKeyer({kWpm, true}).dot_ms());

  MorseKeyer keyer({kWpm, true});
  uint16_t status_ticks = 0;

  while (true) {
    const bool dot_pressed = dot_paddle.sample();
    const bool dash_pressed = dash_paddle.sample();
    const MorseKeyerOutput output = keyer.tick(dot_pressed, dash_pressed);
    set_outputs(output.key_down);

    if (++status_ticks >= 1000) {
      status_ticks = 0;
      printf("dot raw=%ld def=%ld active=%u | dash raw=%ld def=%ld active=%u\n",
             static_cast<long>(dot_paddle.last_raw()),
             static_cast<long>(dot_paddle.last_deflection()), dot_pressed ? 1 : 0,
             static_cast<long>(dash_paddle.last_raw()),
             static_cast<long>(dash_paddle.last_deflection()), dash_pressed ? 1 : 0);
    }

    sleep_ms(1);
  }
}
