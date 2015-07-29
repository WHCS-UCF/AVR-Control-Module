# Makefile for WHCS Control Modules
PROGRAM=control-module
SOURCES=main.cpp

LIBRARIES=lib/MEGA32A_UART_LIBRARY/MEGA32A_UART_LIBRARY/libUART.a \
	  lib/MEGA32A_SPI_LIBRARY/MEGA32A_SPI_LIBRARY/libSPI.a \
	  lib/WHCS_RF24/WHCS_RF24/libRF24.a \
	  common/libcommon.a
CFLAGS=-Wall -Wextra -Werror

# chip and CPU speed information
MCU=atmega328p
F_CPU=16000000L
AVRDUDE_MCU=m328p

# define hardware baud rate
export USART_BAUD=115200

ifdef MAKE_DOOR
DEFINES=-DMAKE_DOOR
endif
ifdef MAKE_LIGHT
DEFINES=-DMAKE_LIGHT
endif
ifdef MAKE_OUTLET
DEFINES=-DMAKE_OUTLET
endif
ifdef MAKE_TEMP
DEFINES=-DMAKE_TEMP
endif

include atmega.mk
