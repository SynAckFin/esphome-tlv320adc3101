#pragma once

#include <cinttypes>

/* Register definitions. */

#define TLV_PAGE_SIZE      256
#define TLV_REG(page, reg) ((page * TLV_PAGE_SIZE) + reg)
#define TLV_MAX_REG        ( TLV_PAGE_SIZE * 5 )
#define TLV_PAGE_SELECT    TLV_REG(0, 0)
#define TLV_RESET          TLV_REG(0, 1)
/* 2-3 Reserved */
#define TLV_CLKGEN_MUX                 TLV_REG(0, 4)
#define TLV_PLL_PROG_PR                TLV_REG(0, 5)
#define TLV_PLL_PROG_J                 TLV_REG(0, 6)
#define TLV_PLL_PROG_D_MSB             TLV_REG(0, 7)
#define TLV_PLL_PROG_D_LSB             TLV_REG(0, 8)
#define TLV_ADC_NADC                   TLV_REG(0, 18)
#define TLV_ADC_MADC                   TLV_REG(0, 19)
#define TLV_ADC_AOSR                   TLV_REG(0, 20)
#define TLV_ADC_IADC                   TLV_REG(0, 21)
#define TLV_CLKOUT_MUX                 TLV_REG(0, 25)
#define TLV_CLKOUT_M_DIV               TLV_REG(0, 26)
#define TLV_INTERFACE_CTRL_1           TLV_REG(0, 27)
#define TLV_CH_OFFSET_1                TLV_REG(0, 28)
#define TLV_INTERFACE_CTRL_2           TLV_REG(0, 29)
#define TLV_BCLK_N_DIV                 TLV_REG(0, 30)
#define TLV_INTERFACE_CTRL_3           TLV_REG(0, 31)
#define TLV_INTERFACE_CTRL_4           TLV_REG(0, 32)
#define TLV_INTERFACE_CTRL_5           TLV_REG(0, 33)
#define TLV_I2S_SYNC                   TLV_REG(0, 34)
#define TLV_ADC_FLAG                   TLV_REG(0, 36)
#define TLV_CH_OFFSET_2                TLV_REG(0, 37)
#define TLV_I2S_TDM_CTRL               TLV_REG(0, 38)
#define TLV_INTR_FLAG_1                TLV_REG(0, 42)
#define TLV_INTR_FLAG_2                TLV_REG(0, 43)
#define TLV_INTR_FLAG_ADC1             TLV_REG(0, 45)
#define TLV_INTR_FLAG_ADC2             TLV_REG(0, 47)
#define TLV_INT1_CTRL                  TLV_REG(0, 48)
#define TLV_INT2_CTRL                  TLV_REG(0, 49)
#define TLV_GPIO2_CTRL                 TLV_REG(0, 51)
#define TLV_GPIO1_CTRL                 TLV_REG(0, 52)
#define TLV_DOUT_CTRL                  TLV_REG(0, 53)
#define TLV_SYNC_CTRL_1                TLV_REG(0, 57)
#define TLV_SYNC_CTRL_2                TLV_REG(0, 58)
#define TLV_CIC_GAIN_CTRL              TLV_REG(0, 59)
#define TLV_PRB_SELECT                 TLV_REG(0, 61)
#define TLV_INST_MODE_CTRL             TLV_REG(0, 62)
#define TLV_MIC_POLARITY_CTRL          TLV_REG(0, 80)
#define TLV_ADC_DIGITAL                TLV_REG(0, 81)
#define TLV_ADC_FGA                    TLV_REG(0, 82)
#define TLV_LADC_VOL                   TLV_REG(0, 83)
#define TLV_RADC_VOL                   TLV_REG(0, 84)
#define TLV_ADC_PHASE_COMP             TLV_REG(0, 85)
#define TLV_LEFT_CHN_AGC_1             TLV_REG(0, 86)
#define TLV_LEFT_CHN_AGC_2             TLV_REG(0, 87)
#define TLV_LEFT_CHN_AGC_3             TLV_REG(0, 88)
#define TLV_LEFT_CHN_AGC_4             TLV_REG(0, 89)
#define TLV_LEFT_CHN_AGC_5             TLV_REG(0, 90)
#define TLV_LEFT_CHN_AGC_6             TLV_REG(0, 91)
#define TLV_LEFT_CHN_AGC_7             TLV_REG(0, 92)
#define TLV_LEFT_AGC_GAIN              TLV_REG(0, 93)
#define TLV_RIGHT_CHN_AGC_1            TLV_REG(0, 94)
#define TLV_RIGHT_CHN_AGC_2            TLV_REG(0, 95)
#define TLV_RIGHT_CHN_AGC_3            TLV_REG(0, 96)
#define TLV_RIGHT_CHN_AGC_4            TLV_REG(0, 97)
#define TLV_RIGHT_CHN_AGC_5            TLV_REG(0, 98)
#define TLV_RIGHT_CHN_AGC_6            TLV_REG(0, 99)
#define TLV_RIGHT_CHN_AGC_7            TLV_REG(0, 100)
#define TLV_RIGHT_AGC_GAIN             TLV_REG(0, 101)
/*
 * Page 1 registers.
 */
#define TLV_DITHER_CTRL                TLV_REG(1, 26)
#define TLV_MICBIAS_CTRL               TLV_REG(1, 51)
#define TLV_LEFT_PGA_SEL_1             TLV_REG(1, 52)
#define TLV_LEFT_PGA_SEL_2             TLV_REG(1, 54)
#define TLV_RIGHT_PGA_SEL_1            TLV_REG(1, 55)
#define TLV_RIGHT_PGA_SEL_2            TLV_REG(1, 57)
#define TLV_LEFT_APGA_CTRL             TLV_REG(1, 59)
#define TLV_RIGHT_APGA_CTRL            TLV_REG(1, 60)
#define TLV_LOW_CURRENT_MODES          TLV_REG(1, 61)
#define TLV_ANALOG_PGA_FLAGS           TLV_REG(1, 62)
/*
 * Page 4 registers. First page of coefficient memory for the miniDSP.
 */
#define TLV_LEFT_ADC_IIR_COEFF_N0_MSB  TLV_REG(4, 8)
#define TLV_LEFT_ADC_IIR_COEFF_N0_LSB  TLV_REG(4, 9)
#define TLV_LEFT_ADC_IIR_COEFF_N1_MSB  TLV_REG(4, 10)
#define TLV_LEFT_ADC_IIR_COEFF_N1_LSB  TLV_REG(4, 11)
#define TLV_LEFT_ADC_IIR_COEFF_D1_MSB  TLV_REG(4, 12)
#define TLV_LEFT_ADC_IIR_COEFF_D1_LSB  TLV_REG(4, 13)
#define TLV_RIGHT_ADC_IIR_COEFF_N0_MSB TLV_REG(4, 72)
#define TLV_RIGHT_ADC_IIR_COEFF_N0_LSB TLV_REG(4, 73)
#define TLV_RIGHT_ADC_IIR_COEFF_N1_MSB TLV_REG(4, 74)
#define TLV_RIGHT_ADC_IIR_COEFF_N1_LSB TLV_REG(4, 75)
#define TLV_RIGHT_ADC_IIR_COEFF_D1_MSB TLV_REG(4, 76)
#define TLV_RIGHT_ADC_IIR_COEFF_D1_LSB TLV_REG(4, 77)

#define TLV_MAX_GAIN	40

namespace esphome {
namespace tlv320adc3101 {
struct adc_rate_divs {
	uint32_t mclk;
	uint32_t rate;
	uint8_t pll_p;
	uint8_t pll_r;
	uint8_t pll_j;
	uint16_t pll_d;
	uint8_t nadc;
	uint8_t madc;
	uint8_t aosr;
};

static const struct adc_rate_divs adc_divs[] = {
	/*   mclk,   rate, p, r,  j,    d, nadc, madc, aosr */
    { 2048000,  16000, 1, 1, 44, 0000,   22,    2,  128 },
    { 4096000,  16000, 1, 1, 20, 0000,   20,    2,  128 },
    { 6144000,  16000, 1, 1, 16, 0000,   24,    2,  128 },
    { 8192000,  16000, 1, 1, 15, 0000,   30,    2,  128 },
	// When it is secondary the BLCK is source for the
	// PLL and its frequency is usually bits*sample_rate*channels
	// So the following are for when it is secondary
	{  512000,  16000, 1, 6, 32, 0000,    3,   16,  128 }
};

}  // namespace tlv320adc3101
}  // namespace esphome
