void setup()
{
  Serial.begin(115200);
  Serial.println("Payphone coin test");
}

void loop()
{
  int coinRead = analogRead(A0);
  if(coinRead < 1000)
  Serial.println("Coin!");
  //Serial.print("coin: ");
  //Serial.println(coinRead);

  //delay(10);
}
