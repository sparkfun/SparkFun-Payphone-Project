//

const int pin_dime = 8;
const int pin_nickel = 9;
const int pin_quarter = 10;


void setup()
{
  Serial.begin(115200);
  Serial.println("Payphone coin test");

  beginTotalizer();
}

void loop()
{

}

void coinDetected(void) {
  Serial.println("!");
}
