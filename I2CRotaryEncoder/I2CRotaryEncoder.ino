#include <Wire.h>

#define I2C_ADDRESS           0x77
#define I2C_ID                0xFE

#define I2C_ROTARY_SIGNATURE    0x67
#define I2C_ROTARY_VALUE        0x65

const byte encoderPinA = 2;
const byte encoderPinB = 3;

volatile int count = 0;

uint8_t encoder_value = 0;
uint8_t encoder_value_prev = 0;

#define readA bitRead(PIND,2) // Faster to read this way than digitalRead();
#define readB bitRead(PIND,3) // Faster to read this way than digitalRead();

void setup() {
  pinMode(encoderPinA, INPUT_PULLUP);
  pinMode(encoderPinB, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(encoderPinA), isrA, FALLING);
  attachInterrupt(digitalPinToInterrupt(encoderPinB), isrB, FALLING);
  Wire.begin(I2C_ADDRESS);
  Wire.onRequest(requestEvent);
  Wire.onReceive(receiveEvent);
  
} 

void loop()  {
  noInterrupts();
  encoder_value = count;
  interrupts();
  encoder_value_prev = encoder_value;
}

void isrA() {
  if(readB != readA) {
    count ++;
  } else {
    count --;
  }
  if (count < 0) count=0;
  if (count > 100) count=100;
}

void isrB() {
  if (readA == readB) {
    count ++;
  } else {
    count --;
  }
  if (count < 0) count=0;
  if (count > 100) count=100;
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

