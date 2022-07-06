/*
  Nathan Seidle
  SparkFun Electronics
  October 9th, 2021

  State:
    Upon open hook, connect phone to VOIP
    Monitor keypad.
      If '911' or '1888' or '1800' pressed, allow connection.
      If not, disconnect VOIP.
      If 3038240979 (SparkFun/Rick) or 8675309 (Jenny) play egg.
    Monitor coin drop. If coin dropped, allow all connections.

  Based on:
    https://create.arduino.cc/projecthub/MM_Shoaib/dtmf-decoder-using-only-arduino-872502
    https://github.com/jacobrosenthal/Goertzel
    http://www.embedded.com/design/embedded/4024443/The-Goertzel-Algorithm

  Press any key to open configuration menu (MP3 volume, DTMF settings, etc).

  Print number dialed
*/

#include <EEPROM.h>
#include "settings.h" //All system, button, and dial states

#include "Wire.h"

//Hardware connections
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
//These pins are set in beginBoard()
const int pin_keypadDTMF = A0;
const int pin_hook = 3;
const int pin_VoipRelayControl = 4;
const int pin_HandsetRelayControl = 7;
const int pin_Quarter = 8;
const int pin_Nickel = 9;
const int pin_Dime = 10;
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//MP3 Control
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "SparkFun_Qwiic_MP3_Trigger_Arduino_Library.h" //http://librarymanager/All#SparkFun_MP3_Trigger
MP3TRIGGER mp3;
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Pin Change Interrupts on all pins
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "PinChangeInterrupt.h" //By Nico Hood: http://librarymanager/All#PinChangeInterrupt
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Global Variables
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
volatile bool coinDetected = false; //Goes true in ISR
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

void setup()
{
  Serial.begin(115200);
  Serial.println();
  Serial.println(F("SparkFun Payphone Coin Collector"));

  pinMode(pin_hook, INPUT_PULLUP);
  pinMode(pin_keypadDTMF, INPUT);
  pinMode(pin_VoipRelayControl, OUTPUT);
  pinMode(pin_HandsetRelayControl, OUTPUT);

  pinMode(pin_Quarter, INPUT_PULLUP);
  pinMode(pin_Nickel, INPUT_PULLUP);
  pinMode(pin_Dime, INPUT_PULLUP);

  loadSettings();

  disconnectVoip();
  connectHandset();

  beginTotalizer(); //Start coin interrupts

  beginMP3(); //Start comm with MP3s

  Serial.print(F("Tone Debugging: "));
  if (settings.enableToneDebug == true) Serial.println(F("true"));
  else Serial.println(F("false"));

  Serial.print(F("DTMF threshold: "));
  Serial.println(settings.threshold);

  Serial.print(F("DTMF samples: "));
  Serial.println(settings.N);

  Serial.print(F("MP3 Volume: "));
  Serial.println(settings.mp3Volume);

  Serial.println(F("Phone setup complete"));
}

void loop()
{
  if (Serial.available()) mainMenu();

  updateSystemState();
}

void mainMenu()
{
  while (1)
  {
    Serial.println();

    Serial.print(F("Interactions: "));
    Serial.println(settings.interactions);

    Serial.print(F("Free calls: "));
    Serial.println(settings.freeCalls);

    Serial.print(F("Paid Calls: "));
    Serial.println(settings.paidCalls);

    Serial.print(F("I need a dollar played: "));
    Serial.println(settings.dollarCalled);

    Serial.print(F("SparkFun called: "));
    Serial.println(settings.rickCalled);

    Serial.print(F("Jenny called: "));
    Serial.println(settings.jennyCalled);

    Serial.print(F("911 called: "));
    Serial.println(settings.nine11Called);

    Serial.println(F("m) Play MP3"));

    Serial.print(F("1) Toggle VOIP relay: "));
    if (digitalRead(pin_VoipRelayControl) == HIGH) Serial.println(F("HIGH"));
    else Serial.println(F("LOW"));

    Serial.print(F("2) Toggle Handset relay: "));
    if (digitalRead(pin_HandsetRelayControl) == HIGH) Serial.println(F("HIGH"));
    else Serial.println(F("LOW"));

    Serial.print(F("d) Tone Debugging: "));
    if (settings.enableToneDebug == true) Serial.println(F("true"));
    else Serial.println(F("false"));

    Serial.print(F("f) Set DTMF threshold: "));
    Serial.println(settings.threshold);

    Serial.print(F("n) Set DTMF samples: "));
    Serial.println(settings.N);

    Serial.print(F("v) Set MP3 Volume: "));
    Serial.println(settings.mp3Volume);

    Serial.println(F("k) Keypad Testing"));

    Serial.println(F("e) Erase EEPROM"));

    Serial.println(F("x) Exit"));

    byte incoming = getByteChoice();

    if (incoming == 'm')
    {
      playDollar();
    }
    else if (incoming == 'd')
    {
      Serial.println(F("Toggle debug"));

      if (settings.enableToneDebug == true) settings.enableToneDebug = false;
      else settings.enableToneDebug = true;

      recordSystemSettings();
    }
    else if (incoming == 'f')
    {
      Serial.println(F("Enter new DTMF threshold: "));
      float newThreshold = getNumber();

      if (newThreshold > 500 && newThreshold < 10000)
      {
        settings.threshold = newThreshold;
        recordSystemSettings();
      }
      else
        Serial.println(F("Out of range"));
    }
    else if (incoming == 'n')
    {
      Serial.println(F("Enter new DTMF Sample Amount: "));
      float newAmount = getNumber();

      if (newAmount > 10 && newAmount < 200)
      {
        settings.N = newAmount;
        recordSystemSettings();
      }
      else
        Serial.println(F("Out of range"));
    }
    else if (incoming == 'v')
    {
      Serial.println(F("Enter new MP3 Volume: "));
      float newVolume = getNumber();

      if (newVolume > 0 && newVolume < 32)
      {
        settings.mp3Volume = newVolume;
        recordSystemSettings();
        mp3.setVolume(settings.mp3Volume); //Volume can be 0 (off) to 31 (max)
      }
      else
        Serial.println(F("Out of range"));
    }
    else if (incoming == '1')
    {
      if (digitalRead(pin_VoipRelayControl) == HIGH)
      {
        Serial.println(F("Turning VOIP off"));
        digitalWrite(pin_VoipRelayControl, LOW);
      }
      else if (digitalRead(pin_VoipRelayControl) == LOW)
      {
        Serial.println(F("Turning VOIP on"));
        digitalWrite(pin_VoipRelayControl, HIGH);
      }
    }
    else if (incoming == '2')
    {
      if (digitalRead(pin_HandsetRelayControl) == HIGH)
      {
        Serial.println(F("Turning Handset off"));
        digitalWrite(pin_HandsetRelayControl, LOW);
      }
      else if (digitalRead(pin_HandsetRelayControl) == LOW)
      {
        Serial.println(F("Turning Handset on"));
        digitalWrite(pin_HandsetRelayControl, HIGH);
      }
    }
    else if (incoming == 'e')
    {
      for (int x = 0 ; x < 100 ; x++)
        EEPROM.write(x, 0xFF);
    }
    else if (incoming == 'k')
    {
      keypadTesting();
    }
    else if (incoming == 'x')
    {
      Serial.println(F("Later gator"));
      break;
    }
  }

  delay(250);
  while (Serial.available()) Serial.read(); //Empty buffer of any newline chars
}

void keypadTesting()
{

  if (phoneOnHook() == true)
  {
    Serial.println(F("Pickup the phone"));
    while (phoneOnHook() == true)
      delay(1);
  }

  if (phoneOnHook() == false)
  {
    if (mp3.isPlaying()) mp3.stop();

    coinDetected = false;
    buttonState = BUTTON_NO_BUTTON;
    dialState = DIAL_TONE;
    changeState(STATE_OFF_HOOK);
    connectVoip(); //Connect system to VOIP
    connectHandset(); //Connect handset to VOIP system
  }

  while (1)
  {
    Serial.println();

    Serial.print(F("f) Set DTMF threshold: "));
    Serial.println(settings.threshold);

    Serial.print(F("n) Set DTMF samples: "));
    Serial.println(settings.N);

    Serial.println(F("x) Exit"));

    Serial.flush();
    delay(50);//Wait for any incoming chars to hit buffer
    while (Serial.available() > 0) Serial.read(); //Clear buffer

    byte incoming = 0;
    while (1)
    {
      checkKeypad(true); //Attempt to identify any new DTMF frequencies from keypad. In test mode (no state changes)

      if (Serial.available() > 0)
      {
        incoming = Serial.read();
        if (incoming >= 'a' && incoming <= 'z') break;
        if (incoming >= 'A' && incoming <= 'Z') break;
        if (incoming >= '0' && incoming <= '9') break;
      }
    }

    if (incoming == 'f')
    {
      Serial.println(F("Enter new DTMF threshold: "));
      float newThreshold = getNumber();

      if (newThreshold > 500 && newThreshold < 10000)
      {
        settings.threshold = newThreshold;
        recordSystemSettings();
      }
      else
        Serial.println(F("Out of range"));
    }
    else if (incoming == 'n')
    {
      Serial.println(F("Enter new DTMF Sample Amount: "));
      float newAmount = getNumber();

      if (newAmount > 10 && newAmount < 200)
      {
        settings.N = newAmount;
        recordSystemSettings();
      }
      else
        Serial.println(F("Out of range"));
    }

    else if (incoming == 'x')
    {
      break;
    }
  }

  delay(250);
  while (Serial.available()) Serial.read(); //Empty buffer of any newline chars
}
