/*
 * MCP2515_HHN.h
 *
 * Created: 07.03.2017 10:26:27
 *  Author: Professor
      
      * Disclaimer: This code sample is just a prototype. It is in no way suitable
      * for safe and reliable code, since the authors have intentionally abstained
      * from error or plausibility checks.
      * Therefore, the code examples must not be used in military or commercial or
      * safety-related products. Running this software can potentially be dangerous.
      *
      * THIS SOFTWARE IS PROVIDED BY THE AUTHOR AND CONTRIBUTORS ``AS IS'' AND
      * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
      * IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
      * ARE DISCLAIMED.  IN NO EVENT SHALL THE REGENTS OR CONTRIBUTORS BE LIABLE
      * FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL
      * DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS
      * OR SERVICES; LOSS OF USE, DATA, OR PROFITS, OR BUSINESS INTERRUPTION)
      * HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT
      * LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY
      * OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF
      * SUCH DAMAGE.
      *
      * Redistribution and use in source and binary forms, with or without
      * modification, are permitted under license CC-BY-NC-SA provided that the
      * following conditions are met:
      * 1. Redistributions of source code must retain the above copyright
      *    notice, this list of conditions and the above disclaimer.
      * 2. Redistributions in binary form must reproduce the above copyright
      *    notice, this list of conditions and the above disclaimer in the
      *    documentation and/or other materials provided with the distribution.
 */ 


#ifndef MCP2515_HHN_H_
#define MCP2515_HHN_H_

#include <avr/io.h>
#include "SPI.h"


typedef struct 
{
	tspiHandle MCP2515spi;
} MCP2515_pins;

typedef struct
{
	uint8_t EIDE_Bit; //STANDARD_ID oder EXTENDED_ID
	uint8_t RTR_Bit; //DATA_FRAME oder REMOTE_FRAME
	uint32_t ulID; //11 oder 29 Bit ID
	uint8_t ucLength; //Anzahl der Datenbytes
	uint8_t ucData[8]; //Datenvektor für die maximale Nachrichtenlänge
}can_frame;

typedef struct  
{
	uint8_t RecBuff_ID[2]; //STANDARD_ID oder EXTENDED_ID
	uint8_t Rec_Buff0_Rollover; //ROLLOVER_ON / ROLLOVER_OFF
	uint8_t Filter_RecBuff[2]; //FILTER_ON oder FILTER_OFF
	uint32_t ulRecBuff_Filter[6]; //die ersten 2 Filter sind für den Buffer1, die weiteren 4 für den Zweiten
	uint32_t ulRecBuff_Mask[2]; //Filtermasken der Empfangspuffer
}can_filter;

//****************************************************************************************
//Funktionen
//****************************************************************************************
void MCP2515_Init(MCP2515_pins sdevice_pins, uint8_t ucbaud);
void MCP2515_Read_Reg(MCP2515_pins sdevice_pins, uint8_t ucreg_address, uint8_t ucreg_number, uint8_t *ucreg_in);
void MCP2515_Write_Reg(MCP2515_pins sdevice_pins, uint8_t ucreg_address, uint8_t ucreg_number, uint8_t *ucreg_out);
void MCP2515_Change_Reg(MCP2515_pins sdevice_pins, uint8_t ucreg_address, uint8_t ucreg_mask, uint8_t ucreg_data);
void MCP2515_Set_OpMode(MCP2515_pins sdevice_pins, uint8_t ucop_mode);
void MCP2515_Change_ClkOut(MCP2515_pins sdevice_pins, uint8_t ucclk_out);
void MCP2515_OneShotMode(MCP2515_pins sdevice_pins, uint8_t ucone_shot);
void MCP2515_Set_Baudrate(MCP2515_pins sdevice_pins, uint8_t ucbaud);
uint8_t MCP2515_Read_Status(MCP2515_pins sdevice_pins);
void MCP2515_Load_TXBuffer(MCP2515_pins sdevice_pins, uint8_t ucbuffer_start, uint8_t ucbuffer_length, uint8_t *ucreg_out);
uint8_t MCP2515_Send_Message(MCP2515_pins sdevice_pins, can_frame *sframe);
void MCP2515_RequestToSend(MCP2515_pins sdevice_pins, uint8_t uctx_buffer);
void MCP2515_Set_Filter_Mask(MCP2515_pins sdevice_pins, can_filter *sfilter);
void MCP2515_Build_Filter_Frame(uint32_t ulfilter_id, uint8_t ucdata_frame);
uint8_t MCP2515_Read_RxStatus(MCP2515_pins sdevice_pins);
uint8_t MCP2515_Check_Message(MCP2515_pins sdevice_pins, can_frame *sframe);
void MCP2515_Read_RxBuffer(MCP2515_pins sdevice_pins, uint8_t ucbuffer_start, uint8_t ucbuffer_length, uint8_t *ucreg_in);

//****************************************************************************************
//define's
//****************************************************************************************
#define ON					1
#define OFF					0

//OP-codes
#define RESET_CODE			0xC0
#define READ_REG_CODE		0x03
#define READ_RX_BUFFER_CODE	0x90
#define WRITE_REG_CODE		0x02
#define LOAD_TX_BUFFER_CODE	0x40
#define RTS_CODE			0x80
#define READ_STATUS_CODE	0xA0
#define READ_RX_STATUS_CODE	0xB0
#define BIT_MODIFY_CODE		0x05

//Operation mode
#define OP_MODE_MASK		0xE0
#define NORMAL_OP_MODE		0x00
#define SLEEP_MODE			0x20
#define LOOPBACK_MODE		0x40
#define LISTEN_ONLY_MODE	0x60
#define CONFIG_MODE			0x80

//Clock-out
#define CLOCK_OUT_MASK		0x07
#define CLOCK_OUT_DISABLE	0x00
#define ENABLE_FOUT_1TO1	0x04
#define ENABLE_FOUT_1TO2	0x05
#define ENABLE_FOUT_1TO4	0x06
#define ENABLE_FOUT_1TO8	0x07

//One-Shot Mode
#define ONE_SHOT_MASK		0x08
#define ONE_SHOT_ENABLE		0x08
#define ONE_SHOT_DISABLE	0x00

//Control Register Address
#define BFPCTRL			0x0C
#define TXRTSCTRL		0x0D
#define CANSTAT			0x0E
#define CANCTRL			0x0F
#define TEC				0x1C
#define REC				0x1D
#define CNF3			0x28 //nur im Config mode einstellbar
#define CNF2			0x29 //nur im Config mode einstellbar
#define CNF1			0x2A //nur im Config mode einstellbar
#define CANINTE			0x2B
#define CANINF			0x2C
#define EFLG			0x2D
#define TXB0CTRL		0x30
#define TXB1CTRL		0x40
#define TXB2CTRL		0x50
#define RXB0CTRL		0x60
#define RXB1CTRL		0x70

//Transmit Register Address
//Sendepuffer 1
#define TXBN0CTRL		0x30
#define TXB0SIDH		0x31
#define TXB0SIDL		0x32
#define TXB0EID8		0x33
#define TXB0EID0		0x34
#define TXB0DLC			0x35
#define TXB0D0			0x36 //das erste der 8 Datenregister
//Sendepuffer 2
#define TXBN1CTRL		0x40
#define TXB1SIDH		0x41
#define TXB1SIDL		0x42
#define TXB1EID8		0x43
#define TXB1EID0		0x44
#define TXB1DLC			0x45
#define TXB1D0			0x46 //das erste der 8 Datenregister
//Sendepuffer 3
#define TXBN2CTRL		0x50
#define TXB2SIDH		0x51
#define TXB2SIDL		0x52
#define TXB2EID8		0x53
#define TXB2EID0		0x54
#define TXB2DLC			0x55
#define TXB2D0			0x56 //das erste der 8 Datenregister

//Receive Register
//Empfangspuffer 1
#define RXB0SIDH		0x61
#define RXB0SIDL		0x62
#define RXB0EID8		0x63
#define RXB0EID0		0x64
#define RXB0DLC			0x65
#define RXB0D0			0x66 //das erste der 8 Datenregister
//Empfangspuffer 2
#define RXB1SIDH		0x71
#define RXB1SIDL		0x72
#define RXB1EID8		0x73
#define RXB1EID0		0x74
#define RXB1DLC			0x75
#define RXB1D0			0x76 //das letzte der 8 Datenregister

//Receive Filter Register
#define RXF0SIDH		0x00 //nur im Config mode einstellbar
#define RXF0SIDL		0x01 //nur im Config mode einstellbar
#define RXF0EID8		0x02 //nur im Config mode einstellbar
#define RXF0EID0		0x03 //nur im Config mode einstellbar
#define RXF1SIDH		0x04 //nur im Config mode einstellbar
#define RXF1SIDL		0x05 //nur im Config mode einstellbar
#define RXF1EID8		0x06 //nur im Config mode einstellbar
#define RXF1EID0		0x07 //nur im Config mode einstellbar
#define RXF2SIDH		0x08 //nur im Config mode einstellbar
#define RXF2SIDL		0x09 //nur im Config mode einstellbar
#define RXF2EID8		0x0A //nur im Config mode einstellbar
#define RXF2EID0		0x0B //nur im Config mode einstellbar
#define RXF3SIDH		0x10 //nur im Config mode einstellbar
#define RXF3SIDL		0x11 //nur im Config mode einstellbar
#define RXF3EID8		0x12 //nur im Config mode einstellbar
#define RXF3EID0		0x13 //nur im Config mode einstellbar
#define RXF4SIDH		0x14 //nur im Config mode einstellbar
#define RXF4SIDL		0x15 //nur im Config mode einstellbar
#define RXF4EID8		0x16 //nur im Config mode einstellbar
#define RXF4EID0		0x17 //nur im Config mode einstellbar
#define RXF5SIDH		0x18 //nur im Config mode einstellbar
#define RXF5SIDL		0x19 //nur im Config mode einstellbar
#define RXF5EID8		0x1A //nur im Config mode einstellbar
#define RXF5EID0		0x1B //nur im Config mode einstellbar

//Receive Filter Mask
#define RXM0SIDH		0x20
#define RXM0SIDL		0x21
#define RXM0EID8		0x22
#define RXM0EID0		0x23
#define RXM1SIDH		0x24
#define RXM1SIDL		0x25
#define RXM1EID8		0x26
#define RXM1EID0		0x27

//Interrupt Register Address
#define CANINTE			0x2B
#define CANINTF			0x2C

//RX-TX Pin Control and Status Register
#define BFPCTRL			0x0C
#define TXRTSCTRL		0x0D

//CAN Baudrate
#define BAUDRATE_10_KBPS	0
#define BAUDRATE_20_KBPS	1
#define BAUDRATE_50_KBPS	2
#define BAUDRATE_100_KBPS	3
#define BAUDRATE_125_KBPS	4
#define BAUDRATE_250_KBPS	5
#define BAUDRATE_500_KBPS	6
#define BAUDRATE_1_MBPS		7

//Flags
#define STANDARD_ID			0x00
#define EXTENDED_ID			0x08
#define DATA_FRAME			0x00
#define REMOTE_FRAME		0x40
#define ROLLOVER_ON			0x04
#define ROLLOVER_OFF		0x00
#define FILTER_ON			0x01
#define FILTER_OFF			0x00

#define TXREQ0				0x04
#define TXREQ1				0x10
#define TXREQ2				0x40

//Errors
#define NO_ERROR			0x00
#define TX_BUFFER_FULL		0x01

#define NO_MESSAGE			0x00
#define MESSAGE_RECEIVED	0x01


#endif /* MCP2515_HHN_H_ */
