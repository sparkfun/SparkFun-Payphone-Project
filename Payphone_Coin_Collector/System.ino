//Get single byte from user
//Waits for and returns the character that the user provides
//Returns STATUS_GETNUMBER_TIMEOUT if input times out
//Returns 'x' if user presses 'x'
uint8_t getByteChoice()
{
  Serial.flush();
  delay(50);//Wait for any incoming chars to hit buffer
  while (Serial.available() > 0) Serial.read(); //Clear buffer

  byte incoming;
  while (1)
  {
    if (Serial.available() > 0)
    {
      incoming = Serial.read();
      if (incoming >= 'a' && incoming <= 'z') break;
      if (incoming >= 'A' && incoming <= 'Z') break;
      if (incoming >= '0' && incoming <= '9') break;
    }
  }

  return (incoming);
}

//Get a string/value from user, remove all non-numeric values
//Returns STATUS_GETNUMBER_TIMEOUT if input times out
//Returns STATUS_PRESSED_X if user presses 'x'
int32_t getNumber()
{
  delay(10); //Wait for any incoming chars to hit buffer
  while (Serial.available() > 0) Serial.read(); //Clear buffer

  //Get input from user
  char cleansed[20]; //Good for very large numbers: 123,456,789,012,345,678\0

  int spot = 0;
  while (spot < 20 - 1) //Leave room for terminating \0
  {
    while (Serial.available() == 0) //Wait for user input
    {
      delay(10); //Yield to processor
    }

    byte incoming = Serial.read();
    if (incoming == '\n' || incoming == '\r')
    {
      Serial.println();
      break;
    }

    if ((isDigit(incoming) == true) || ((incoming == '-') && (spot == 0))) // Check for digits and a minus sign
    {
      Serial.write(incoming); //Echo user's typing
      cleansed[spot++] = (char)incoming;
    }
  }

  cleansed[spot] = '\0';

  int64_t largeNumber = 0;
  int x = 0;
  if (cleansed[0] == '-') // If our number is negative
  {
    x = 1; // Skip the minus
  }
  for ( ; x < spot ; x++)
  {
    largeNumber *= 10;
    largeNumber += (cleansed[x] - '0');
  }
  if (cleansed[0] == '-') // If our number is negative
  {
    largeNumber = 0 - largeNumber; // Make it negative
  }
  return (largeNumber);
}


bool phoneOnHook()
{
  if (digitalRead(pin_hook) == HIGH) return (true);
  return (false);
}

//Trigger relay to disconnect from VOIP service
void disconnectVoip()
{
  Serial.println(F("Disconnect VOIP"));
  digitalWrite(pin_VoipRelayControl, LOW);
}
void connectVoip()
{
  Serial.println(F("Connect VOIP"));
  digitalWrite(pin_VoipRelayControl, HIGH);
}

//Remove handset from system so that we can overlay MP3s
void disconnectHandset()
{
  Serial.println(F("Disconnect Handset"));
  digitalWrite(pin_HandsetRelayControl, HIGH);
}

//Connect handset to system for normal audio
void connectHandset()
{
  Serial.println(F("Connect Handset"));
  digitalWrite(pin_HandsetRelayControl, LOW);
}


//Given a freq, return true if magnitude of freq is above threshold
bool detectTone(float targetFreq)
{
  Goertzel goertzel = Goertzel(targetFreq, settings.N, samplingFreq); //Initialize library function with the given sampling frequency, no of samples, and target freq
  goertzel.sample(pin_keypadDTMF);                               //Will take n samples
  float magnitude = goertzel.detect();                      //Get the magnitude of give freq within signal

  if (settings.enableToneDebug == true)
  {
    Serial.print("F: ");
    Serial.print(targetFreq);
    Serial.print("\t");

    Serial.print("M: ");
    Serial.print(magnitude, 2);
    Serial.print("\t");

    Serial.println();
  }

  if (magnitude > settings.threshold)
    return true;
  else
    return false;
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
  {
    Serial.print(number);
    return (number);
  }
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

  return (0); //We will not deal with special keys
}
