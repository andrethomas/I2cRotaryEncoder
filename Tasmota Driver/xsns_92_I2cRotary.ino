/*
  xsns_92_I2cRotary.ino - I2C Rotary Encoder for Tasmota
  
  Copyright (C) 2020  Andre Thomas and @feedbagg
  
  This program is free software: you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation, either version 3 of the License, or
  (at your option) any later version.
  
  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.
  
  You should have received a copy of the GNU General Public License
  along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#ifdef USE_I2C
#ifdef USE_I2C_ROTARY

#define XSNS_92                         92

#define I2C_ROTARY_ADDRESS            0x77

#define I2C_ROTARY_SIGNATURE          0x67
#define I2C_ROTARY_VALUE              0x65

#define I2C_ROTARY_READ_INTERVAL         1 // Read the rotary encoder every this many 100milliseconds and report via MQT the current value IF the Value changed

uint8_t i2c_rotary_type = 0;
uint8_t i2c_rotary_read_counter = 0;
uint8_t i2c_rotary_oldvalue = 0;

void I2cRotary_Detect(void)
{
  if (i2c_rotary_type) { return; }

  uint8_t buffer = I2cRead8(I2C_ROTARY_ADDRESS, I2C_ROTARY_SIGNATURE);
  if (0xFE == buffer) {
    i2c_rotary_type = 1;
    snprintf_P(log_data, sizeof(log_data), S_LOG_I2C_FOUND_AT, "I2cRotaryEncoder", I2C_ROTARY_ADDRESS);
    AddLog(LOG_LEVEL_DEBUG);
  }
}

uint8_t I2cRotary_Read(void)
{
  return I2cRead8(I2C_ROTARY_ADDRESS, I2C_ROTARY_VALUE);
}

void I2cRotary_Telemetry(void)
{
  if (i2c_rotary_type) {
    uint8_t value = I2cRotary_Read();
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s,\"I2cRotary\":%i"), mqtt_data, value);
  }
}

void I2cRotary_Report_Value(void)
{
  uint8_t newvalue = I2cRotary_Read();
  if (i2c_rotary_oldvalue != newvalue) {
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("{\"" D_JSON_TIME "\":\"%s\""), GetDateAndTime(DT_LOCAL).c_str());
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s,{\"I2cRotary_Value\":%i}"), mqtt_data, newvalue);
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s}"), mqtt_data);
    MqttPublishPrefixTopic_P(RESULT_OR_STAT, mqtt_data);
    i2c_rotary_oldvalue = newvalue;
  }
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

boolean Xsns92(byte function)
{
  boolean result = false;

  if (i2c_flg) {
    switch (function) {
      case FUNC_EVERY_100_MSECOND:
        I2cRotary_Detect();
        if (i2c_rotary_type) { // We have a valid rotary encoder on I2C
          i2c_rotary_read_counter++; // Increase the counter
          if (i2c_rotary_read_counter >= I2C_ROTARY_READ_INTERVAL) { // We have reached our reporting time as defined by I2C_ROTARY_READ_INTERVAL
            I2cRotary_Report_Value();
            i2c_rotary_read_counter = 0; // Reset the counter variable
          }
        }
        break;
      case FUNC_JSON_APPEND:
        I2cRotary_Telemetry();
        break;
      default:
        break;
    }
  }
  return result;
}

#endif  // USE_I2C_ROTARY
#endif // USE_I2C
