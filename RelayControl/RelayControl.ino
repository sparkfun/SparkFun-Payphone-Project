/*
  Nathan Seidle
  SparkFun Electronics
  October 9th, 2021

*/

//Hardware connections
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//These pins are set in beginBoard()
int pin_hook = 3;
int pin_keypadDTMF = A0;
int pin_relayControl = 4;
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=


//Global Variables
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
volatile bool coinDetected = false; //Goes true in ISR
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun Payphone Coin Collector");

  pinMode(pin_hook, INPUT_PULLUP);
  pinMode(pin_keypadDTMF, INPUT);
  pinMode(pin_relayControl, OUTPUT);
 
}

void loop()
{
  if(Serial.available())
  {
    byte incoming = Serial.read();

    if(incoming == 'a')
    {
      digitalWrite(pin_relayControl, HIGH);
      Serial.println("On!");
    }
    else if(incoming == 'z')
    {
      digitalWrite(pin_relayControl, LOW);
      Serial.println("Off!");
    }
    
  }
}
