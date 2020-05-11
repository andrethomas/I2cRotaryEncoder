#define CLK 2
#define DATA 3

#include <Wire.h>

#define I2C_ADDRESS           0x77
#define I2C_ID                0xFE

#define I2C_ROTARY_SIGNATURE    0x67
#define I2C_ROTARY_VALUE        0x65

#define DEBUG 1         //level 1 für Seriellen Monitor, 0 für aus

uint8_t encoder_value = 0;

void setup() {
  pinMode(CLK, INPUT);
  pinMode(CLK, INPUT_PULLUP);
  pinMode(DATA, INPUT);
  pinMode(DATA, INPUT_PULLUP);
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
      if (DEBUG >= 1){
        Serial.begin (115200);
        Serial.println("I2C Rotaryencoder for Tasmota");
      }
}

static uint8_t prevNextCode = 0;
static uint16_t store=0;

void loop() {
static int8_t c,val;

   if( val=read_rotary() ) {
      c +=val;
      if (DEBUG >= 1){
        Serial.print(c);Serial.print(" ");
        Serial.print(encoder_value);Serial.print(" ");
      }
      if ( prevNextCode==0x0b) {
        encoder_value --;
        if (DEBUG >= 1){
         Serial.print("eleven ");
         Serial.println(store,HEX);
        }
      }

      if ( prevNextCode==0x07) {
        encoder_value ++;
        if (DEBUG >= 1){
         Serial.print("seven ");
         
         Serial.println(store,HEX);
        }
      }
   }
}

int8_t read_rotary() {
  static int8_t rot_enc_table[] = {0,1,1,0,1,0,0,1,1,0,0,1,0,1,1,0};

  prevNextCode <<= 2;
  if (digitalRead(DATA)) prevNextCode |= 0x02;
  if (digitalRead(CLK)) prevNextCode |= 0x01;
  prevNextCode &= 0x0f;

   if  (rot_enc_table[prevNextCode] ) {
      store <<= 4;
      store |= prevNextCode;
      if ((store&0xff)==0x2b) return -1;
      if ((store&0xff)==0x17) return 1;
   }
   return 0;
}

void receiveEvent(int bytesReceived) {
}

void SendValue(void) {
  Wire.write(encoder_value);
}

void SendSignature(void) {
  Wire.write(I2C_ID);
}

void requestEvent() {
  uint8_t reg = Wire.read();
    switch (reg) {
      case I2C_ROTARY_SIGNATURE:
        SendSignature();
        break;
      case I2C_ROTARY_VALUE:
        SendValue();
        break;
    }
}
