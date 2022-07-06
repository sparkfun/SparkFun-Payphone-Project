//Based on inputs, move from state to state
void updateSystemState()
{
  if (systemState == STATE_ON_HOOK)
  {
    if (phoneOnHook() == false)
    {
      if (mp3.isPlaying()) mp3.stop();

      coinDetected = false;
      buttonState = BUTTON_NO_BUTTON;
      dialState = DIAL_TONE;
      changeState(STATE_OFF_HOOK);
      connectHandset(); //Connect handset to VOIP system
    }
  }
  else if (systemState == STATE_OFF_HOOK)
  {
    if (phoneOnHook() == true)
    {
      disconnectVoip(); //Hang up VOIP. This causes VOIP to disconnect previous call.
      delay(1000);
      connectVoip(); //VOIP must be connected so inbound calls ring through
      changeState(STATE_ON_HOOK);
    }

    checkKeypad(false); //Attempt to identify any new DTMF frequencies from keypad. Change system state accordingly.

    if (coinDetected == true)
    {
      //Toggle handset to mimic payphone behavior
      disconnectHandset();
      delay(300);
      connectHandset(); //Connect handset to VOIP system

      changeState(STATE_COIN_CALL);
    }
  }
  else if (systemState == STATE_FREE_CALL || systemState == STATE_COIN_CALL)
  {
    changeState(STATE_VOIP_CONNECTED);
  }
  else if (systemState == STATE_VOIP_CONNECTED)
  {
    //Do nothing (or start call time limit) until hangup
    if (phoneOnHook() == true)
    {
      disconnectVoip(); //Hang up VOIP. This causes VOIP to disconnect previous call.
      delay(1000);
      connectVoip(); //VOIP must be connected so inbound calls ring through
      changeState(STATE_ON_HOOK);
    }
  }
  else if (systemState == STATE_VOIP_DISCONNECT)
  {
    disconnectVoip();
    changeState(STATE_VOIP_DISCONNECTED);
  }
  else if (systemState == STATE_VOIP_DISCONNECTED)
  {
    if (phoneOnHook() == true)
    {
      disconnectVoip(); //Hang up VOIP. This causes VOIP to disconnect previous call.
      delay(1000);
      connectVoip(); //VOIP must be connected so inbound calls ring through
      changeState(STATE_ON_HOOK); //Do nothing until hangup
    }
  }
  else if (systemState == STATE_EGG_PLAY_JENNY)
  {
    delay(500);
    disconnectVoip();
    disconnectHandset();

    playJenny(); //Start song

    changeState(STATE_EGG_PLAYING);
  }
  else if (systemState == STATE_EGG_PLAY_RICK)
  {
    delay(500);
    disconnectVoip();
    disconnectHandset();

    playRick(); //Start song

    changeState(STATE_EGG_PLAYING);
  }
  else if (systemState == STATE_EGG_PLAY_DOLLAR)
  {
    delay(500);
    disconnectVoip();
    disconnectHandset();

    playDollar(); //Start song

    changeState(STATE_EGG_PLAYING);
  }
  else if (systemState == STATE_EGG_PLAY_HOLD)
  {
    delay(500);
    disconnectVoip();
    disconnectHandset();

    playStrongBad(); //Start song

    changeState(STATE_EGG_PLAYING);
  }
  else if (systemState == STATE_EGG_PLAYING)
  {
    if (phoneOnHook() == true)
    {
      mp3.stop();
      disconnectVoip(); //Hang up VOIP. This causes VOIP to disconnect previous call.
      delay(1000);
      connectVoip(); //VOIP must be connected so inbound calls ring through
      changeState(STATE_ON_HOOK); //Do nothing until hangup
    }
  }
}

//Change states and print the new state
void changeState(SystemState newState)
{
  if (newState == STATE_OFF_HOOK) settings.interactions++;
  if (newState == STATE_EGG_PLAY_DOLLAR) settings.dollarCalled++;
  if (newState == STATE_EGG_PLAY_JENNY) settings.jennyCalled++;
  if (newState == STATE_EGG_PLAY_RICK) settings.rickCalled++;
  if (newState == STATE_FREE_CALL) settings.freeCalls++;
  if (newState == STATE_COIN_CALL) settings.paidCalls++;

  recordSystemSettings(); //Record stats

  systemState = newState;

  //Debug print
  switch (systemState)
  {
    case (STATE_ON_HOOK):
      Serial.println(F("State: On Hook"));
      break;
    case (STATE_OFF_HOOK):
      Serial.println(F("State: Off Hook"));
      break;
    case (STATE_FREE_CALL):
      Serial.println(F("State: Free Call"));
      break;
    case (STATE_COIN_CALL):
      Serial.println(F("State: Coin Call"));
      break;
    case (STATE_VOIP_CONNECTED):
      Serial.println(F("State: VOIP Connected"));
      break;
    case (STATE_VOIP_DISCONNECTED):
      Serial.println(F("State: VOIP Disconnect"));
      break;
    case (STATE_EGG_PLAY_RICK):
      Serial.println(F("State: Egg Play Rick"));
      break;
    case (STATE_EGG_PLAY_JENNY):
      Serial.println(F("State: Egg Play Jenny"));
      break;
    case (STATE_EGG_PLAY_DOLLAR):
      Serial.println(F("State: Egg Play Dollar"));
      break;
    case (STATE_EGG_PLAY_HOLD):
      Serial.println(F("State: Egg Play Hold"));
      break;
    case (STATE_EGG_PLAYING):
      Serial.println(F("State: Egg Playing"));
      break;

    default:
      Serial.print(F("Change State Unknown: "));
      Serial.println(systemState);
      break;
  }
}
