/*
  Read x88c64
  port A - low address bus and data bus
  port C.0 - C.4 - high address bus
  port L.0 - ~PSEN
  port L.1 - ~ALE
  port L.2 - ~RD
  port L.3 - ~CE
*/

#include <stdio.h>

#define WRITE_AD(value) WriteAll(22,8,1,value)
#define WRITE_A(value) WriteAll(37,5,-1,value)
#define WRITE_CTL(value) WriteAll(49,4,-1,value)
#define READ_D(value) ReadAll(22,8,1)

void DDIR_AD(int out) 
{
  if (out) {
    pinMode(22, OUTPUT);  // AD0
    pinMode(23, OUTPUT);  // AD1
    pinMode(24, OUTPUT);  // AD2
    pinMode(25, OUTPUT);  // AD3
    pinMode(26, OUTPUT);  // AD4
    pinMode(27, OUTPUT);  // AD5
    pinMode(28, OUTPUT);  // AD6
    pinMode(29, OUTPUT);  // AD7
  } else {
    pinMode(22, INPUT);  // AD0
    pinMode(23, INPUT);  // AD1
    pinMode(24, INPUT);  // AD2
    pinMode(25, INPUT);  // AD3
    pinMode(26, INPUT);  // AD4
    pinMode(27, INPUT);  // AD5
    pinMode(28, INPUT);  // AD6
    pinMode(29, INPUT);  // AD7    
  }
}

void WriteAll(int pinstart, int numpins, int incdec, int value)
{
  int pin = pinstart;
  int i=0;
  for(i=0; i<numpins; i++) {
    digitalWrite(pin, value & 0x01);
    value >>= 1;
    pin += incdec;
  }
}

int ReadAll(int pinstart, int numpins, int incdec)
{
  int pin = pinstart;
  int i=0;
  int val = 0;
  for(i=0; i<numpins; i++) {
    val |= (digitalRead(pin) & 0x01) << i;
    pin += incdec;
  }
  return val;
}

char tmp[128];
    
void setup() {
  Serial.begin(115200);
    
  DDIR_AD(1);

  pinMode(37, OUTPUT);  // A8
  pinMode(36, OUTPUT);  // A9
  pinMode(35, OUTPUT);  // A10
  pinMode(34, OUTPUT);  // A11
  pinMode(33, OUTPUT);  // A12

  pinMode(49, OUTPUT);  // ~PSEN
  pinMode(48, OUTPUT);  // ~ALE
  pinMode(47, OUTPUT);  // ~RD
  pinMode(46, OUTPUT);  // ~CE
  
  noInterrupts();
}

int n = 1;
void loop() {
  digitalWrite(13, 1);
  delayMicroseconds(100);
  digitalWrite(13, 0);
  //WRITE_AD(0);
  //WRITE_A(0);
  
  //WRITE_CTL(0x7);  // CE
  //delay(n);
  //for(int addr=0; addr<0x100; addr++)
  for(unsigned long int addr=0; addr<0x2000; addr++)
  {
    // psen and ale high
    WRITE_CTL(0x7);  // CE
    delayMicroseconds(100);
        
    // Put address on bus
    WRITE_AD(addr & 0xff);
    WRITE_A((addr & 0x1f00) >> 8);
    DDIR_AD(1);
    delayMicroseconds(200);

    // clock it out
    WRITE_CTL(0x5); // ALE low
    delayMicroseconds(50);

    // remove from bus
    DDIR_AD(0);
    delayMicroseconds(50);

    // signal all clear to drive bus
    //WRITE_CTL(0x4); // PSEN low
    WRITE_CTL(0x1); // RD low
    delayMicroseconds(50);
    
    // read data
    int val = READ_D();
    delayMicroseconds(100);

    if ((addr % 16) == 0) {
      sprintf(tmp, "\n%04x:", addr);
      Serial.print(tmp);
    }
    sprintf(tmp, " %02x", val);
    Serial.print(tmp);

  }
  Serial.print('\n');
  //WRITE_CTL(0xf);  // CE
}
