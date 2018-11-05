**Tasmota-I2cRotaryEncoder**

This code is experimental and a work in progress - please do not ask for it to be included in Sonoff-Tasmota at this time.

You are welcome to test, but in order to do this you need to be able to compile Sonoff-Tasmota.

Once you can do this you can add the file:
https://github.com/andrethomas/I2cRotaryEncoder/blob/master/Sonoff-Tasmota%20Driver/xsns_91_I2cRotary.ino
to the /sonoff/ source code folder which will allow it to be included during compilation.

The rotary encoder needs only the Arduino sketch:
https://github.com/andrethomas/I2cRotaryEncoder/blob/master/I2CRotaryEncoder/I2CRotaryEncoder.ino

This needs to be flashed to an Arduino UNO, Arduino Pro-Mini, or compatible and then connected via I2C bus to the ESP8266 on which the Tasmota firmware is running.

**TO-DO LIST**

- Complete register integration to enable updating of current value from Sonoff-Tasmota via I2C
- Implement PWM support on at least one PWM capable output pin
- Implement zero cross detection so that PWM can also be used for AC dimming.

