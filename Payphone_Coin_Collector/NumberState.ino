//DTMF Decoding
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=
#include "Goertzel.h" //From: https://github.com/jacobrosenthal/Goertzel

//N = 100, threshold of 4000 is a bit sluggish for fast dials. 90/2200 works well with 0.1uF cap inline.
const float samplingFreq = 8900;         //Maximum detectable frequency is the sampling rate/2.
//Arduino Uno with 16MHz can support sampling upto 8900 Hz

float xFrequencies[4] = {1209, 1336, 1477, 1633}; //Known frequencies for a 4x4 keypad. Don't worry if you're using the more common 3x4 keypad.
float yFrequencies[4] = {697, 770, 852, 941};

byte xFreq = 0; //Found freqs when button is pressed
byte yFreq = 0;
//=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=-=

//Attempt to identify any new DTMF frequencies from keypad
void checkKeypad(bool inTestingMode)
{
  if (buttonState == BUTTON_NO_BUTTON)
  {
    //Step through the known xFreq and see if the current signal
    //on A0 is above the threshold for that freq.
    for (byte i = 0 ; i < 4 ; i++)
    {
      if (detectTone(xFrequencies[i]) == true)
      {
        xFreq = i;
        buttonState = BUTTON_X_FOUND;
        break;
      }
    }
  }
  else if (buttonState == BUTTON_X_FOUND)
  {
    //Look for yFreq
    for (byte i = 0 ; i < 4 ; i++)
    {
      if (detectTone(yFrequencies[i]) == true)
      {
        yFreq = i;
        buttonState = BUTTON_XY_FOUND;
        break;
      }
    }

    //If yFreq not found, return to searching state
    if (buttonState == BUTTON_X_FOUND)
    {
      buttonState = BUTTON_NO_BUTTON;
    }
  }
  else if (buttonState == BUTTON_XY_FOUND)
  {
    numberPressed = resolveNumber(yFreq, xFreq);

    if (inTestingMode == false)
      updateNumberState(numberPressed); //Keep track of numbers that are allowed without coin payment. Disconnect as needed.

    buttonState = BUTTON_HELD;
  }
  else if (buttonState == BUTTON_HELD)
  {
    //Look for missing freq to indicate button release
    if (detectTone(xFrequencies[xFreq]) != true || detectTone(yFrequencies[yFreq]) != true)
    {
      buttonState = BUTTON_NO_BUTTON;
    }
  }
}

//Based on number pressed, allow call without coin, or disconnect if bad number is entered
void updateNumberState(int numberPressed)
{
  if (dialState == DIAL_TONE)
  {
    if (numberPressed == 9)
      dialState = DIAL_9;
    else if (numberPressed == 1)
      dialState = DIAL_1;
    else if (numberPressed == 8)
      dialState = DIAL_8;
    else if (numberPressed == 3)
      dialState = DIAL_3;
    else if (numberPressed == 0)
      callStrongBad();
    else
      badNumber();
  }

  //Start 911
  else if (dialState == DIAL_9)
  {
    if (numberPressed == 1)
      dialState = DIAL_91;
    else
      badNumber();
  }
  else if (dialState == DIAL_91)
  {
    if (numberPressed == 1)
    {
      settings.nine11Called++;
      recordSystemSettings();
      validNoCoinCall();
    }
    else
      badNumber();
  }

  //Start 1-800 and 1-833, 1-844, 1-855, 1-866, 1-877, 1-888
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
    else if (numberPressed == 3)
      dialState = DIAL_183;
    else if (numberPressed == 4)
      dialState = DIAL_184;
    else if (numberPressed == 5)
      dialState = DIAL_185;
    else if (numberPressed == 6)
      dialState = DIAL_186;
    else if (numberPressed == 7)
      dialState = DIAL_187;
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
  else if (dialState == DIAL_183)
  {
    if (numberPressed == 3)
      validNoCoinCall();
    else
      badNumber();
  }
  else if (dialState == DIAL_184)
  {
    if (numberPressed == 4)
      validNoCoinCall();
    else
      badNumber();
  }
  else if (dialState == DIAL_185)
  {
    if (numberPressed == 5)
      validNoCoinCall();
    else
      badNumber();
  }
  else if (dialState == DIAL_186)
  {
    if (numberPressed == 6)
      validNoCoinCall();
    else
      badNumber();
  }
  else if (dialState == DIAL_187)
  {
    if (numberPressed == 7)
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
      callJenny();
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
    if (numberPressed == 2)
      dialState = DIAL_3032;
    else
      badNumber();
  }
  else if (dialState == DIAL_3032)
  {
    if (numberPressed == 8)
      dialState = DIAL_30328;
    else
      badNumber();
  }
  else if (dialState == DIAL_30328)
  {
    if (numberPressed == 4)
      dialState = DIAL_303284;
    else
      badNumber();
  }
  else if (dialState == DIAL_303284)
  {
    if (numberPressed == 0)
      dialState = DIAL_3032840;
    else
      badNumber();
  }
  else if (dialState == DIAL_3032840)
  {
    if (numberPressed == 9)
      dialState = DIAL_30328409;
    else
      badNumber();
  }
  else if (dialState == DIAL_30328409)
  {
    if (numberPressed == 7)
      dialState = DIAL_303284097;
    else
      badNumber();
  }
  else if (dialState == DIAL_303284097)
  {
    if (numberPressed == 9)
      callRick();
    else
      badNumber();
  }

}

void callStrongBad()
{
  dialState = DIAL_NOMORE;
  changeState(STATE_EGG_PLAY_HOLD);
  Serial.println(F("Time to go to where tropical breezes flow..."));
}

void callRick()
{
  dialState = DIAL_NOMORE;
  changeState(STATE_EGG_PLAY_RICK);
  Serial.println(F("Time to call Rick..."));
}

void callJenny()
{
  dialState = DIAL_NOMORE;
  changeState(STATE_EGG_PLAY_JENNY);
  Serial.println(F("Time to call Jenny..."));
}

//If a valid number is pressed, connect call
void validNoCoinCall()
{
  dialState = DIAL_NOMORE;
  changeState(STATE_FREE_CALL);
  Serial.println(F("No coin call placed. Connecting..."));
}

//Runs when a bad number is pressed
void badNumber()
{
  dialState = DIAL_NOMORE;
  changeState(STATE_EGG_PLAY_DOLLAR);
  Serial.println(F("Bad Number"));
}
