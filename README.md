# TLV320ADC3101 Stereo ADC With miniDSP

This platform provides a flexible `audio_adc` component for use with analogue microphones. It can be configured as either a `primary` or `secondary` for an `i2s_audio` source.

The TLV320ADC3101 ADC is used in several generations of Amazon Echo and this component is primarily intended to make use of the ADC as it is used in the Echo. The default settings should work with a 2nd Gen Amazon Echo microphone board.

    audio_adc:
      - platform: tlv320adc3101
        id: tlv320_adc
        pin_reset: GPIO17
        i2s_mode: secondary
        mclk: 4096000
        address: 0x18
        mic_gain: 100%
        channel_left:
          source: SEL4
          micbias: 2.5V
          mic_gain: 100%
          enabled: true
        channel_right:
          source: SEL4
          micbias: 2.5V
          mic_gain: 100%
          enabled: true

## Configuration variables

 - pin_reset (Optional, Pin Schema): The GPIO used to reset the chip. Defining this will have the side effect of increasing the setup priority of this instance so that if multiple chip's share the same GPIO this is run before the others. There should be at least one TLV320ADC3101 device with this defined.
 - i2s_mode (Optional, enum): The I²S mode to use. One of primary (clock driven by this chip) or secondary (clock driven by other source). Defaults to secondary
 - mclk (Optional, int): The frequency of the master clock. This is only used when this is `primary`. Default 4096000.
 - address (Optional, int): The I²C address. Defaults to 24
 - mic_gain (Optional, percent): The microphone gain setting for both microphones. This can be overridden by the setting in `channel_left` and/or `channel_right`. Default 100%
 - channel_left (Optional): Left channel configuration
   - source (Optional, enum): Analogue source for the left channel (see [datasheet](https://www.ti.com/lit/gpn/TLV320ADC3101 "datasheet")). One of `SEL1`, `SEL2`, `SEL3`, `SEL4`, `SEL1X`, `SEL2X`, `SEL3X`. Default `SEL4`
   - micbias (Optional, enum): Bias voltage of the microphone. One of `none`, `2.0V`,`2.5V`, `AVDD`. Default `2.5V`
   - mic_gain (Optional, percent): The microphone gain setting for left microphone.
   - enabled (Optional, boolean): Microphone is enabled (`true`) of disabled (`false`). Default `true`
 - channel_right (Optional): Right channel configuration
   - source (Optional, enum): Analogue source for the right channel (see [datasheet](https://www.ti.com/lit/gpn/TLV320ADC3101 "datasheet")). One of `SEL1`, `SEL2`, `SEL3`, `SEL4`, `SEL1X`, `SEL2X`, `SEL3X`. Default `SEL4`
   - micbias (Optional, enum): Bias voltage of the microphone. One of `none`, `2.0V`,`2.5V`, `AVDD`. Default `2.5V`
   - mic_gain (Optional, percent): The microphone gain setting for right microphone.
   - enabled (Optional, boolean): Microphone is enabled (`true`) of disabled (`false`). Default `true`
