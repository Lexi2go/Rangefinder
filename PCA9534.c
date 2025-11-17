/*
 * PCA9534.c
 *
 * Created: 30.08.2015 18:24:14
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

//****************************************************************************************
//Modul: PCA9534.c
//Datum: 13.08.2018
//Autor: Professor
//Version: 1.0
//Beschreibung: das Modul beinhaltet eine Bibliothek mit Funktionen f|r die Ansteuerung
//				eines Output Expander Bausteins vom Typ PCA9534
//Status: freigegeben
//****************************************************************************************

//****************************************************************************************
//include Dateien
//****************************************************************************************
#include "PCA9534.h"

//****************************************************************************************
//Funktion: PCA9534_Write_Register
//Datum: 13.08.2018
//Autor: Professor
//Version: 1.0
//Beschreibung:	die Funktion steuert einen Baustein vom Typ PCA9534 und dndert seine Konfiguration
//Aufgerufene Funktionen:	TWI_Master_Start, TWI_Master_Transmit, TWI_Master_Stop
//\bergabeparameter:		ucdevice_address - Adresse des Bausteins in der Schaltung (0..7)
//							ucregister - das zu beschreibende Byte (Polarity oder Configuration)
//							ucvalue - neuer Wert des Registers
//R|ckgabeparameter:		TWI_OK oder TWI_ERROR
//Revision:
//Status: freigegeben
//****************************************************************************************
uint8_t PCA9534_Write_Register(uint8_t ucdevice_address, uint8_t ucregister, uint8_t ucreg_value)
{
	uint8_t ucDeviceAddress;

	ucDeviceAddress = (ucdevice_address << 1) | PCA9534_DEVICE_TYP_ADDRESS;
	ucDeviceAddress |= TWI_WRITE;	//Write-Modus
	//Start
	TWI_Master_Start();
	if((TWI_STATUS_REGISTER) != TWI_START)	return TWI_ERROR;
	// Device Adresse senden
	TWI_Master_Transmit(ucDeviceAddress);
	if((TWI_STATUS_REGISTER) != TWI_MT_SLA_ACK)  return TWI_ERROR;
	//die Adresse des Registers wird gesendet
	TWI_Master_Transmit(ucregister);
	if((TWI_STATUS_REGISTER) != TWI_MT_DATA_ACK)  return TWI_ERROR;
	//der neue Wert des Registers wird gesendet
	TWI_Master_Transmit(ucreg_value);
	if((TWI_STATUS_REGISTER) != TWI_MT_DATA_ACK)  return TWI_ERROR;
	// Stop
	TWI_Master_Stop();
	return TWI_OK;
}


//****************************************************************************************
//Funktion: PCA9534_Write_Port
//Datum: 13.08.2018
//Autor: Professor
//Version: 1.0
//Beschreibung:	die Funktion steuert einen Baustein vom Typ PCA9534 und dndert den Zustand seiner Pins
//Aufgerufene Funktionen:	TWI_Master_Start, TWI_Master_Transmit, TWI_Master_Stop
//\bergabeparameter:		ucdevice_address - Adresse des Bausteins in der Schaltung (0..7)
//							ucport_value - neuer Wert des Ausgangsports
//R|ckgabeparameter:		TWI_OK oder TWI_ERROR
//Revision:
//Status: freigegeben
//****************************************************************************************
uint8_t PCA9534_Write_Port(uint8_t ucdevice_address, uint8_t ucport_value)
{
	uint8_t ucDeviceAddress;

	ucDeviceAddress = (ucdevice_address << 1) | PCA9534_DEVICE_TYP_ADDRESS;
	ucDeviceAddress |= TWI_WRITE;	//Write-Modus
	//Start
	TWI_Master_Start();
	if((TWI_STATUS_REGISTER) != TWI_START)	return TWI_ERROR;
	// Device Adresse senden
	TWI_Master_Transmit(ucDeviceAddress);
	if((TWI_STATUS_REGISTER) != TWI_MT_SLA_ACK)
	{
		return TWI_ERROR;
	}
	TWI_Master_Transmit(OUTPUT_REGISTER);	//
	if((TWI_STATUS_REGISTER) != TWI_MT_DATA_ACK)
	{
		return TWI_ERROR;
	}
	TWI_Master_Transmit(ucport_value);	//
	if((TWI_STATUS_REGISTER) != TWI_MT_DATA_ACK)
	{
		return TWI_ERROR;
	}
	// Stop
	TWI_Master_Stop();
	return TWI_OK;
}


//****************************************************************************************
//Funktion: PCA9534_Read_Port
//Datum: 13.08.2018
//Autor: Professor
//Version: 1.0
//Beschreibung:	die Funktion steuert einen Baustein vom Typ PCA9534 und liest den Zustand seiner Pins
//Aufgerufene Funktionen:	TWI_Master_Start, TWI_Master_Transmit, TWI_Master_Stop
//\bergabeparameter:		ucdevice_address - Adresse des Bausteins in der Schaltung (0..7)
//							ucport_value - neuer Wert des Eingangsports
//R|ckgabeparameter:		TWI_OK oder TWI_ERROR
//Revision:
//Status: freigegeben
//****************************************************************************************
uint8_t PCA9534_Read_Port(uint8_t ucdevice_address, uint8_t *ucport_value)
{
	uint8_t ucDeviceAddress;
	ucDeviceAddress = (ucdevice_address << 1) | PCA9534_DEVICE_TYP_ADDRESS;
	ucDeviceAddress |= TWI_WRITE; //Write-Modus
	//Start
	TWI_Master_Start();
	if((TWI_STATUS_REGISTER) != TWI_START) return TWI_ERROR;
	// Device Adresse senden
	TWI_Master_Transmit(ucDeviceAddress);
	if((TWI_STATUS_REGISTER) != TWI_MT_SLA_ACK) return TWI_ERROR;

	TWI_Master_Transmit(INPUT_REGISTER);	//
	if((TWI_STATUS_REGISTER) != TWI_MT_DATA_ACK)
	{
		return TWI_ERROR;
	}
	
	ucDeviceAddress = (ucdevice_address << 1) | PCA9534_DEVICE_TYP_ADDRESS;
	ucDeviceAddress |= TWI_READ; //Write-Modus
	
	//Restart
	TWI_Master_Start();
	if((TWI_STATUS_REGISTER) != TWI_RESTART) return TWI_ERROR;
	// Device Adresse senden
	TWI_Master_Transmit(ucDeviceAddress);
	if((TWI_STATUS_REGISTER) != TWI_MR_SLA_ACK) return TWI_ERROR;
	
	*ucport_value = TWI_Master_Read_NAck();
	if((TWI_STATUS_REGISTER) != TWI_MR_DATA_NACK) return TWI_ERROR;
	// Stop
	TWI_Master_Stop();
	return TWI_OK;
}


//****************************************************************************************
//Funktion: PCA9534_ReRead_Port
//Datum: 13.08.2018
//Autor: Professor
//Version: 1.0
//Beschreibung:	die Funktion steuert einen Baustein vom Typ PCA9534 und liest den Zustand seiner Pins;
//				nach dem Aufruf der Funktion PCA9534_Read_Port liefert diese Funktion 
//				den aktuellen Zustand der Eingdnge nur solange keine andere Funktion aufgerufen wird
//Aufgerufene Funktionen:	TWI_Master_Start, TWI_Master_Transmit, TWI_Master_Stop
//\bergabeparameter:		ucdevice_address - Adresse des Bausteins in der Schaltung (0..7)
//							ucport_value - neuer Wert des Eingangsports
//R|ckgabeparameter:		TWI_OK oder TWI_ERROR
//Revision:
//Status: freigegeben
//****************************************************************************************
uint8_t PCA9534_ReRead_Port(uint8_t ucdevice_address, uint8_t *ucport_value)
{
	uint8_t ucDeviceAddress;
	ucDeviceAddress = (ucdevice_address << 1) | PCA9534_DEVICE_TYP_ADDRESS;
	ucDeviceAddress |= TWI_READ; //Write-Modus
	
	//Start
	TWI_Master_Start();
	if((TWI_STATUS_REGISTER) != TWI_START) return TWI_ERROR;
	// Device Adresse senden
	TWI_Master_Transmit(ucDeviceAddress);
	if((TWI_STATUS_REGISTER) != TWI_MR_SLA_ACK) return TWI_ERROR;
	
	*ucport_value = TWI_Master_Read_NAck();
	if((TWI_STATUS_REGISTER) != TWI_MR_DATA_NACK) return TWI_ERROR;
	// Stop
	TWI_Master_Stop();
	return TWI_OK;
}


//****************************************************************************************
//Funktion: PCA9534_Read_Register
//Datum: 13.08.2018
//Autor: Professor
//Version: 1.0
//Beschreibung:	die Funktion steuert einen Baustein vom Typ PCA9534 und liest den Inhalt eines
//				Registers
//Aufgerufene Funktionen:	TWI_Master_Start, TWI_Master_Transmit, TWI_Master_Stop
//\bergabeparameter:		ucdevice_address - Adresse des Bausteins in der Schaltung (0..7)
//							ucregister - Register dessen Inhalt gelesen wird
//							ucregister_value - Inhalt des Registers
//R|ckgabeparameter:		TWI_OK oder TWI_ERROR
//Revision:
//Status: freigegeben
//****************************************************************************************
uint8_t PCA9534_Read_Register(uint8_t ucdevice_address, uint8_t ucregister, uint8_t *ucport_value)
{
	uint8_t ucDeviceAddress;
	ucDeviceAddress = (ucdevice_address << 1) | PCA9534_DEVICE_TYP_ADDRESS;
	ucDeviceAddress |= TWI_WRITE; //Write-Modus
	//Start
	TWI_Master_Start();
	if((TWI_STATUS_REGISTER) != TWI_START) return TWI_ERROR;
	// Device Adresse senden
	TWI_Master_Transmit(ucDeviceAddress);
	if((TWI_STATUS_REGISTER) != TWI_MT_SLA_ACK) return TWI_ERROR;

	TWI_Master_Transmit(ucregister);	//
	if((TWI_STATUS_REGISTER) != TWI_MT_DATA_ACK)
	{
		return TWI_ERROR;
	}
	
	ucDeviceAddress = (ucdevice_address << 1) | PCA9534_DEVICE_TYP_ADDRESS;
	ucDeviceAddress |= TWI_READ; //Write-Modus
	
	//Restart
	TWI_Master_Start();
	if((TWI_STATUS_REGISTER) != TWI_RESTART) return TWI_ERROR;
	// Device Adresse senden
	TWI_Master_Transmit(ucDeviceAddress);
	if((TWI_STATUS_REGISTER) != TWI_MR_SLA_ACK) return TWI_ERROR;
	
	*ucport_value = TWI_Master_Read_NAck();
	if((TWI_STATUS_REGISTER) != TWI_MR_DATA_NACK) return TWI_ERROR;
	// Stop
	TWI_Master_Stop();
	return TWI_OK;

}
