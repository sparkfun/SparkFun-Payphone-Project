//This is all the settings that can be set on RTK Surveyor. It's recorded to NVM and the config file.
struct struct_settings {
  int N = 90;                        //Number of samples code will take. You can change for sensitivity. If too large it can slow the arduino.
  float threshold = 2200;            //Minimum tone amplitude to be considered. Found using debug prints in detectTone()
  bool enableToneDebug = false;
  int mp3Volume = 15;
  
  //Stats
  int interactions = 0; //++, p
  int paidCalls = 0; //++, p
  int freeCalls = 0; //++, p
  int dollarCalled = 0; //++, p
  int rickCalled = 0; //++, p
  int jennyCalled = 0; //++, p
  int nine11Called = 0; //++, p
  
} settings;

//System goes from On Hook to VOIP Connect depending on coin and validNoCoinCall numbers.
typedef enum
{
  STATE_ON_HOOK = 0,
  STATE_OFF_HOOK,
  STATE_VOIP_CONNECTED,
  STATE_FREE_CALL,
  STATE_COIN_CALL,
  STATE_VOIP_DISCONNECT,
  STATE_VOIP_DISCONNECTED,
  STATE_EGG_PLAY_RICK,
  STATE_EGG_PLAY_JENNY, //10
  STATE_EGG_PLAY_DOLLAR, //11
  STATE_EGG_PLAY_HOLD, //12
  STATE_EGG_PLAYING, //13
} SystemState;
SystemState systemState = STATE_ON_HOOK;

//As button frequencies are identified, move through states. Starts at 'No Button' at power on.
typedef enum
{
  BUTTON_NO_BUTTON = 0,
  BUTTON_X_FOUND,
  BUTTON_XY_FOUND,
  BUTTON_HELD,
} ButtonState;
ButtonState buttonState = BUTTON_NO_BUTTON;

//Detect if a number is allowed without coin deposit
typedef enum
{
  DIAL_TONE = 0,
  DIAL_9, //911
  DIAL_91,
  DIAL_1, //1-800 and 1-888
  DIAL_18,
  DIAL_180,
  DIAL_183,
  DIAL_184,
  DIAL_185,
  DIAL_186,
  DIAL_187,
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
  DIAL_3032,
  DIAL_30328,
  DIAL_303284,
  DIAL_3032840,
  DIAL_30328409,
  DIAL_303284097,
  DIAL_NOMORE,
} DialState;
DialState dialState = DIAL_TONE;
byte numberPressed = 0;
