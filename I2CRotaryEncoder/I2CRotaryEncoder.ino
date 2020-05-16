/*
  I2CRotaryEncoderino - I2C Rotary Encoder for Tasmota
  
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

#include <Wire.h>

#define LIMIT_ENABLE            // Keep value between LIMIT_MIN and LIMIT_MAX
#define LIMIT_MIN 0
#define LIMIT_MAX 100

#define CLK  2
#define DATA 3

#define I2C_ADDRESS             0x77
#define I2C_ID                  0xFE

#define I2C_ROTARY_SIGNATURE    0x67
#define I2C_ROTARY_SETROTARY    0x66
#define I2C_ROTARY_VALUE        0x65

#define DEBUG                         // Serial monitor debug output

uint8_t encoder_value = 0;
static uint8_t prevNextCode = 0;
static uint16_t store=0;

void setup(void)
{
  pinMode(CLK, INPUT);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DATA, INPUT);
  pinMode(DATA, INPUT_PULLUP);
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
#ifdef DEBUG
  Serial.begin (115200);
  Serial.println("I2C Rotaryencoder for Tasmota");
#endif // DEBUG  
}

void loop(void)
{
  static int8_t c,val;
  if (val = read_rotary()) {
    c+=val;
#ifdef DEBUG
    Serial.print(c);
    Serial.print(" ");
    Serial.print(encoder_value);
    Serial.print(" ");
#endif // DEBUG
    if (prevNextCode == 0x0b) {
#ifdef LIMIT_ENABLE
      if (encoder_value > (uint8_t)LIMIT_MIN) {
        encoder_value--;
      }
#else      
      encoder_value--;
#endif // LIMIT_ENABLE
#ifdef DEBUG
      Serial.print("left turn ");
      Serial.println(store,HEX);
#endif // DEBUG
    }
    if (prevNextCode == 0x07) {
#ifdef LIMIT_ENABLE
      if (encoder_value < (uint8_t)LIMIT_MAX) {
        encoder_value++;
      }
#else      
      encoder_value++;
#endif // LIMIT_ENABLE
#ifdef DEBUG
      Serial.print("right turn ");
      Serial.println(store,HEX);
#endif // DEBUG
    }
  }
}

int8_t read_rotary(void)
{
  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

  prevNextCode <<= 2;
  if (digitalRead(DATA)) {
    prevNextCode |= 0x02;
  }
  if (digitalRead(CLK)) {
    prevNextCode |= 0x01;
  }
  prevNextCode &= 0x0f;

  if  (rot_enc_table[prevNextCode]) {
    store <<= 4;
    store |= prevNextCode;
    if ((store&0xff) == 0x2b) {
      return -1;
    }
    if ((store&0xff) == 0x17) {
      return 1;
    }
 }
 return 0;
}

void receiveEvent(int bytesReceived)
{
}

void SendValue(void)
{
  Wire.write(encoder_value);
}

void SendSignature(void)
{
  Wire.write(I2C_ID);
}

void SetValue(void)
{
  uint8_t tmp = Wire.read();
#ifdef LIMIT_ENABLE
  if (tmp < (uint8_t)LIMIT_MIN) {
    tmp = (uint8_t)LIMIT_MIN;
  }
  if (tmp > (uint8_t)LIMIT_MAX) {
    tmp = (uint8_t)LIMIT_MAX;
  }
#endif  
  encoder_value = tmp;
}

void requestEvent(void)
{
  uint8_t reg = Wire.read();
  switch (reg) {
    case I2C_ROTARY_SIGNATURE:
         SendSignature();
         break;
    case I2C_ROTARY_VALUE:
         SendValue();
         break;
    case I2C_ROTARY_SETROTARY:
         SetValue();
         break;
    default:
         break;
  }
}
