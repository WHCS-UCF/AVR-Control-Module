# Makefile for WHCS Control Modules
PROGRAM=control-module
SOURCES=main.cpp

LIBRARIES=lib/MEGA32A_UART_LIBRARY/MEGA32A_UART_LIBRARY/ \
	  lib/MEGA32A_SPI_LIBRARY/MEGA32A_SPI_LIBRARY/ \
	  lib/WHCS_RF24/WHCS_RF24/ \
	  common/

INCLUDE=$(LIBRARIES)
LIB_INCLUDE=$(LIBRARIES)

# include any libraries
LDFLAGS=-lcommon -lUART -lRF24 -lSPI

# chip and CPU speed information
MCU=atmega328p
F_CPU=16000000L
AVRDUDE_MCU=m328p

# define hardware baud rate
export USART_BAUD=115200

include atmega.mk
