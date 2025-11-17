/*
 * TWI_ATMEGA.h
 *
 * Created: 05.09.2014 08:59:08
 *  Author: Petre Professor
  * Status: freigegeben
  * Historie: 05.09.2014		V1.0
 */ 


#ifndef TWI_ATMEGA_H_
#define TWI_ATMEGA_H_

//****************************************************************************************
//TWI define's
//****************************************************************************************

#define TWI_STATUS_REGISTER (TWSR & 0xF8)

#define TWI_RUNNING			0x00
#define TWI_TRIGGERED		0x01

#define TWI_START			0x08
#define TWI_RESTART			0x10
#define TWI_MR_SLA_ACK		0x40
#define TWI_MR_DATA_ACK		0x50
#define TWI_MR_DATA_NACK	0x58

#define TWI_MT_SLA_ACK		0x18
#define TWI_MT_DATA_ACK		0x28
#define TWI_MT_DATA_NACK	0x30

#define TWI_ACK				1
#define TWI_NACK			0

#define TWI_ERROR			0x01
#define TWI_OK				0x00

#define TWI_WRITE			0x00
#define TWI_READ			0x01


//****************************************************************************************
//Deklaration der Funktionen
//****************************************************************************************
void TWI_Master_Init(unsigned char uctwi_clock);
void TWI_Master_Start(void);
void TWI_Master_Stop(void);
void TWI_Master_Transmit(unsigned char ucdata);
unsigned char TWI_Master_Read_Ack(void);
unsigned char TWI_Master_Read_NAck(void);

void TWI_INT_Master_Transmit(unsigned char ucdata);

unsigned char TWI_Get_State(void);
unsigned char TWI_Get_TWSRRegister(void);
void TWI_INT_Enable(void);
void TWI_INT_Disable(void);
void TWI_Set_ErrorFlag(void);

#endif /* TWI_ATMEGA_H_ */