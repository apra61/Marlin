#include "cs1237.hpp"

#include "pico/stdlib.h"

namespace {
constexpr uint8_t kCommandBits = 7;
constexpr uint32_t kClockDelayUs = 2;
}  // namespace

Cs1237::Cs1237(unsigned int dout_gpio, unsigned int sclk_gpio)
    : dout_gpio_(dout_gpio), sclk_gpio_(sclk_gpio) {}

void Cs1237::init() const {
  gpio_init(sclk_gpio_);
  gpio_set_dir(sclk_gpio_, GPIO_OUT);
  gpio_put(sclk_gpio_, 0);

  gpio_init(dout_gpio_);
  set_data_input();
}

bool Cs1237::write_config(uint8_t config, uint32_t timeout_us) {
  int32_t discarded = 0;
  if (!read_raw_blocking(&discarded, timeout_us)) {
    return false;
  }

  set_data_output();
  gpio_put(dout_gpio_, 0);

  pulse_idle();  // 28
  pulse_idle();  // 29
  write_command(kCommandWriteConfig);
  pulse_idle();  // 37
  write_byte(config);
  pulse_idle();  // 46

  set_data_input();
  return true;
}

bool Cs1237::read_config(uint8_t* config, uint32_t timeout_us) {
  if (config == nullptr) {
    return false;
  }

  int32_t discarded = 0;
  if (!read_raw_blocking(&discarded, timeout_us)) {
    return false;
  }

  set_data_output();
  gpio_put(dout_gpio_, 0);

  pulse_idle();  // 28
  pulse_idle();  // 29
  write_command(kCommandReadConfig);
  pulse_idle();  // 37

  set_data_input();
  *config = read_byte();
  pulse_idle();  // 46

  set_data_input();
  return true;
}

bool Cs1237::read_raw_blocking(int32_t* value, uint32_t timeout_us) {
  if (value == nullptr || !wait_ready(timeout_us)) {
    return false;
  }

  *value = sign_extend24(read_unsigned_frame());
  return true;
}

bool Cs1237::read_raw_nonblocking(int32_t* value) {
  if (value == nullptr || gpio_get(dout_gpio_) != 0) {
    return false;
  }

  *value = sign_extend24(read_unsigned_frame());
  return true;
}

bool Cs1237::wait_ready(uint32_t timeout_us) const {
  const absolute_time_t timeout = make_timeout_time_us(timeout_us);
  while (gpio_get(dout_gpio_) != 0) {
    if (time_reached(timeout)) {
      return false;
    }
    tight_loop_contents();
  }
  return true;
}

uint32_t Cs1237::read_unsigned_frame() const {
  uint32_t raw = 0;
  set_data_input();

  for (uint8_t i = 0; i < 24; ++i) {
    raw = (raw << 1) | (pulse_read_bit() ? 1U : 0U);
  }

  // CS1237 accepts 24 data clocks plus three extra clocks for simple reads.
  pulse_idle();
  pulse_idle();
  pulse_idle();
  return raw;
}

uint8_t Cs1237::read_byte() const {
  uint8_t value = 0;
  for (uint8_t i = 0; i < 8; ++i) {
    value = static_cast<uint8_t>((value << 1) | (pulse_read_bit() ? 1U : 0U));
  }
  return value;
}

bool Cs1237::pulse_read_bit() const {
  gpio_put(sclk_gpio_, 1);
  sleep_us(kClockDelayUs);
  const bool bit = gpio_get(dout_gpio_) != 0;
  gpio_put(sclk_gpio_, 0);
  sleep_us(kClockDelayUs);
  return bit;
}

void Cs1237::pulse_idle() const {
  gpio_put(sclk_gpio_, 1);
  sleep_us(kClockDelayUs);
  gpio_put(sclk_gpio_, 0);
  sleep_us(kClockDelayUs);
}

void Cs1237::pulse_write_bit(bool bit) const {
  gpio_put(sclk_gpio_, 1);
  sleep_us(kClockDelayUs);
  gpio_put(dout_gpio_, bit ? 1 : 0);
  gpio_put(sclk_gpio_, 0);
  sleep_us(kClockDelayUs);
}

void Cs1237::write_command(uint8_t command) const {
  for (int8_t bit = kCommandBits - 1; bit >= 0; --bit) {
    pulse_write_bit(((command >> bit) & 0x01U) != 0);
  }
}

void Cs1237::write_byte(uint8_t value) const {
  for (int8_t bit = 7; bit >= 0; --bit) {
    pulse_write_bit(((value >> bit) & 0x01U) != 0);
  }
}

void Cs1237::set_data_input() const {
  gpio_set_dir(dout_gpio_, GPIO_IN);
  gpio_pull_up(dout_gpio_);
}

void Cs1237::set_data_output() const {
  gpio_disable_pulls(dout_gpio_);
  gpio_set_dir(dout_gpio_, GPIO_OUT);
}
