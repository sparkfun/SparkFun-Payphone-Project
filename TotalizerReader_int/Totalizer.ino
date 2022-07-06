
//For a group of pins, must be all on same port
#define PCIE  digitalPinToPCICRbit(PCINT_PIN_Q)
#define PCPORT *portInputRegister(digitalPinToPort(PCINT_PIN_Q))

#define PCINT_PIN_Q 8 //Quarter
#define PCMSK8 *digitalPinToPCMSK(PCINT_PIN_Q)
#define PCINT8 digitalPinToPCMSKbit(PCINT_PIN_Q)

#define PCINT_PIN_N 9 //Nickel
#define PCMSK9 *digitalPinToPCMSK(PCINT_PIN_N)
#define PCINT9 digitalPinToPCMSKbit(PCINT_PIN_N)
#define PCIE9  digitalPinToPCICRbit(PCINT_PIN_N)

#define PCINT_PIN_D 10 //Dime
#define PCMSK10 *digitalPinToPCMSK(PCINT_PIN_D)
#define PCINT10 digitalPinToPCMSKbit(PCINT_PIN_D)
#define PCIE10  digitalPinToPCICRbit(PCINT_PIN_D)

#if (PCIE == 0)
#define PCINT_vect PCINT0_vect
#elif (PCIE == 1)
#define PCINT_vect PCINT1_vect
#elif (PCIE == 2)
#define PCINT_vect PCINT2_vect
#endif

volatile uint8_t totalizerOldPortValue = 0x00;


void beginTotalizer(void) {
  totalizerOldPortValue = PCPORT;

  // pin change mask registers decide which pins are enabled as triggers
  PCMSK8 |= (1 << PCINT8);
  PCMSK9 |= (1 << PCINT9);
  PCMSK10 |= (1 << PCINT10);

  // PCICR: Pin Change Interrupt Control Register - enables interrupt vectors
  PCICR |= (1 << PCIE);
  PCICR |= (1 << PCIE9);
  PCICR |= (1 << PCIE10);
}

ISR(PCINT_vect)
{
  uint8_t newPortValue = PCPORT; //Read the value of this port

  uint8_t change = newPortValue ^ totalizerOldPortValue;

  uint8_t falling = change & totalizerOldPortValue;
  totalizerOldPortValue = newPortValue;

  // check which pins are triggered, compared with the settings
  uint8_t trigger = 0x00;

  trigger |= (falling & (1 << PCINT8));
  trigger |= (falling & (1 << PCINT9));
  trigger |= (falling & (1 << PCINT10));
  
  if (trigger & ((1 << PCINT8) | (1 << PCINT9) | (1 << PCINT10)))
    coinDetected();
}
