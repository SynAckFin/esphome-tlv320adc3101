#include "tlv320adc3101.h"
#include "tlv320adc3101_const.h"
#include "esphome/core/hal.h"
#include "esphome/core/log.h"
#include <cinttypes>

#define ARRAY_SIZE(a) (sizeof((a)) / sizeof((a)[0]))
namespace esphome {
  namespace tlv320adc3101 {
    
    static const char *const TAG = "tlv320adc3101";
    void TLV320::dump_config() {
      ESP_LOGCONFIG(TAG,
                    "TLV320 audio ADC:\n"
                    "  Bits Per Sample: %" PRIu8 "\n"
                    "  Sample Rate: %" PRIu32 "\n"
                    "  MCLK Frequency: %i\n"
                    "  I2S Mode: %s\n"
                    "  Priority: %f\n",
                    this->bits_per_sample_,
                    this->sample_rate_,
                    this->mclk_frequency_,
                    this->isprimary_ ? "primary" : "secondary",
                    this->get_setup_priority()
                    );
      LOG_PIN("  Reset Pin: ", this->reset_pin_);
      LOG_I2C_DEVICE(this);
      this->dump_channel_config("Left",TLV320_LEFT_CHANNEL);
      this->dump_channel_config("Right",TLV320_RIGHT_CHANNEL);
      if (this->is_failed()) {
        ESP_LOGE(TAG, "  Failed to initialize");
        return;
      }
    }
    void TLV320::dump_channel_config(const char *name,int channel) {
      const char *src;
      switch(this->channel[channel].source) {
        case TLV320_SEL1:  src = "SEL1"; break;
        case TLV320_SEL2:  src = "SEL2"; break;
        case TLV320_SEL3:  src = "SEL3"; break;
        case TLV320_SEL4:  src = "SEL4"; break;
        case TLV320_SEL1X: src = "SEL1X"; break;
        case TLV320_SEL2X: src = "SEL2X"; break;
        case TLV320_SEL3X: src = "SEL3X"; break;
        default: src = "Invalid"; break;
      };
      const char *bias;
      switch(this->channel[channel].micbias) {
        case TLV320_MICBIAS_NONE: bias = "None"; break;
        case TLV320_MICBIAS_2V:   bias = "2V"; break;
        case TLV320_MICBIAS_2_5V: bias = "2.5V"; break;
        case TLV320_MICBIAS_AVDD: bias = "AVDD"; break;
        default: bias = "Invalid"; break;
      };
      ESP_LOGCONFIG(TAG,"  %s\n",name);
      ESP_LOGCONFIG(TAG,"    source: %s \n",src);
      ESP_LOGCONFIG(TAG,"    micbias: %s\n",bias);
      ESP_LOGCONFIG(TAG,"    gain: %.1f\n",this->channel[channel].gain);
    }
    float TLV320::get_setup_priority() const {
      // The reset pin might be connected to multiple
      // tlv320s so make sure that the instance that has
      // the pin defined runs BEFORE those that dont.
      if(this->reset_pin_ != nullptr) {
        return setup_priority::DATA + 1.0;
      }
      return setup_priority::DATA;
    }
    void TLV320::setup() {
      // Setup GPIO pin
      if(this->reset_pin_ != nullptr)
        this->reset_pin_->setup();
      this->hard_reset_();
      this->soft_reset_();
      this->setup_pll_();
      this->setup_i2s_format_();
      this->setup_adc_();
      this->setup_mic_bias_();
      this->setup_mic_src_();
      this->setup_mic_gain_();
      this->setup_misc_();
    }
    bool TLV320::set_mic_gain(int channel,float mic_gain) {
      uint8_t gain;
      channel &= 0x01;
      this->channel[channel].gain = mic_gain * TLV_MAX_GAIN;
      return this->setup_mic_gain_();
    }
    bool TLV320::set_mic_gain(float mic_gain) {
      this->channel[TLV320_LEFT_CHANNEL].gain = mic_gain * TLV_MAX_GAIN;
      this->channel[TLV320_RIGHT_CHANNEL].gain = mic_gain * TLV_MAX_GAIN;
      return setup_mic_gain_();
    }
    float TLV320::mic_gain() {
      float sum_gain = 0;
      int max_gain = 0;
      if(this->channel[TLV320_LEFT_CHANNEL].enabled) {
        sum_gain += this->channel[TLV320_LEFT_CHANNEL].gain;
        max_gain += TLV_MAX_GAIN;
      }
      if(this->channel[TLV320_RIGHT_CHANNEL].enabled) {
        sum_gain += this->channel[TLV320_RIGHT_CHANNEL].gain;
        max_gain += TLV_MAX_GAIN;
      }
      return max_gain ? sum_gain / (float) max_gain : 0.0;
    }
    bool TLV320::hard_reset_() {
      if(this->reset_pin_ != nullptr) {
        this->reset_pin_->digital_write(false);
        delay(1);
        this->reset_pin_->digital_write(true);
      }
      delay(1);
      return true;
    }
    bool TLV320::soft_reset_() {
      return this->write_byte(TLV_RESET, 0x01);
    }
    bool TLV320::setup_mic_gain_() {
      // set Left and Right Gain
      uint8_t gain;

      gain = (uint8_t) (this->channel[TLV320_LEFT_CHANNEL].gain * 2 + 0.001);
      gain &= 0x7f;
      this->update_reg_(TLV_LEFT_APGA_CTRL,gain);

      gain = (uint8_t) (this->channel[TLV320_RIGHT_CHANNEL].gain * 2 + 0.001);
      gain &= 0x7f;
      this->update_reg_(TLV_RIGHT_APGA_CTRL,gain);

      return true;
    }
    bool TLV320::setup_mic_src_() {
      // Left MIC is Connected to IN2L(P) and IN3L(M) as differential Pair
      uint8_t sel1,sel2;
      switch(this->channel[TLV320_LEFT_CHANNEL].source) {
        case TLV320_SEL1:  sel1 = 0xfc; sel2 = 0x3f; break;
        case TLV320_SEL2:  sel1 = 0xf3; sel2 = 0x3f; break;
        case TLV320_SEL3:  sel1 = 0xcf; sel2 = 0x3f; break;
        case TLV320_SEL4:  sel1 = 0x3f; sel2 = 0x3f; break;
        case TLV320_SEL1X: sel1 = 0xff; sel2 = 0x3c; break;
        case TLV320_SEL2X: sel1 = 0xff; sel2 = 0x33; break;
        case TLV320_SEL3X: sel1 = 0xff; sel2 = 0x0f; break;
        default:
          sel1 = 0xff; sel2 = 0x3f;
          ESP_LOGE(TAG, "Invalid Left mic source");
          break;
      }
      this->update_reg_(TLV_LEFT_PGA_SEL_1,sel1);
      this->update_reg_(TLV_LEFT_PGA_SEL_2,sel2);
      switch(this->channel[TLV320_RIGHT_CHANNEL].source) {
        case TLV320_SEL1:  sel1 = 0xfc; sel2 = 0x3f; break;
        case TLV320_SEL2:  sel1 = 0xf3; sel2 = 0x3f; break;
        case TLV320_SEL3:  sel1 = 0xcf; sel2 = 0x3f; break;
        case TLV320_SEL4:  sel1 = 0x3f; sel2 = 0x3f; break;
        case TLV320_SEL1X: sel1 = 0xff; sel2 = 0x3c; break;
        case TLV320_SEL2X: sel1 = 0xff; sel2 = 0x33; break;
        case TLV320_SEL3X: sel1 = 0xff; sel2 = 0x0f; break;
        default:
          sel1 = 0xff; sel2 = 0x3f;
          ESP_LOGE(TAG, "Invalid Right mic source");
          break;
      }
      this->update_reg_(TLV_RIGHT_PGA_SEL_1,sel1);
      this->update_reg_(TLV_RIGHT_PGA_SEL_2,sel2);
      return true;
    }
    bool TLV320::setup_mic_bias_() {
      // Left MIC is Connected to IN2L(P) and IN3L(M) as differential Pair
      uint8_t bias = 0;
      switch(this->channel[TLV320_LEFT_CHANNEL].micbias) {
        case TLV320_MICBIAS_NONE: bias |= 0x00; break;
        case TLV320_MICBIAS_2V:   bias |= 0x08; break;
        case TLV320_MICBIAS_2_5V: bias |= 0x10; break;
        case TLV320_MICBIAS_AVDD: bias |= 0x18; break;
        default:
          ESP_LOGE(TAG, "Invalid Left MIC Bias value");
          break;
      }
      switch(this->channel[TLV320_RIGHT_CHANNEL].micbias) {
        case TLV320_MICBIAS_NONE: bias |= 0x00; break;
        case TLV320_MICBIAS_2V:   bias |= 0x20; break;
        case TLV320_MICBIAS_2_5V: bias |= 0x40; break;
        case TLV320_MICBIAS_AVDD: bias |= 0x60; break;
        default:
          ESP_LOGE(TAG, "Invalid Left MIC Bias value");
          break;
      }
      this->update_reg_(TLV_MICBIAS_CTRL,bias);
      return true;
    }
    bool TLV320::setup_adc_() {
      // Power up Left and Right ADC if enabled and disable soft stepping
      uint8_t adc_digital = 0x00;  // Everything off
      if(this->channel[TLV320_LEFT_CHANNEL].enabled)
        adc_digital |= 0x80;
      if(this->channel[TLV320_RIGHT_CHANNEL].enabled)
        adc_digital |= 0x40;
      // Disable soft stepping
      adc_digital |= 0x02;
      this->update_reg_(TLV_ADC_DIGITAL,adc_digital);
      return true;
    }
    bool TLV320::setup_misc_() {
      // Unmute the left and right channels
      this->update_reg_(TLV_ADC_FGA,0x00);
      // Automatic gain control isn't used, but these
      // 2 registers default to an invalid value which
      // irks me so they are getting reset to 0
      this->update_reg_(TLV_LEFT_CHN_AGC_3,0x00);
      this->update_reg_(TLV_RIGHT_CHN_AGC_3,0x00);
      return true;
    }
    bool TLV320::setup_i2s_format_() {
      // set bits per sample
      uint8_t reg_val = 0;
      switch (this->bits_per_sample_) {
        case TLV320_BITS_PER_SAMPLE_16:
          reg_val = 0x00;
          break;
        case TLV320_BITS_PER_SAMPLE_20:
          reg_val = 0x01;
          break;
        case TLV320_BITS_PER_SAMPLE_24:
          reg_val = 0x02;
          break;
        case TLV320_BITS_PER_SAMPLE_32:
          reg_val = 0x03;
          break;
        default:
          return false;
      }
      reg_val <<= 4;
      reg_val |= this->reg_values.adc_ctrl_1 & 0xcf;
      // Set I2S mode
      reg_val &= 0x3f;
      // Enable 3-stating
      reg_val |= 0x01;
      // Save it
      this->reg_values.adc_ctrl_1 = reg_val;
      this->update_reg_(TLV_INTERFACE_CTRL_1,this->reg_values.adc_ctrl_1);
      // Left as defaults
      // BCLK Not inverted
      // BCLK and WCLK active when codec powered down
      // BDIV_CLKIN = TLV_CLK
      return true;
    }
    bool TLV320::update_reg_(uint16_t reg_addr, uint8_t data) {
      uint8_t page = reg_addr >> 8;
      uint8_t reg  = reg_addr & 0xff;
      this->update_page_(page);
      return this->write_byte(reg, data);
    }
    bool TLV320::update_page_(uint8_t page) {
      if(page == this->reg_values.page)
        return true;
      this->reg_values.page = page;
      return this->write_byte(TLV_PAGE_SELECT,this->reg_values.page);
    }
    bool TLV320::setup_pll_() {
      uint32_t pll_clk;
      if(this->isprimary_)
        pll_clk = this->mclk_frequency_;
      else
        pll_clk = this->sample_rate_ * this->bits_per_sample_ * 2;
  
      const struct adc_rate_divs *adcdiv = nullptr;
      // Find matching settings for pll_clk and sample rate
      for(int i=0;ARRAY_SIZE(adc_divs); i++) {
        if( adc_divs[i].mclk == pll_clk &&
            adc_divs[i].rate == this->sample_rate_ ) {
          adcdiv = &adc_divs[i];
          break;
        }
      }
      if(adcdiv == nullptr) {
        ESP_LOGE(TAG, "Unsupported MCLK Frequency and sample rate");
        this->mark_failed();
        return false;
      }

      int tmp_reg;

      // Set P value
      tmp_reg = this->reg_values.pll_p_r;
      tmp_reg &= 0x8f;
      tmp_reg |= (adcdiv->pll_p & 0x07) << 4;
      // Set R value
      tmp_reg &= 0xf0;
      tmp_reg |= (adcdiv->pll_r & 0x0f);
      this->reg_values.pll_p_r = tmp_reg;
      this->update_reg_(TLV_PLL_PROG_PR,this->reg_values.pll_p_r);

      // Set J value
      this->reg_values.pll_j = adcdiv->pll_j & 0x7f;
      this->update_reg_(TLV_PLL_PROG_J,this->reg_values.pll_j);

      // Set D value (MSB MUST be first)
      this->reg_values.pll_d_msb = (adcdiv->pll_d & 0x3fff) >> 8;
      this->update_reg_(TLV_PLL_PROG_D_MSB,this->reg_values.pll_d_msb);
      this->reg_values.pll_d_lsb = adcdiv->pll_d & 0xff;
      this->update_reg_(TLV_PLL_PROG_D_LSB,this->reg_values.pll_d_lsb);

      // Set NADC
      this->reg_values.pll_nadc = adcdiv->nadc & 0x7f;
      this->update_reg_(TLV_ADC_NADC, this->reg_values.pll_nadc);

      // Set MADC
      this->reg_values.pll_madc = adcdiv->madc & 0x7f;
      this->update_reg_(TLV_ADC_MADC, this->reg_values.pll_madc);

      // Set AOSR
      this->reg_values.aosr = adcdiv->aosr;
      this->update_reg_(TLV_ADC_AOSR, this->reg_values.aosr);

      // Set BCLK N Divider
      int n = (adcdiv->aosr * adcdiv->madc) / (2 * this->bits_per_sample_);
      this->reg_values.bclk_n_div =  (n & 0x7f) | (this->reg_values.bclk_n_div & 0x80);
      this->update_reg_(TLV_BCLK_N_DIV, this->reg_values.bclk_n_div);

      // Set Codec Clock input to PLL
      this->reg_values.clk_gen_mux &= 0xfc;
      this->reg_values.clk_gen_mux |= 0x03;
      this->update_reg_(TLV_CLKGEN_MUX,this->reg_values.clk_gen_mux);

      // Power up MADC
      this->reg_values.pll_madc |= 0x80;
      this->update_reg_(TLV_ADC_MADC, this->reg_values.pll_madc);
      // Power up NADC
      this->reg_values.pll_nadc |= 0x80;
      this->update_reg_(TLV_ADC_NADC, this->reg_values.pll_nadc);

      if(this->isprimary_) {
        // Set PLL clock input to MCLK
        this->reg_values.clk_gen_mux &= 0xf3;
        this->update_reg_(TLV_CLKGEN_MUX,this->reg_values.clk_gen_mux);
        // Power up BCLK N Divider
        this->reg_values.bclk_n_div |= 0x80;
        this->update_reg_(TLV_BCLK_N_DIV, this->reg_values.bclk_n_div);
        // Set BCLK and WCLK as output
        this->reg_values.adc_ctrl_1 |= 1 << 3;     // BCLK
        this->reg_values.adc_ctrl_1 |= 1 << 2;     // WCLK
        this->update_reg_(TLV_INTERFACE_CTRL_1,this->reg_values.adc_ctrl_1);
      } else {
        // Set PLL clock input to BCLK
        this->reg_values.clk_gen_mux &= 0xf3;
        this->reg_values.clk_gen_mux |= 0x04;
        this->update_reg_(TLV_CLKGEN_MUX,this->reg_values.clk_gen_mux);
        // Set BCLK and WCLK as input
        this->reg_values.adc_ctrl_1 &= 0xf7;     // BCLK
        this->reg_values.adc_ctrl_1 &= 0xfb;     // WCLK
        this->update_reg_(TLV_INTERFACE_CTRL_1,this->reg_values.adc_ctrl_1);
      }
      // Power up PLL
      this->reg_values.pll_p_r |= 0x80;
      this->update_reg_(TLV_PLL_PROG_PR,this->reg_values.pll_p_r);

      return true;
    }
  }  // namespace tlv320adc3101
}  // namespace esphome
