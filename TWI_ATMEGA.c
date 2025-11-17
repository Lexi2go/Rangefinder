/*
 * TWI_ATMEGA.c
 *
 * Created: 05.09.2014 08:58:18
 * Author: Petre Professor
 * Status: freigegeben
 * Historie: 05.09.2014		V1.0 
 */ 

//****************************************************************************************
//include Dateien
//****************************************************************************************
#include <avr/io.h>
#include <avr/interrupt.h>
#include "TWI_ATMEGA.h"


//****************************************************************************************
//unsigned char Variablen
//****************************************************************************************
unsigned char ucTWI_INT_Flag = 0;
unsigned char ucTWSR_Register, ucTWI_ErrorFlag = 0;


//****************************************************************************************
//Funktionname: TWI_Master_Init
//Datum: 05.09.2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung:				die Funktion initialisiert den �C als TWI-Master
//Aufgerufene Funktionen:	keine
//�bergabeparameter:		keine
//R�ckgabeparameter:		keine
//Revision:
//Status: freigegeben
//****************************************************************************************
void TWI_Master_Init(unsigned char uctwi_clock)
{
	TWCR = (1 << TWEA) | (1 << TWEN);
	TWBR = uctwi_clock;	//die Taktfrequenz wird in der Header-Datei ausgerechnet
}


//****************************************************************************************
//Funktionname: TWI_Master_Start
//Datum: 05.09.2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung:				der Master initiiert die TWI-Kommunikation
//Aufgerufene Funktionen:	keine
//�bergabeparameter:		keine
//R�ckgabeparameter:		keine
//Revision:
//Status: freigegeben
//------------------------------------------------------------------------
void TWI_Master_Start(void)
{
	TWCR = (1 << TWINT) | (1 << TWSTA) | (1 << TWEN);
	//Start SDA-Leitung auf Low-setzen
	while(!(TWCR & (1 << TWINT)));
	//warten bis SDA gesetzt wird
}


//****************************************************************************************
//Funktionname: TWI_Master_Stop
//Datum: 05.09.2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung:				die Funktion beendet eine TWI-Kommunikation
//Aufgerufene Funktionen:	keine
//�bergabeparameter:		keine
//R�ckgabeparameter:		keine
//Revision:
//Status: freigegeben
//****************************************************************************************
void TWI_Master_Stop(void)
{
	TWCR = (1 << TWINT) | (1 << TWEN) | (1 << TWSTO);
}


//****************************************************************************************
//Funktionname: TWI_Master_Transmit
//Datum: 05.09.2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung:
//Aufgerufene Funktionen:	keine
//�bergabeparameter:		ucdata - das zu �bertragende Byte
//R�ckgabeparameter:		keine
//Revision:
//Status: freigegeben
//****************************************************************************************
void TWI_Master_Transmit(unsigned char ucdata)
{
	TWDR = ucdata;
	TWCR = (1 << TWINT) | (1 << TWEN);

	while(!(TWCR & (1 << TWINT)));
	//die Hardware setzt das Bit TWINT auf 1 wenn ACK oder NACK
	//empfangen wurde
}


//****************************************************************************************
//Funktionname: TWI_Master_Read_Ack
//Datum: 05.09.2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung:				Master liest ein Byte und sendet Acknowledge an den Slave
//							um ein weiteres Byte empfangen zu k�nnen
//Aufgerufene Funktionen:	keine
//�bergabeparameter:		keine
//R�ckgabeparameter:		das empfangene Byte
//Revision:
//Status: freigegeben
//****************************************************************************************
unsigned char TWI_Master_Read_Ack(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN)|(1<<TWEA);
	while(!(TWCR & (1 << TWINT)));
	return TWDR;
}


//****************************************************************************************
//Funktionname: TWI_Master_Read_NAck
//Datum: 05.09.2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung:				Master liest ein letztes Byte und sendet NAcknowledge an den Slave
//Aufgerufene Funktionen:	keine
//�bergabeparameter:		keine 
//R�ckgabeparameter:		das empfangene Byte
//Revision:
//****************************************************************************************
unsigned char TWI_Master_Read_NAck(void)
{
	TWCR = (1<<TWINT)|(1<<TWEN);
	while(!(TWCR & (1 << TWINT)));
	return TWDR;
}

/*
//------------------------------------------------------------------------
//Funktionname: TWI_INT_Master_Init
//Datum: 19.12.2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung: die Funktion initialisiert den �C als TWI-Master und gibt den TWI-Interrupt frei
//Aufgerufene Funktionen:keine
//�bergabeparameter: keine
//R�ckgabeparameter: keine
//Revision:
//Status: freigegeben
//------------------------------------------------------------------------
void TWI_INT_Master_Init(unsigned char uctwi_clock)
{
	TWCR = (1 << TWEA) | (1 << TWEN) | (1 << TWIE);
	TWBR = uctwi_clock;	//die Taktfrequenz wird in der Header-Datei ausgerechnet
}*/


void TWI_INT_Enable(void)
{
	TWCR |= 1 << TWIE;
}


void TWI_INT_Disable(void)
{
	TWCR &= ~(1 << TWIE);
}


//------------------------------------------------------------------------
//Funktionname: TWI_INT_Master_Transmit
//Datum: 19.12.2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung:
//Aufgerufene Funktionen: keine
//�bergabeparameter: data - das zu �bertragende Byte
//R�ckgabeparameter: keine
//Revision:
//Status: 
//------------------------------------------------------------------------
void TWI_INT_Master_Transmit(unsigned char data)
{
	TWDR = data;
	TWCR = (1 << TWINT) | (1 << TWEN);
	
	//die Hardware setzt das Bit TWINT auf 1 wenn ACK oder NACK
	//empfangen wurde
}


//------------------------------------------------------------------------
//Funktionname: TWI_Get_State
//Datum: 19.12.2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung:
//Aufgerufene Funktionen: keine
//�bergabeparameter: keine
//R�ckgabeparameter:	TWI_TRIGGERED wenn Interrupt ausgel�st oder
//						TWI_RUNNING
//Revision:
//Status:
//------------------------------------------------------------------------
unsigned char TWI_Get_State(void)
{
	if(ucTWI_INT_Flag)
	{
		ucTWI_INT_Flag = 0;
		return TWI_TRIGGERED;
	}
	else return TWI_RUNNING;
}


//------------------------------------------------------------------------
//Funktionname: TWI_Get_TWSRRegister
//Datum: 19.12.2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung:
//Aufgerufene Funktionen: keine
//�bergabeparameter: keine
//R�ckgabeparameter:	ucTWSR_Register
//Revision:
//Status:
//------------------------------------------------------------------------
unsigned char TWI_Get_TWSRRegister(void)
{
	return ucTWSR_Register;
}


void TWI_Set_ErrorFlag(void)
{
	ucTWI_ErrorFlag = 1;
}
//----------------------------------------------------------------------------------------
//Funktion: Interrupt Service Routine von TWI
//Datum: 19.12. 2014
//Autor: Petre Professor
//Version: 1.0
//Beschreibung:
//Aufgerufene Funktionen:
//Status:
//----------------------------------------------------------------------------------------
ISR(TWI_vect)
{
	ucTWI_INT_Flag = 1;
	ucTWSR_Register = TWI_STATUS_REGISTER;
	
	TWCR |= 1 << TWINT;
}