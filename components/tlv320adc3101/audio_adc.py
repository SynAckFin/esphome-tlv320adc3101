import esphome.codegen as cg
from esphome.components import i2c
from esphome.components import i2s_audio
from esphome.components.audio_adc import AudioAdc
from esphome import pins
import esphome.config_validation as cv
from esphome.components.const import CONF_ENABLED
from esphome.const import (
    CONF_BITS_PER_SAMPLE,
    CONF_ID,
    CONF_MIC_GAIN,
    CONF_SAMPLE_RATE,
    CONF_SOURCE
)
from esphome.components.i2s_audio import (
    CONF_I2S_MODE,
    CONF_PRIMARY,
    CONF_SECONDARY
)
CONF_MCLK_FREQUENCY = "mclk"
CONF_PIN_RESET = "pin_reset"
CONF_CHANNEL_LEFT = "channel_left"
CONF_CHANNEL_RIGHT = "channel_right"
CONF_MICBIAS = "micbias"

CODEOWNERS = ["@SynAckFin"]
DEPENDENCIES = ["i2c"]

tlv320adc3101_ns = cg.esphome_ns.namespace("tlv320adc3101")
TLV320 = tlv320adc3101_ns.class_("TLV320", AudioAdc, cg.Component, i2c.I2CDevice)


tlv320_channel = tlv320adc3101_ns.enum("TLV320Channel")
TLV320_CHANNEL_ENUM = {
    "CHANNEL_RIGHT": tlv320_channel.TLV320_RIGHT_CHANNEL,
    "CHANNEL_LEFT": tlv320_channel.TLV320_LEFT_CHANNEL,
}
tlv320_bps = tlv320adc3101_ns.enum("TLV320BitsPerSample")
TLV320_BPS_ENUM = {
    16: tlv320_bps.TLV320_BITS_PER_SAMPLE_16,
    24: tlv320_bps.TLV320_BITS_PER_SAMPLE_24,
    32: tlv320_bps.TLV320_BITS_PER_SAMPLE_32,
}

tlv320_channel_src = tlv320adc3101_ns.enum("TLV320ChannelSource")
TLV320_CHANNEL_SRC_ENUM = {
    "SEL1":  tlv320_channel_src.TLV320_SEL1,
    "SEL2":  tlv320_channel_src.TLV320_SEL2,
    "SEL3":  tlv320_channel_src.TLV320_SEL3,
    "SEL4":  tlv320_channel_src.TLV320_SEL4,
    "SEL1X": tlv320_channel_src.TLV320_SEL1X,
    "SEL2X": tlv320_channel_src.TLV320_SEL2X,
    "SEL3X": tlv320_channel_src.TLV320_SEL3X,
}
tlv320_micbias = tlv320adc3101_ns.enum("TLV320MicBias")
TLV320_MICBIAS_ENUM = {
    "none":  tlv320_micbias.TLV320_MICBIAS_NONE,
    "2V":  tlv320_micbias.TLV320_MICBIAS_2V,
    "2.5V":  tlv320_micbias.TLV320_MICBIAS_2_5V,
    "AVDD":  tlv320_micbias.TLV320_MICBIAS_AVDD,
}


_validate_bits = cv.float_with_unit("bits", "bit")
_channel_schema = cv.Schema(
    {
        cv.Optional(CONF_MIC_GAIN): cv.percentage,
        cv.Optional(CONF_SOURCE, default = "SEL4"): cv.enum(TLV320_CHANNEL_SRC_ENUM),
        cv.Optional(CONF_ENABLED, default = True): cv.boolean, 
        cv.Optional(CONF_MICBIAS, default = "2.5V"): cv.enum(TLV320_MICBIAS_ENUM),
    },
)

CONFIG_SCHEMA = (
    cv.Schema(
        {
            cv.GenerateID(): cv.declare_id(TLV320),
            cv.Optional(CONF_PIN_RESET): pins.internal_gpio_output_pin_schema,
            cv.Optional(CONF_MCLK_FREQUENCY,default=4096000): cv.int_range(min=0),
            cv.Optional(CONF_BITS_PER_SAMPLE, default="16bit"): cv.All(
                _validate_bits, cv.enum(TLV320_BPS_ENUM),
            ),
            cv.Optional(CONF_SAMPLE_RATE, default=16000): cv.int_range(min=1),
            cv.Optional(CONF_MIC_GAIN): cv.percentage,
            cv.Optional(CONF_I2S_MODE, default=CONF_SECONDARY): cv.one_of(
                CONF_PRIMARY,CONF_SECONDARY, lower=True
            ),
            cv.Optional(CONF_CHANNEL_RIGHT, default={}): _channel_schema,
            cv.Optional(CONF_CHANNEL_LEFT, default={}): _channel_schema,
        }
    )
    .extend(cv.COMPONENT_SCHEMA)
    .extend(i2c.i2c_device_schema(0x18))
)


async def to_code(config):
    var = cg.new_Pvariable(config[CONF_ID])
    await cg.register_component(var, config)
    await i2c.register_i2c_device(var, config)

    cg.add(var.set_primary(config[CONF_I2S_MODE] == CONF_PRIMARY))

    cg.add(var.set_bits_per_sample(config[CONF_BITS_PER_SAMPLE]))
    cg.add(var.set_sample_rate(config[CONF_SAMPLE_RATE]))
    cg.add(var.set_mclk_frequency(config[CONF_MCLK_FREQUENCY]));

    cg.add(var.set_channel_source(tlv320_channel.TLV320_RIGHT_CHANNEL,config[CONF_CHANNEL_RIGHT][CONF_SOURCE]))
    cg.add(var.set_channel_micbias(tlv320_channel.TLV320_RIGHT_CHANNEL,config[CONF_CHANNEL_RIGHT][CONF_MICBIAS]))

    cg.add(var.set_channel_enabled(tlv320_channel.TLV320_RIGHT_CHANNEL,config[CONF_CHANNEL_RIGHT][CONF_ENABLED]))
    cg.add(var.set_channel_source(tlv320_channel.TLV320_LEFT_CHANNEL,config[CONF_CHANNEL_LEFT][CONF_SOURCE]))
    cg.add(var.set_channel_micbias(tlv320_channel.TLV320_LEFT_CHANNEL,config[CONF_CHANNEL_LEFT][CONF_MICBIAS]))
    cg.add(var.set_channel_enabled(tlv320_channel.TLV320_LEFT_CHANNEL,config[CONF_CHANNEL_LEFT][CONF_ENABLED]))
    if CONF_MIC_GAIN in config:
      cg.add(var.set_channel_gain(tlv320_channel.TLV320_LEFT_CHANNEL,config[CONF_MIC_GAIN]))
      cg.add(var.set_channel_gain(tlv320_channel.TLV320_RIGHT_CHANNEL,config[CONF_MIC_GAIN]))
    if CONF_MIC_GAIN in config[CONF_CHANNEL_LEFT]:
      cg.add(var.set_channel_gain(tlv320_channel.TLV320_LEFT_CHANNEL,config[CONF_CHANNEL_LEFT][CONF_MIC_GAIN]))
    if CONF_MIC_GAIN in config[CONF_CHANNEL_RIGHT]:
      cg.add(var.set_channel_gain(tlv320_channel.TLV320_RIGHT_CHANNEL,config[CONF_CHANNEL_RIGHT][CONF_MIC_GAIN]))
    if CONF_PIN_RESET in config:
      pin_i = await cg.gpio_pin_expression(config[CONF_PIN_RESET])
      cg.add(var.set_reset_pin(pin_i))
