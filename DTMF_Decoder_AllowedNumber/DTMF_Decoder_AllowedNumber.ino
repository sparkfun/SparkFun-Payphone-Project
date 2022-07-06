/*
  Nathan Seidle
  SparkFun Electronics
  October 9th, 2021

  This example decodes DTFM tones and prints the number pressed. It uses a state machine to
  debounce and return a single button press.

  Based on:
    https://create.arduino.cc/projecthub/MM_Shoaib/dtmf-decoder-using-only-arduino-872502
    https://github.com/jacobrosenthal/Goertzel
    http://www.embedded.com/design/embedded/4024443/The-Goertzel-Algorithm

*/

#include "Goertzel.h" //From: https://github.com/jacobrosenthal/Goertzel

int sensorPin = A0;
const int N = 100;                       //Number of samples code will take. You can change for sensitivity. If too large it can slow the arduino.
const float threshold = 4000;            //Minimum tone amplitude to be considered. Found using debug prints in detectTone()
const float samplingFreq = 8900;         //Maximum detectable frequency is the sampling rate/2. Arduino Uno with 16MHz can support sampling upto 8900 Hz

float xFrequencies[4] = {1209, 1336, 1477, 1633};                   //Known frequencies for a 4x4 keypad. Don't worry if you're using the more common 3x4 keypad.
float yFrequencies[4] = {697, 770, 852, 941};

byte xFreq = 0; //Found freqs when button is pressed
byte yFreq = 0;

//System can enter a variety of states starting at 'No Button' at power on
typedef enum
{
  STATE_NO_BUTTON = 0,
  STATE_X_FOUND,
  STATE_XY_FOUND,
  STATE_BUTTON_HELD,
} SystemState;
SystemState systemState = STATE_NO_BUTTON;

//Detect if a number is allowed without coin deposit
typedef enum
{
  DIAL_TONE = 0,
  DIAL_9, //911
  DIAL_91,
  DIAL_1, //1-800 and 1-888
  DIAL_18,
  DIAL_180,
  DIAL_188,
  DIAL_8, //Jenny
  DIAL_86,
  DIAL_867,
  DIAL_8675,
  DIAL_86753,
  DIAL_867530,
  DIAL_3, //SparkFun
  DIAL_30,
  DIAL_303,
  DIAL_3037,
  DIAL_30373,
  DIAL_303734,
  DIAL_3037348,
  DIAL_30373485,
  DIAL_303734856,
  DIAL_BAD_NUMBER,
  DIAL_CONNECT,
  DIAL_JENNY,
  DIAL_RICKROLL,
} DialState;
DialState dialState = DIAL_TONE;
byte numberPressed = 0;

void setup()
{
  Serial.begin(115200);
  Serial.println("SparkFun DTFM Decoder");
}

void loop()
{
  if (systemState == STATE_NO_BUTTON)
  {
    //Step through the known xFreq and see if the current signal
    //on A0 is above the threshold for that freq.
    for (byte i = 0 ; i < 4 ; i++)
    {
      if (detectTone(xFrequencies[i]) == true)
      {
        xFreq = i;
        systemState = STATE_X_FOUND;
        break;
      }
    }
  }
  else if (systemState == STATE_X_FOUND)
  {
    //Look for yFreq
    for (byte i = 0 ; i < 4 ; i++)
    {
      if (detectTone(yFrequencies[i]) == true)
      {
        yFreq = i;
        systemState = STATE_XY_FOUND;
        break;
      }
    }

    //If yFreq not found, return to searching state
    if (systemState == STATE_X_FOUND)
    {
      systemState = STATE_NO_BUTTON;
    }
  }
  else if (systemState == STATE_XY_FOUND)
  {
    numberPressed = resolveNumber(yFreq, xFreq);

    updateNumberState(numberPressed); //Keep track of numbers that are allowed without coin payment. Disconnect as needed.

    systemState = STATE_BUTTON_HELD;
  }
  else if (systemState == STATE_BUTTON_HELD)
  {
    //Look for missing freq to indicate button release
    if (detectTone(xFrequencies[xFreq]) != true || detectTone(yFrequencies[yFreq]) != true)
    {
      systemState = STATE_NO_BUTTON;
    }
  }
}

void updateNumberState(int numberPressed)
{
  //Based on number pressed, allow call without coin, or disconnect if bad number is entered
  if (dialState == DIAL_TONE)
  {
    if (numberPressed == 9)
    {
      dialState = DIAL_9;
      Serial.println("DT9");
    }
    else if (numberPressed == 1)
    {
      dialState = DIAL_1;
      Serial.println("DT1");
    }
    else if (numberPressed == 8)
    {
      dialState = DIAL_8;
      Serial.println("DT8");
    }
    else if (numberPressed == 3)
    {
      dialState = DIAL_3;
    }
    else
      badNumber();
  }

  //Start 911
  else if (dialState == DIAL_9)
  {
    if (numberPressed == 1)
    {
      dialState = DIAL_91;
      Serial.println("DT91");
    }
    else
      badNumber();
  }
  else if (dialState == DIAL_91)
  {
    if (numberPressed == 1)
      validNoCoinCall();
    else
      badNumber();
  }

  //Start 1-800 and 1-888
  else if (dialState == DIAL_1)
  {
    if (numberPressed == 8)
      dialState = DIAL_18;
    else
      badNumber();
  }
  else if (dialState == DIAL_18)
  {
    if (numberPressed == 0)
      dialState = DIAL_180;
    else if (numberPressed == 8)
      dialState = DIAL_188;
    else
      badNumber();
  }
  else if (dialState == DIAL_180)
  {
    if (numberPressed == 0)
      validNoCoinCall();
    else
      badNumber();
  }
  else if (dialState == DIAL_188)
  {
    if (numberPressed == 8)
      validNoCoinCall();
    else
      badNumber();
  }

  //Start Jenny
  else if (dialState == DIAL_8)
  {
    if (numberPressed == 6)
      dialState = DIAL_86;
    else
      badNumber();
  }
  else if (dialState == DIAL_86)
  {
    if (numberPressed == 7)
      dialState = DIAL_867;
    else
      badNumber();
  }
  else if (dialState == DIAL_867)
  {
    if (numberPressed == 5)
      dialState = DIAL_8675;
    else
      badNumber();
  }
  else if (dialState == DIAL_8675)
  {
    if (numberPressed == 3)
      dialState = DIAL_86753;
    else
      badNumber();
  }
  else if (dialState == DIAL_86753)
  {
    if (numberPressed == 0)
      dialState = DIAL_867530;
    else
      badNumber();
  }
  else if (dialState == DIAL_867530)
  {
    if (numberPressed == 9)
      dialState = DIAL_JENNY;
    else
      badNumber();
  }

  //Start SparkFun
  else if (dialState == DIAL_3)
  {
    if (numberPressed == 0)
      dialState = DIAL_30;
    else
      badNumber();
  }
  else if (dialState == DIAL_30)
  {
    if (numberPressed == 3)
      dialState = DIAL_303;
    else
      badNumber();
  }
  else if (dialState == DIAL_303)
  {
    if (numberPressed == 7)
      dialState = DIAL_3037;
    else
      badNumber();
  }
  else if (dialState == DIAL_3037)
  {
    if (numberPressed == 3)
      dialState = DIAL_30373;
    else
      badNumber();
  }
  else if (dialState == DIAL_30373)
  {
    if (numberPressed == 4)
      dialState = DIAL_303734;
    else
      badNumber();
  }
  else if (dialState == DIAL_303734)
  {
    if (numberPressed == 8)
      dialState = DIAL_3037348;
    else
      badNumber();
  }
  else if (dialState == DIAL_3037348)
  {
    if (numberPressed == 5)
      dialState = DIAL_30373485;
    else
      badNumber();
  }
  else if (dialState == DIAL_30373485)
  {
    if (numberPressed == 6)
      dialState = DIAL_303734856;
    else
      badNumber();
  }
  else if (dialState == DIAL_303734856)
  {
    if (numberPressed == 8)
    {
      Serial.println("Dial SparkFun");
    }
    else
      badNumber();
  }

}

//If a valid number is pressed, connect call
void validNoCoinCall()
{
  dialState = DIAL_CONNECT;
  Serial.println("No coin call placed. Connecting...");
}

//Runs when a bad number is pressed
void badNumber()
{
  dialState = DIAL_BAD_NUMBER;
  Serial.println("Bad Number");
  disconnectCall();
}

//Disconnect handset from VOIP
void disconnectCall()
{
  Serial.println("Disconnect call");
  //digitalWrite()
}

//Given a freq, return true if magnitude of freq is above threshold
bool detectTone(float targetFreq)
{
  Goertzel goertzel = Goertzel(targetFreq, N, samplingFreq); //Initialize library function with the given sampling frequency, no of samples, and target freq
  goertzel.sample(sensorPin);                               //Will take n samples
  float magnitude = goertzel.detect();                      //Get the magnitude of give freq within signal

  //  Serial.print("F: ");
  //  Serial.print(targetFreq);
  //  Serial.print("\t");
  //
  //  Serial.print("M: ");
  //  Serial.print(magnitude, 2);
  //  Serial.print("\t");
  //
  //  Serial.println();

  if (magnitude > threshold) {                               //if you're getting false hits or no hits adjust the threshold
    //    Serial.print(targetFreq);
    //    Serial.print("\n");
    return true;
  }
  else
  {
    //    Serial.print("Freq below thresh: ");
    //    Serial.println(freq);
    return false;
  }
}

//Given row and column, print the keypad button being pressed
byte resolveNumber(int row, int column)
{
  int number = 0;
  if (row == 0)
  {
    if (column == 0)
      number = 1;
    else if (column == 1)
      number = 2;
    else if (column == 2)
      number = 3;
    else if (column == 3)
      number = 10;
  }
  else if (row == 1)
  {
    if (column == 0)
      number = 4;
    else if (column == 1)
      number = 5;
    else if (column == 2)
      number = 6;
    else if (column == 3)
      number = 11;
  }
  else if (row == 2)
  {
    if (column == 0)
      number = 7;
    else if (column == 1)
      number = 8;
    else if (column == 2)
      number = 9;
    else if (column == 3)
      number = 12;
  }
  else if (row == 3)
  {
    if (column == 0)
      number = 14;
    else if (column == 1)
      number = 0;
    else if (column == 2)
      number = 15;
    else if (column == 3)
      number = 13;
  }

  if (number < 10)
    Serial.print(number);
  else if (number == 10)
    Serial.print('A');
  else if (number == 11)
    Serial.print('B');
  else if (number == 12)
    Serial.print('C');
  else if (number == 13)
    Serial.print('D');
  else if (number == 14)
    Serial.print('*');
  else if (number == 15)
    Serial.print('#');
  Serial.println();

  if (number < 10)
    return (number);

  return (0); //Not needed
}
