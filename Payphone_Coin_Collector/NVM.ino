void loadSettings()
{
  //Check to see if EEPROM is blank
  uint32_t testRead = 0;
  if (EEPROM.get(0, testRead) == 0xFFFFFFFF)
  {
    Serial.println(F("EEPROM is blank. Default settings applied"));
    recordSystemSettings(); //Record default settings to EEPROM and config file. At power on, settings are in default state
  }

  //Read current settings
  EEPROM.get(0, settings);
}

//Record the current settings struct to EEPROM and then to config file
void recordSystemSettings()
{
  EEPROM.put(0, settings);
}
