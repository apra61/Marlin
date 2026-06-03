#include <cstdio>

#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "morse_keyer.hpp"
#include "pico/stdlib.h"

namespace {
constexpr uint kDotAdcGpio = 26;
constexpr uint kDashAdcGpio = 27;
constexpr uint kDotAdcChannel = 0;
constexpr uint kDashAdcChannel = 1;
constexpr uint kKeyOutGpio = 16;
constexpr uint kSidetoneGpio = 15;
constexpr uint kStatusLedGpio = 25;

constexpr uint16_t kWpm = 20;
constexpr uint16_t kPressThresholdCounts = 420;
constexpr uint16_t kReleaseThresholdCounts = 260;
constexpr uint16_t kCalibrationSamples = 1000;
constexpr uint16_t kSidetoneHz = 700;
constexpr float kSignalAlpha = 0.18f;
constexpr float kZeroTrackingAlpha = 0.00015f;

class AnalogPaddle {
 public:
  AnalogPaddle(uint gpio, uint adc_channel, int direction)
      : gpio_(gpio), adc_channel_(adc_channel), direction_(direction) {}

  void init() const {
    adc_gpio_init(gpio_);
  }

  void calibrate() {
    uint32_t sum = 0;
    for (uint16_t i = 0; i < kCalibrationSamples; ++i) {
      sum += read_raw();
      sleep_us(500);
    }

    zero_counts_ = static_cast<float>(sum) / kCalibrationSamples;
    filtered_counts_ = zero_counts_;
    active_ = false;
  }

  bool sample() {
    const uint16_t raw = read_raw();
    filtered_counts_ += (static_cast<float>(raw) - filtered_counts_) * kSignalAlpha;
    const float deflection = direction_ * (filtered_counts_ - zero_counts_);

    if (active_) {
      active_ = deflection > kReleaseThresholdCounts;
    } else {
      active_ = deflection > kPressThresholdCounts;
    }

    if (!active_) {
      zero_counts_ += (filtered_counts_ - zero_counts_) * kZeroTrackingAlpha;
    }

    last_raw_ = raw;
    last_deflection_ = static_cast<int32_t>(deflection);
    return active_;
  }

  uint16_t last_raw() const { return last_raw_; }
  int32_t last_deflection() const { return last_deflection_; }

 private:
  uint16_t read_raw() const {
    adc_select_input(adc_channel_);
    sleep_us(3);
    return adc_read();
  }

  uint gpio_;
  uint adc_channel_;
  int direction_;
  bool active_ = false;
  uint16_t last_raw_ = 0;
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
  adc_init();

  gpio_init(kKeyOutGpio);
  gpio_set_dir(kKeyOutGpio, GPIO_OUT);
  gpio_put(kKeyOutGpio, 0);

  gpio_init(kStatusLedGpio);
  gpio_set_dir(kStatusLedGpio, GPIO_OUT);
  gpio_put(kStatusLedGpio, 0);

  setup_sidetone_pwm();

  AnalogPaddle dot_paddle(kDotAdcGpio, kDotAdcChannel, 1);
  AnalogPaddle dash_paddle(kDashAdcGpio, kDashAdcChannel, 1);
  dot_paddle.init();
  dash_paddle.init();

  sleep_ms(1000);
  printf("Pico strain-gauge Morse keyer: keep both paddles released for calibration.\n");
  dot_paddle.calibrate();
  dash_paddle.calibrate();
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
      printf("dot raw=%u def=%ld active=%u | dash raw=%u def=%ld active=%u\n",
             dot_paddle.last_raw(), static_cast<long>(dot_paddle.last_deflection()),
             dot_pressed ? 1 : 0, dash_paddle.last_raw(),
             static_cast<long>(dash_paddle.last_deflection()), dash_pressed ? 1 : 0);
    }

    sleep_ms(1);
  }
}
