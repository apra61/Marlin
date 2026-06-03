#pragma once

#include <cstdint>

class Cs1237 {
 public:
  static constexpr uint8_t kCommandWriteConfig = 0x65;
  static constexpr uint8_t kCommandReadConfig = 0x56;

  static constexpr uint8_t kChannelA = 0x00;
  static constexpr uint8_t kGain1 = 0x00;
  static constexpr uint8_t kGain2 = 0x04;
  static constexpr uint8_t kGain64 = 0x08;
  static constexpr uint8_t kGain128 = 0x0C;
  static constexpr uint8_t kRate10Hz = 0x00;
  static constexpr uint8_t kRate40Hz = 0x10;
  static constexpr uint8_t kRate640Hz = 0x20;
  static constexpr uint8_t kRate1280Hz = 0x30;
  static constexpr uint8_t kRefOutputEnabled = 0x00;

  Cs1237(unsigned int dout_gpio, unsigned int sclk_gpio);

  void init() const;
  bool write_config(uint8_t config, uint32_t timeout_us);
  bool read_config(uint8_t* config, uint32_t timeout_us);
  bool read_raw_blocking(int32_t* value, uint32_t timeout_us);
  bool read_raw_nonblocking(int32_t* value);

  static int32_t sign_extend24(uint32_t raw) {
    raw &= 0x00FFFFFFU;
    if ((raw & 0x00800000U) != 0) {
      raw |= 0xFF000000U;
    }
    return static_cast<int32_t>(raw);
  }

 private:
  bool wait_ready(uint32_t timeout_us) const;
  uint32_t read_unsigned_frame() const;
  uint8_t read_byte() const;
  bool pulse_read_bit() const;
  void pulse_idle() const;
  void pulse_write_bit(bool bit) const;
  void write_command(uint8_t command) const;
  void write_byte(uint8_t value) const;
  void set_data_input() const;
  void set_data_output() const;

  unsigned int dout_gpio_;
  unsigned int sclk_gpio_;
};
