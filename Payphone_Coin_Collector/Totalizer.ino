void beginTotalizer(void)
{
  attachPCINT(digitalPinToPCINT(pin_Quarter), quarter, FALLING);
  attachPCINT(digitalPinToPCINT(pin_Nickel), nickel, FALLING);
  attachPCINT(digitalPinToPCINT(pin_Dime), dime, FALLING);

  Serial.println(F("Totalizer setup"));
}

void quarter(void) {
  coinDetected = true;
}
void nickel(void) {
  coinDetected = true;
}
void dime(void) {
  coinDetected = true;
}
