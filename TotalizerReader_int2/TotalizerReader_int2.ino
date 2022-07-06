
const int pin_dime = 8;
const int pin_nickel = 9;
const int pin_quarter = 10;

#define PCINT_PIN_Q 8 //Quarter

#define PCINT_PIN_N 9 //Nickel

#define PCINT_PIN_D 10 //Dime


volatile uint8_t oldPort = 0x00;


void setup()
{
  Serial.begin(115200);
  Serial.println("Payphone coin test");

  attachInterrupt(digitalPinToInterrupt(9), coinDetected, FALLING);
}

void loop()
{

}

void coinDetected(void) {
  // switch Led state
  digitalWrite(LED_BUILTIN, !digitalRead(LED_BUILTIN));
  Serial.println("!");
}
