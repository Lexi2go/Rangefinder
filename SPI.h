/*
 * SPI.h
 *
 * Created: 11.01.2015 12:29:37
 *  Author: Petre Professor
 */ 


#ifndef SPI_H_
#define SPI_H_

#include <avr/io.h>

//****************************************************************************************
//define's
//****************************************************************************************
typedef struct{

	volatile uint8_t* CS_DDR;
	volatile uint8_t* CS_PORT;
	uint8_t CS_pin;
	uint8_t CS_state;
} tspiHandle;

//SPI Register
#define SPI_CONTROL_REGISTER		SPCR
#define SPI_DATA_REGISTER			SPDR
#define SPI_STATUS_REGISTER			SPSR
//
#define SPI_ENABLE					0x40
#define SPI_MASTER					0x10
//SPIE-Bit in das SPCR-Register (SPI-Interupt Enable Bit)
#define SPI_INTERRUPT_ENABLE		0x80
#define SPI_INTERRUPT_DISABLE		0x00
//DORD-Bit in das SPCR-Register (data order LSB or MSB first)
#define SPI_LSB_FIRST				0x20
#define SPI_MSB_FIRST				0x00
//SPI-Modus wird �ber 2 Bits bestimmt CPOL und CPHA im SPCR-Register
#define SPI_MODE_0					0x00
#define SPI_MODE_1					0x01
#define SPI_MODE_2					0x02
#define SPI_MODE_3					0x03
//Taktfrequenz der SPI-Schnittstelle (FOSC = Taktfrequenz des Mikrocontrollers)
#define SPI_FOSC_DIV_2				0x04
#define SPI_FOSC_DIV_4				0x00
#define SPI_FOSC_DIV_8				0x05
#define SPI_FOSC_DIV_16				0x01
#define SPI_FOSC_DIV_32				0x06
#define SPI_FOSC_DIV_64				0x02
#define SPI_FOSC_DIV_128			0x03

#define SPI_RUNNING					(!(SPSR & (1 << SPIF)))

//****************************************************************************************
//Funktionen
//****************************************************************************************
void SPI_Master_Init(uint8_t ucspi_interrupt, uint8_t ucspi_data_order, uint8_t ucspi_mode, uint8_t ucspi_sck_freq);
void SPI_Master_SlaveSelectInit(tspiHandle tspi_pins);
void SPI_Master_Start(tspiHandle tspi_pins);
void SPI_Master_Stop(tspiHandle tspi_pins);
unsigned char SPI_Master_Write(uint8_t ucdata);

//****************************************************************************************
//Definitionen f�r den ATMega88 Mikrocontroller
//dieser Teil muss neu definiert werden f�r einen anderen Mikrocontroller
//das SlaveSelect- Signal ist von der konkreten Anwendung abh�ngig, deshalb wird es in 
//der main-Datei behandelt
//****************************************************************************************
//MOSI-Signal
#define SPI_MOSI_DDR_REG			DDRB
#define SPI_MOSI_PORT_REG			PORTB
#define SPI_MOSI_BIT				PB3
//MISO-Signal
#define SPI_MISO_DDR_REG			DDRB
#define SPI_MISO_PORT_REG			PORTB
#define SPI_MISO_BIT				PB4
//Clock-Signal
#define SPI_CLK_DDR_REG				DDRB
#define SPI_CLK_PORT_REG			PORTB
#define SPI_CLK_BIT					PB5

#endif /* SPI_H_ */