#pragma once

#include "esphome/components/audio_adc/audio_adc.h"
#include "esphome/components/i2c/i2c.h"
#include "esphome/core/component.h"

#include "tlv320adc3101_const.h"

namespace esphome {
namespace tlv320adc3101 {

enum TLV320Channel {
    TLV320_RIGHT_CHANNEL = 0,
    TLV320_LEFT_CHANNEL = 1,
};
enum TLV320BitsPerSample : uint8_t {
    TLV320_BITS_PER_SAMPLE_16 = 16,
    TLV320_BITS_PER_SAMPLE_20 = 20,
    TLV320_BITS_PER_SAMPLE_24 = 24,
    TLV320_BITS_PER_SAMPLE_32 = 32,
};
enum TLV320ChannelSource : uint8_t {
    TLV320_SEL1 = 1,
    TLV320_SEL2,
    TLV320_SEL3,
    TLV320_SEL4,
    TLV320_SEL1X,
    TLV320_SEL2X,
    TLV320_SEL3X,
};
enum TLV320MicBias : uint8_t {
    TLV320_MICBIAS_NONE = 0,
    TLV320_MICBIAS_2V,
    TLV320_MICBIAS_2_5V,
    TLV320_MICBIAS_AVDD,
};

class TLV320 : public audio_adc::AudioAdc, public Component, public i2c::I2CDevice {
    public:
      void setup() override;
      void dump_config() override;
      void dump_channel_config(const char *name, int channel);
      float get_setup_priority() const override;

      // This is required by the spec and is a percentage 0.0 -> 1.0
      bool set_mic_gain(float mic_gain) override;
      bool set_mic_gain(int channel,float mic_gain);
      // Required by spec and will be an average of enabled channels
      float mic_gain() override;

      //
      // These functions should only be called prior to setup()
      // and will only modify internal variables and not the chip.
      // When setup() is called it will use these settings to
      // configure the chip.
      // 
      void set_bits_per_sample(TLV320BitsPerSample bits_per_sample) {
        this->bits_per_sample_ = bits_per_sample;
      }
      void set_reset_pin(GPIOPin *pin_i) {
        this->reset_pin_ = pin_i;
      }
      void set_primary(bool isprimary) {
        this->isprimary_ = isprimary;
      }
      void set_mclk_frequency(uint32_t freq) {
        this->mclk_frequency_ = freq;
      }
      void set_sample_rate(uint32_t sample_rate) {
        this->sample_rate_ = sample_rate;
      }
      void set_channel_gain(int channel,float gain) {
        this->channel[channel].gain = gain * TLV_MAX_GAIN;
      }
      void set_channel_enabled(int channel,bool enabled) {
        this->channel[channel].enabled = enabled;
      }
      void set_channel_source(int channel,TLV320ChannelSource source) {
        this->channel[channel].source = source;
      }
      void set_channel_micbias(int channel,TLV320MicBias bias) {
        this->channel[channel].micbias = bias;
      }
    protected:
      GPIOPin *reset_pin_{nullptr};  // Reset pin.
      uint32_t mclk_frequency_;
      //float mic_gain_;
      bool isprimary_;
      TLV320BitsPerSample bits_per_sample_{TLV320_BITS_PER_SAMPLE_16};
      uint32_t sample_rate_{0};

      bool update_reg_(uint16_t reg_addr, uint8_t data);
      bool update_page_(uint8_t page);
    
      bool hard_reset_();
      bool soft_reset_();
      bool setup_sample_rate_();
      bool setup_pll_();
      bool setup_i2s_format_();
      bool setup_adc_();
      bool setup_mic_gain_();
      bool setup_mic_src_();
      bool setup_mic_bias_();
      bool setup_misc_();
    
      struct {
        uint8_t page{0x00};
        uint8_t clk_gen_mux{0x00};
        uint8_t pll_p_r{0x00};
        uint8_t pll_j{0x04};
        uint8_t pll_d_msb{0x00};
        uint8_t pll_d_lsb{0x00};
        uint8_t pll_nadc{0x01};
        uint8_t pll_madc{0x01};
        uint8_t aosr{0x80};
        uint8_t bclk_n_div{0x01};
        uint8_t adc_ctrl_1{0x00};
      } reg_values;
      struct {
        enum TLV320ChannelSource source{TLV320_SEL4};
        enum TLV320MicBias micbias{TLV320_MICBIAS_2_5V};
        float gain{TLV_MAX_GAIN};
        bool enabled;
      } channel[2];
};
}  // namespace tlv320adc3101
}  // namespace esphome
