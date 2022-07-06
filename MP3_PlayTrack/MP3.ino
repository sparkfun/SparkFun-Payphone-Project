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
    Serial.println("Qwiic MP3 failed to respond. Please check wiring and possibly the I2C address. Freezing...");
    while (1)
      ;
  }

  if (mp3.hasCard() == false)
  {
    Serial.println("Qwiic MP3 is missing its SD card. Freezing...");
    while (1)
      ;
  }

  mp3.setVolume(10); //Volume can be 0 (off) to 31 (max)
}

void playRick()
{
  mp3.playTrack(2);
}

void playJenny()
{
  mp3.playTrack(1);
}

void stopMP3()
{
  mp3.stop();
}
