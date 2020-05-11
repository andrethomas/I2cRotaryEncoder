#define XSNS_92             92


#ifdef USE_I2C
#ifdef USE_I2C_ROTARY

#define I2C_ROTARY_ADDRESS            0x77

#define I2C_ROTARY_SIGNATURE          0x67
#define I2C_ROTARY_VALUE              0x65

#define I2C_ROTARY_READ_INTERVAL         1 // Read the rotary encoder every this many 100milliseconds and report via MQT the current value IF the Value changed

uint8_t i2c_rotary_type = 0;
uint8_t i2c_rotary_read_counter = 0;
uint8_t oldvalue = 0;

void I2cRotary_Detect(void) {
  if (i2c_rotary_type) { return; }

  uint8_t buffer = I2cRead8(I2C_ROTARY_ADDRESS,I2C_ROTARY_SIGNATURE);
  if (0xFE == buffer) {
    i2c_rotary_type = 1;
    snprintf_P(log_data, sizeof(log_data), S_LOG_I2C_FOUND_AT, "I2cRotaryEncoder", I2C_ROTARY_ADDRESS);
    AddLog(LOG_LEVEL_DEBUG);
  }
}

uint8_t I2cRotary_Read(void) {
  uint8_t value = I2cRead8(I2C_ROTARY_ADDRESS,I2C_ROTARY_VALUE);
  return value;
}


void I2cRotary_Telemetry(void)
{
  if (i2c_rotary_type) {
    uint8_t value = I2cRotary_Read();
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s,{\"I2cRotary\":%i}"), mqtt_data, value);
  }
}

void I2cRotary_Report_Value(void) {
uint8_t newvalue = I2cRotary_Read();
  if (oldvalue != newvalue){
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("{\"" D_JSON_TIME "\":\"%s\""), GetDateAndTime(DT_LOCAL).c_str());
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s,\"I2cRotary_Value\":%i}"), mqtt_data, newvalue);
    snprintf_P(mqtt_data, sizeof(mqtt_data), PSTR("%s}"), mqtt_data);
    MqttPublishPrefixTopic_P(RESULT_OR_STAT, mqtt_data);
    oldvalue = newvalue;
  }
}

/*********************************************************************************************\
 * Interface
\*********************************************************************************************/

//#define XSNS_92

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
    }
  }
  return result;
}

#endif  // USE_I2C_ROTARY
#endif  // USE_I2C
