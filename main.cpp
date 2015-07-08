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
Radio radio(&rf24, 0x0);

#define MAIN_LOOP_WARNING 300 // 300ms maximum main loop time until warning
static FILE mystdout;
static FILE mystdin;

int main()
{
  // initialize timing (millis()) as the first call 
  timing_init();
  WHCSADC::init(); // this sets the ADC port as all inputs
  initUart();

  // setup STDIN/STDOUT for printf
  fdev_setup_stream(&mystdout, uart_putchar, NULL, _FDEV_SETUP_WRITE);
  fdev_setup_stream(&mystdin, NULL, uart_getchar, _FDEV_SETUP_READ);
  stdout = &mystdout;
  stdin = &mystdin;
  
  printf("[W]ireless [H]ome [C]ontrol [S]ystem Control Module\n");

  // show that we're powered up
  PIN_MODE_OUTPUT(STATUS_LED);
  PIN_HIGH(STATUS_LED);

  //radio.begin();

  printf_P(PSTR("WHCS main loop starting\n"));

  // Enable interrupts before entering main event loop
  sei();

  time_t maxLoopTime = 0;

  // Main event loop
  Timer evt;
  evt.periodic(3000);

  radio_pkt pkt;

  PIN_MODE_OUTPUT(DC_RELAY);

  rf24.begin();
  rf24.openReadingPipe(1, 0xE8E8F0F000LL | 0x00);
  rf24.startListening();
  rf24.printDetails();

  while(1)
  {
    unsigned long mainStart = millis();

    pkt.size = 1;

    //radio.sendTo(0xe5, &pkt);
    if(rf24.available())
    {
      size_t amt = rf24.read(pkt.data, 32);

      printf("Got data %02x amt %u\n", pkt.data[1], amt);

      if(pkt.data[1] == 'O') {
        printf("ON\n");
        PIN_HIGH(STATUS_LED);
        PIN_HIGH(DC_RELAY);
      }
      else {
        printf("OFF\n");
        PIN_LOW(STATUS_LED);
        PIN_LOW(DC_RELAY);
      }
    }

    time_t delta = millis() - mainStart;

    if(delta > maxLoopTime) {
      if(delta > MAIN_LOOP_WARNING)
        printf("WARNING LIMIT: ");

      printf("Main loop max: %lums\n", delta);

      maxLoopTime = delta;
    }
  }

  // main must not return
  for(;;);
}
