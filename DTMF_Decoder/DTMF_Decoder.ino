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
    printNumber(yFreq, xFreq);
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

//Given a freq, return true if magnitude of freq is above threshold
bool detectTone(float targetFreq)
{
  Goertzel goertzel = Goertzel(targetFreq, N, samplingFreq); //Initialize library function with the given sampling frequency, no of samples, and target freq
  goertzel.sample(sensorPin);                               //Will take n samples
  float magnitude = goertzel.detect();                      //Get the magnitude of give freq within signal

    Serial.print("F: ");
    Serial.print(targetFreq);
    Serial.print("\t");
  
    Serial.print("M: ");
    Serial.print(magnitude, 2);
    Serial.print("\t");
  
    Serial.println();

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
void printNumber(int row, int column)
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
}
