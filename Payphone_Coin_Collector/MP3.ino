//    if (mp3.isPlaying() == true)
//      Serial.println("Pausing");
//    else
//      Serial.println("Playing");


void beginMP3()
{
  Wire.begin();

  //Check to see if Qwiic MP3 is present on the bus
  if (mp3.begin() == false)
  {
    Serial.println(F("Qwiic MP3 failed to respond. Please check wiring and possibly the I2C address."));
  }

  if (mp3.hasCard() == false)
  {
    Serial.println(F("Qwiic MP3 is missing its SD card."));
  }

  if(mp3.isPlaying())
    mp3.stop();

  mp3.setVolume(settings.mp3Volume); //Volume can be 0 (off) to 31 (max)

  Serial.println(F("MP3 Setup Complete"));
}

void playRick()
{
  Serial.println(F("Time to call Rick..."));
  mp3.playFile(3);
}

void playJenny()
{
  mp3.playFile(2);
}

void playDollar()
{
  Serial.println(F("Time to call Aloe..."));
  mp3.playFile(1);
}

void playStrongBad()
{
  mp3.playFile(4);
}

void stopMP3()
{
  mp3.stop();
}
