/* PORT B
   [0] Status LED (OUT)
   [1] NRF Chip Enable (OUT)
   [2] NRF Chip Select (SPI_SS)
   [3] NRF MOSI (SPI_MOSI)
   [4] NRF MISO (SPI_MISO)
   [5] NRF SCK (SPI_SCK)
*/

#define STATUS_LED_PORT PORTB
#define STATUS_LED_DDR  DDRB
#define STATUS_LED_PIN  PINB
#define STATUS_LED_MSK  _BV(0)
#define STATUS_LED_NUMBER  0

#define NRF_CE_PORT PORTB
#define NRF_CE_DDR  DDRB
#define NRF_CE_PIN  PINB
#define NRF_CE_MSK  _BV(1)
#define NRF_CE_NUMBER  1

#define NRF_CS_PORT PORTB
#define NRF_CS_DDR  DDRB
#define NRF_CS_PIN  PINB
#define NRF_CS_MSK  _BV(2)
#define NRF_CS_NUMBER  2

#define NRF_MOSI_PORT PORTB
#define NRF_MOSI_DDR  DDRB
#define NRF_MOSI_PIN  PINB
#define NRF_MOSI_MSK  _BV(3)
#define NRF_MOSI_NUMBER  3

#define NRF_MISO_PORT PORTB
#define NRF_MISO_DDR  DDRB
#define NRF_MISO_PIN  PINB
#define NRF_MISO_MSK  _BV(4)
#define NRF_MISO_NUMBER  4

#define NRF_SCK_PORT PORTB
#define NRF_SCK_DDR  DDRB
#define NRF_SCK_PIN  PINB
#define NRF_SCK_MSK  _BV(5)
#define NRF_SCK_NUMBER  5


/* PORT C (ADC)
   [0] Temperature Sensor (IN analog)
   [1] DC SS Relay (strike) (OUT)
   [2] AC SS Relay (peripheral) (OUT)
*/

#define ADC_PORT PORTC
#define ADC_DDR  DDRC
#define ADC_PIN  PINC
#define ADC_MSK  0xff

#define TEMP_SENSOR_PORT PORTC
#define TEMP_SENSOR_DDR  DDRC
#define TEMP_SENSOR_PIN  PINC
#define TEMP_SENSOR_MSK  _BV(0)
#define TEMP_SENSOR_NUMBER  0

#define DC_RELAY_PORT PORTC
#define DC_RELAY_DDR  DDRC
#define DC_RELAY_PIN  PINC
#define DC_RELAY_MSK  _BV(1)
#define DC_RELAY_NUMBER  1

#define AC_RELAY_PORT PORTC
#define AC_RELAY_DDR  DDRC
#define AC_RELAY_PIN  PINC
#define AC_RELAY_MSK  _BV(2)
#define AC_RELAY_NUMBER  2


/* PORT D
   [3] NRF IRQ (in)
*/

#define NRF_IRQ_PORT PORTD
#define NRF_IRQ_DDR  DDRD
#define NRF_IRQ_PIN  PIND
#define NRF_IRQ_MSK  _BV(3)
#define NRF_IRQ_NUMBER  3


