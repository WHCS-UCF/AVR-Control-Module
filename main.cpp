#include <avr/io.h>
#include <avr/interrupt.h>
#include <util/delay.h>
#include <stdio.h>

#include <MEGA32A_UART_LIBRARY.h>
#include <RF24.h>

#include "ADC.h"
#include "timing.h"
#include "Radio.h"
#include "Timer.h"

#include "pinout.h"

#define BASE_STATION_ID 0xde

#ifdef MAKE_DOOR
  #pragma message "Making door module"
  #define CTRL_ID 0
  #define CTRL_NAME "Door"
#elif MAKE_LIGHT
  #pragma message "Making light module"
  #define CTRL_ID 1
  #define CTRL_NAME "Light"
#elif MAKE_TEMP
  #pragma message "Making temperature module"
  #define CTRL_ID 2
  #define CTRL_NAME "Temperature"
#elif MAKE_OUTLET
  #pragma message "Making outlet module"
  #define CTRL_ID 3
  #define CTRL_NAME "Outlet"
#else
  #error "Control module unconfigured!"
#endif

int uart_putchar(char c, FILE *stream) {
  if (c == '\n')
    uart_putchar('\r', stream);

  USART_SendByte(c);

  return 1;
}

int uart_getchar(FILE *stream) {
  return USART_ReceiveByte();
}

RF24 rf24(NRF_CE_NUMBER, NRF_CS_NUMBER); // pins on PORTB ONLY
Radio radio(&rf24, CTRL_ID);

#define MAIN_LOOP_WARNING 300 // 300ms maximum main loop time until warning
static FILE mystdout;
static FILE mystdin;

//ISR(INT1_vect)
//{
//  radio_pkt pkt;
//  if(rf24.read(&pkt, MAX_PACKET_SIZE))
//    if(!radio.queuePacket(&pkt));
//      //printf("\n");
//}

// called instead of RESET (vect0) for a bad ISR
ISR(BADISR_vect)
{
  PIN_MODE_OUTPUT(STATUS_LED);

  while(1) {
    PIN_HIGH(STATUS_LED);
    _delay_ms(500);
    PIN_LOW(STATUS_LED);
    _delay_ms(500);
  }
}

// reset the processor
void reset()
{
  // XXX: call reset
  while(1);
}

int main()
{
  // initialize timing (millis()) as the first call 
  timing_init();
#ifdef MAKE_TEMP
  WHCSADC::init(); // this sets the ADC port as all inputs
#endif
  initUart();

  // setup STDIN/STDOUT for printf
  fdev_setup_stream(&mystdout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
  fdev_setup_stream(&mystdin, NULL, uart_getchar, _FDEV_SETUP_READ);
  stdout = &mystdout;
  stdin = &mystdin;
  
  printf_P(PSTR("[W]ireless [H]ome [C]ontrol [S]ystem Control Module\n"));
  printf_P(PSTR("Module name: " CTRL_NAME "\n"));
  printf_P(PSTR("Module ID: %d\n"), CTRL_ID);

  // show that we're powered up
  PIN_MODE_OUTPUT(STATUS_LED);
  PIN_HIGH(STATUS_LED);

  // init the control module ports
#if defined(MAKE_LIGHT) || defined(MAKE_OUTLET)
  printf_P(PSTR("AC relay initialized\n"));
  PIN_MODE_OUTPUT(AC_RELAY);
  PIN_LOW(AC_RELAY);
#elif defined(MAKE_DOOR)
  printf_P(PSTR("DC relay initialized\n"));
  PIN_MODE_OUTPUT(DC_RELAY);
  PIN_LOW(DC_RELAY);
#endif

  radio.begin();
  //radio.enableInterrupt();
  //rf24.printDetails();

  printf_P(PSTR("WHCS main loop starting\n"));

  // Enable interrupts before entering main event loop
  sei();

  // keep track of the worst case loop performance
  Timer tBlink;
  time_t maxLoopTime = 0;
  Timer loopTimeReset;
  loopTimeReset.once(5000);

  // module state
  uint8_t currentState = 0;

#ifdef MAKE_TEMP
#define NUM_TEMP_SAMPLES 10
#define TEMP_UPDATE_INTERVAL 1000
  int8_t tempAvg = 0;
  int8_t lastTempAvg = 0;
  float tempSamples[NUM_TEMP_SAMPLES] = {0};
  uint8_t tempSamplesHead = 0;
  uint8_t numSamples = 0;
  Timer tTemp;
  tTemp.periodic(TEMP_UPDATE_INTERVAL);
#endif

  // Main event loop
  while(1)
  {
    unsigned long mainStart = millis();

    ///////////////////////////////////////
    // BEGIN MAIN LOOP TIMING
    ///////////////////////////////////////
    while(rf24.available())
    {
      //printf("RF AVAIL\n");
      radio_pkt pkt;
      if(rf24.read(&pkt, MAX_PACKET_SIZE))
        if(!radio.queuePacket(&pkt))
          printf("WARNING: Failed to queue\n");
    }

    while(radio.available())
    {
      //printf("RAD AVAIL\n");
      radio_pkt pkt;
      radio.recv(&pkt);

      switch(pkt.opcode)
      {
        case RADIO_OP_SET_STATUS:
        {
          if(pkt.size == 1)
          {
            printf(CTRL_NAME ": set status %hu\n", pkt.data[0]);

#ifdef MAKE_DOOR
            if(pkt.data[0])
            {
              PIN_HIGH(DC_RELAY);
              currentState = 1;
            }
            else
            {
              PIN_LOW(DC_RELAY);
              currentState = 0;
            }
#elif defined(MAKE_LIGHT) || defined(MAKE_OUTLET)
            if(pkt.data[0])
            {
              PIN_HIGH(AC_RELAY);
              currentState = 1;
            }
            else
            {
              PIN_LOW(AC_RELAY);
              currentState = 0;
            }
#elif defined(MAKE_TEMP)
            printf_P(PSTR("Invalid message for control module type\n"));
#endif
          }

          break;
        }
        case RADIO_OP_GET_STATUS:
        {
#ifdef MAKE_DOOR
          uint8_t result = currentState;
#elif defined(MAKE_LIGHT) || defined(MAKE_OUTLET)
          uint8_t result = currentState;
#elif defined(MAKE_TEMP)
          int8_t result = tempAvg;
          printf("Temp %d deg F\n", result);
#endif
          if(!radio.sendTo(pkt.from, RADIO_OP_SUCCESS_RESULT, (uint8_t*)&result, sizeof(result)))
          {
            printf(CTRL_NAME ": get status resp failed\n", pkt.data[0]);
          }

          break;
        }
        case RADIO_OP_PING:
          if(pkt.from == BASE_STATION_ID)
          {
            radio.sendTo(BASE_STATION_ID, RADIO_OP_PONG, NULL, 0);
          }
          break;
        case RADIO_OP_RESTART:
          if(pkt.from == BASE_STATION_ID)
          {
            printf_P(PSTR("Restarting control module...\n"));
            reset(); // no return
          }
          break;
        default:
          printf_P(PSTR("Invalid opcode received: %02x\n"), pkt.opcode);
      }

      // little status blink
      tBlink.once(100);
      PIN_LOW(STATUS_LED);
    }

    if(tBlink.update())
      PIN_HIGH(STATUS_LED);

#if MAKE_TEMP
    if(tTemp.update())
    {
      // TODO: average temperature
      int16_t mV = WHCSADC::read(TEMP_SENSOR_NUMBER);
      float result = (mV*5.0 - 500.0)/10.0;

      tempSamples[tempSamplesHead] = result;
      numSamples++;
      numSamples = min(numSamples, NUM_TEMP_SAMPLES);

      float accum = 0;

      for(int i = 0, iter = tempSamplesHead; i < numSamples; i++) {
        accum += tempSamples[iter];
        iter--;

        if(iter < 0)
          iter = NUM_TEMP_SAMPLES-1;
      }
      //printf("accum %d, num %d\n", accum, numSamples);

      float avg = (float)accum / numSamples;
      float avgF = avg*9.0/5 + 32;
      int8_t outTemp = (int8_t)avgF;

      lastTempAvg = tempAvg;
      tempAvg = outTemp;
      printf("Temp %dF, %dC (out %hd)\n", (int)avgF, (int)avg, outTemp);
      //printf("Listening %d\n", PIN_READ(NRF_CE));

      if(lastTempAvg != tempAvg) // update the base station
      {
        // don't care if it gets there
        radio.sendTo(BASE_STATION_ID, RADIO_OP_NEWDATA, (uint8_t *)&outTemp, sizeof(outTemp));
      }

      tempSamplesHead = (tempSamplesHead + 1) % NUM_TEMP_SAMPLES;
    }
#endif

    ///////////////////////////////////////
    // END MAIN LOOP TIMING
    ///////////////////////////////////////

    time_t delta = millis() - mainStart;

    if(loopTimeReset.update())
      maxLoopTime = 0;

    if(delta > maxLoopTime) {
      loopTimeReset.once(5000);

      if(delta > MAIN_LOOP_WARNING)
        printf_P(PSTR("WARNING LIMIT REACHED: "));

      printf_P(PSTR("Main loop max: %lums"), delta);
      //rf24.printDetails();
      //rf24.startListening();

      if(maxLoopTime > 0)
        printf_P(PSTR(" (+%lums)\n"), delta-maxLoopTime);
      else
        printf_P(PSTR("\n"));

      maxLoopTime = delta;
    }
  }

  // main must not return
  for(;;);
}
