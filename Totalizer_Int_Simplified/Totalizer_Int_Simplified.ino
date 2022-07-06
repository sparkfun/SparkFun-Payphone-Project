//

#include "PinChangeInterrupt.h"

const int pin_Dime = 8;
const int pin_Nickel = 9;
const int pin_Quarter = 10;

void setup()
{
  Serial.begin(115200);
  Serial.println("Payphone coin test");

  pinMode(pin_Quarter, INPUT_PULLUP);
  pinMode(pin_Nickel, INPUT_PULLUP);
  pinMode(pin_Dime, INPUT_PULLUP);

  attachPCINT(digitalPinToPCINT(pin_Quarter), quarter, FALLING);
  attachPCINT(digitalPinToPCINT(pin_Nickel), nickel, FALLING);
  attachPCINT(digitalPinToPCINT(pin_Dime), dime, FALLING);
}

void loop()
{
}

void quarter(void) {
  Serial.println("Q");
}
void nickel(void) {
  Serial.println("N");
}
void dime(void) {
  Serial.println("D");
}
