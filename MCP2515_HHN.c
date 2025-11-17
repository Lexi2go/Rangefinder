/*
 * MCP2515_HHN.c
 *
 * Created: 07.03.2017 10:27:09
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
//Modul: MCP2515.c
//Datum: 07.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung: das Modul beinhaltet eine Bibliothek mit Funktionen für die Ansteuerung
//				des CAN-Controllers MCP2515
//Status: freigegeben
//****************************************************************************************

#include "MCP2515_HHN.h"

//die Baudrate gilt für einen Quarz mit 16 MHz; für andere Frequenzen müssen die Werte neu gerechnet werden
uint8_t ucBaudrate[8][3] = //CNF3, CNF2, CNF1
/*  10KBPS*/				{{0x05, 0xB1, 0x31}, //wake-up off, PS2=5+1 /BTLMODE, PS1=6+1, PRSEG=1+1 /SJW=0+1, BRP=49+1 (16*TQ)
/*  20KBPS*/				{0x05, 0xB1, 0x18}, //wake-up off, PS2=5+1 /BTLMODE, PS1=6+1, PRSEG=1+1 /SJW=0+1, BRP=24+1 (16*TQ)
/*  50KBPS*/				{0x05, 0xB1, 0x09}, //wake-up off, PS2=5+1 /BTLMODE, PS1=6+1, PRSEG=1+1 /SJW=0+1, BRP=9+1 (16*TQ)
/* 100KBPS*/				{0x05, 0xB1, 0x04}, //wake-up off, PS2=5+1 /BTLMODE, PS1=6+1, PRSEG=1+1 /SJW=0+1, BRP=4+1 (16*TQ)
/* 125KBPS*/				{0x05, 0xB1, 0x03}, //wake-up off, PS2=5+1 /BTLMODE, PS1=6+1, PRSEG=1+1 /SJW=0+1, BRP=3+1 (16*TQ)
/* 250KBPS*/				{0x05, 0xB1, 0x01}, //wake-up off, PS2=5+1 /BTLMODE, PS1=6+1, PRSEG=1+1 /SJW=0+1, BRP=1+1 (16*TQ)
/* 500KBPS*/				{0x05, 0xB1, 0x00}, //wake-up off, PS2=5+1 /BTLMODE, PS1=6+1, PRSEG=1+1 /SJW=0+1, BRP=0+1 (16*TQ)
/*1000KBPS*/				{0x02, 0x89, 0x00}};//wake-up off, PS2=2+1 /BTLMODE, PS1=1+1, PRSEG=1+1 /SJW=0+1, BRP=0+1 (8*TQ)
uint8_t ucDataBuffer[13], ucRxDataBuffer[13];
uint8_t ucFilterAddress[6] = {RXF0SIDH, RXF1SIDH, RXF2SIDH, RXF3SIDH, RXF4SIDH, RXF5SIDH};
uint8_t ucMaskAddress[2] = {RXM0SIDH, RXM1SIDH};

	
//****************************************************************************************
//Funktion: MCP2515_Init
//Datum: 07.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	Die Funktion initialisiert den µC für die Ansteuerung eines CAN-Controllers  
//				MCP2515; der Mikrocontrolleranschlus für die Ansteuerung des Slave Select-Pins
//				wird als Ausgang deklariert und auf High gesetzt; die gewünschte Baudrate wird
//				eingestellt und der Clock-out Pin deaktiviert
//				
//Aufgerufene Funktionen: 	SPI_Master_SlaveSelectInit, SPI_Master_Init, MCP2515_Set_OpMode,
//							MCP2515_Set_Baudrate, MCP2515_Change_ClkOut			
//Übergabeparameter:		sdevice_pins: ist eine Struktur mit den Anschlüssen eines CAN-Controllers;
//							für jeden SPI-Bus-Teilnehmer muss eine solche Struktur definiert werden. 
//								1. Element - die Adresse des DDR-Registers an dem die Slave Select Leitung angeschlossen ist
//								2. Element - die Adresse des PORT-Registers an dem die Slave Select Leitung angeschlossen ist
//								3. Element - das entsprechende Bit der Slave Select Leitung
//								4. Element - angesteuerte Leitung ON (OFF fest angeschlossen)
//							in der main-Datei wird für jeden angeschlossenen Speicher eine Struktur  vom Datentyp
//							MCP2515_pins deklariert und entsprechend der Schaltung initialisiert wie z.B.:*/
//							MCP2515_pins  MCP2515_1 = {{/*CS_DDR*/	&DDRB,
//														/*CS_PORT*/		&PORTB,
//														/*CS_pin*/		PB2,
//														/*CS_state*/	ON}};
//							ucbaud: Baudrate in codierter Form
//Rückgabeparameter:		keine
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Init(MCP2515_pins sdevice_pins, uint8_t ucbaud)
{
	//die Funktion SPI_Master_SlaveSelect_Init wird aufgerufen
	//der Slave Select Anschluss des Bausteins wird initialisiert (Ausgang auf High gesetzt)
	SPI_Master_SlaveSelectInit(sdevice_pins.MCP2515spi);
	//SPI Schnittstelle des µC wird konfiguriert
	SPI_Master_Init(SPI_INTERRUPT_DISABLE, SPI_MSB_FIRST, SPI_MODE_3, SPI_FOSC_DIV_4);
	//CAN-Controller wird in den Config-Modus versetzt
	MCP2515_Set_OpMode(sdevice_pins, CONFIG_MODE);
	//die Baudrate wird eingestellt
	MCP2515_Set_Baudrate(sdevice_pins, ucbaud);
	//der Clock-Ausgang des CAN-Controllers wird deaktiviert
	MCP2515_Change_ClkOut(sdevice_pins, CLOCK_OUT_DISABLE);
	//der Normal-Modus wird eingeschaltet
	MCP2515_Set_OpMode(sdevice_pins, NORMAL_OP_MODE);
}


//****************************************************************************************
//Funktion: MCP2515_Read_Reg
//Datum: 07.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion liest aus dem adressierten Baustein angefangen mit der Registeradresse
//				ucreg_address ucreg_number Werte und speichert sie in den Vektor ucreg_in.
//Aufgerufene Funktionen:	SPI_Master_Start, SPI_Master_Write, SPI_Master_Stop
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							ucreg_address:	Anfangsadresse
//							ucreg_number: Anzahl der zu lesenden Register
//							ucreg_in: Datenvektor in den die Inhalte der Register gespeichert werden
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Read_Reg(MCP2515_pins sdevice_pins, uint8_t ucreg_address, uint8_t ucreg_number, uint8_t *ucreg_in)
{
	SPI_Master_Start(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird gestartet
	SPI_Master_Write(READ_REG_CODE); //das Command-Byte wird übertragen
	SPI_Master_Write(ucreg_address); //die Adresse des ersten Registers wird übertragen
	for(uint8_t ucI = 0; ucI < ucreg_number; ucI++)
	{
		ucreg_in[ucI] = SPI_Master_Write(0xFF); //der Inhalt des Registers wird gelesen
	}
	SPI_Master_Stop(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird beendet
}


//****************************************************************************************
//Funktion: MCP2515_Write_Reg
//Datum: 07.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion speichert aus dem Vektor ucreg_out in einen Registerbereich
//				der mit der Adresse ucreg_address anfängt ucreg_number Werte.
//Aufgerufene Funktionen:	SPI_Master_Start, SPI_Master_Write, SPI_Master_Stop
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							ucreg_address:	Anfangsadresse
//							ucreg_number: Anzahl der zu schreibenden Register
//							ucreg_out: Datenvektor für die Register
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Write_Reg(MCP2515_pins sdevice_pins, uint8_t ucreg_address, uint8_t ucreg_number, uint8_t *ucreg_out)
{
	SPI_Master_Start(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird gestartet
	SPI_Master_Write(WRITE_REG_CODE); //das Command-Byte wird übertragen
	SPI_Master_Write(ucreg_address); //die Adresse des ersten registers wird übertragen
	for(uint8_t ucI = 0; ucI < ucreg_number; ucI++)
	{
		SPI_Master_Write(ucreg_out[ucI]); //der Inhalt des Registers wird geändert
	}
	SPI_Master_Stop(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird beendet
}


//****************************************************************************************
//Funktion: MCP2515_Change_Reg
//Datum: 07.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion ändert den Inhalt des adressierten Registers abhängig von der
//				gewählten Maske. Mit dieser Funktion können die Inhalte der Control Register
//				geändert werden (ohne CANSTAT, TEC und REC)
//Aufgerufene Funktionen:	SPI_Master_Start, SPI_Master_Write, SPI_Master_Stop
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							ucreg_address:	Registeradresse
//							ucreg_mask: im adressierten Register kann ein Bit geändert werden
//							nur wenn das entsprechende Bit in der Maske gesetzt ist (1)
//							ucreg_data: neuer Inhalt des Registers (Achtung! Rücksicht auf die Maske)
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Change_Reg(MCP2515_pins sdevice_pins, uint8_t ucreg_address, uint8_t ucreg_mask, uint8_t ucreg_data)
{
	SPI_Master_Start(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird gestartet
	SPI_Master_Write(BIT_MODIFY_CODE); //das Command-Byte wird übertragen
	SPI_Master_Write(ucreg_address); //die Adresse des Registers wird übertragen
	SPI_Master_Write(ucreg_mask); //die Maske wird übertragen
	SPI_Master_Write(ucreg_data); //der neue Inhalt des registers wird übertragen
	SPI_Master_Stop(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird beendet
}


//****************************************************************************************
//Funktion: MCP2515_Set_OpMode
//Datum: 07.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion ändert den Betriebsmodus des Controllers
//Aufgerufene Funktionen:	MCP2515_Change_Reg
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							ucop_mode: der neue Betriebsmodus (NORMAL_OP_MODE, SLEEP_MODE,
//							LOOPBACK_MODE, LISTEN_ONLY_MODE, oder CONFIG_MODE)
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Set_OpMode(MCP2515_pins sdevice_pins, uint8_t ucop_mode)
{
	//der gewünschte Modus wird gewählt
	MCP2515_Change_Reg(sdevice_pins, CANCTRL, OP_MODE_MASK, ucop_mode);
}


//****************************************************************************************
//Funktion: MCP2515_Change_ClkOut
//Datum: 07.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion steuert den Clockout Pin des Controllers
//Aufgerufene Funktionen:	MCP2515_Change_Reg
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							ucop_clk_out: CLOCK_OUT_DISABLE, ENABLE_FOUT_1TO1, ENABLE_FOUT_1TO2,
//							ENABLE_FOUT_1TO4, ENABLE_FOUT_1TO8
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Change_ClkOut(MCP2515_pins sdevice_pins, uint8_t ucclk_out)
{
	MCP2515_Change_Reg(sdevice_pins, CANCTRL, CLOCK_OUT_MASK, ucclk_out);
}


//****************************************************************************************
//Funktion: MCP2515_OneShotMode
//Datum: 07.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion schaltet den Einzelmodus um
//Aufgerufene Funktionen:	MCP2515_Change_Reg
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							ucop_clk_out: ONE_SHOT_ENABLE, ONE_SHOT_DISABLE
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_OneShotMode(MCP2515_pins sdevice_pins, uint8_t ucone_shot)
{
	MCP2515_Change_Reg(sdevice_pins, CANCTRL, ONE_SHOT_MASK, ucone_shot);
}


//****************************************************************************************
//Funktion: MCP2515_Set_Baudrate
//Datum: 08.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion stellt die gewünschte Bitrate ein
//Aufgerufene Funktionen:	MCP2515_Write_Reg
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							ucbaud: Zeiger auf ein Element der Datenmatrix ucBaudrate
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Set_Baudrate(MCP2515_pins sdevice_pins, uint8_t ucbaud)
{
	MCP2515_Write_Reg(sdevice_pins, CNF3, 3, ucBaudrate[ucbaud]);
}


//****************************************************************************************
//Funktion: MCP2515_Read_Status
//Datum: 08.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion liest relevante Status-Flags
//Aufgerufene Funktionen:	SPI_Master_Start, SPI_Master_Write, SPI_Master_Stop
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//Rückgabeparameter:		ein Byte mit den Flags: D7-TX2IF, D6-TXREQ2, D5-TX1IF, D4-TXREQ1
//							D3-TX0IF, D2-TXREQ0, D1-RX1IF, D0-RX0IF
//Revision:
//Status: freigegeben
//****************************************************************************************
uint8_t MCP2515_Read_Status(MCP2515_pins sdevice_pins)
{
	uint8_t ucDummy = 0xFF, ucStatusByte;
	SPI_Master_Start(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird gestartet
	SPI_Master_Write(READ_STATUS_CODE); //das Command-Byte wird übertragen
	ucStatusByte = SPI_Master_Write(ucDummy); //das Statusbyte wird gelesen
	SPI_Master_Write(ucDummy); //dieses Byte ist eine Kopie des Vorigen
	SPI_Master_Stop(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird beendet
	return ucStatusByte;
}


//****************************************************************************************
//Funktion: MCP2515_Load_TxBuffer
//Datum: 09.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	lädt die Senderegister und startet die Übertragung der Nachricht
//Aufgerufene Funktionen:	SPI_Master_Start, SPI_Master_Write, SPI_Master_Stop
//Übergabeparameter:		uidevice_pins:	siehe MCP2515_Init-Funktion
//							ucbuffer_start: codierte Anfangsadrese des Registerbereiches
//							ucbuffer_length: Anzahl der Register
//							ucreg_out: Adresse eines Datenvektors der die Inhalte der 
//										Register speichert
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Load_TXBuffer(MCP2515_pins sdevice_pins, uint8_t ucbuffer_start, uint8_t ucbuffer_length, uint8_t *ucreg_out)
{
	uint8_t ucOp_Code;
	ucOp_Code = LOAD_TX_BUFFER_CODE | ucbuffer_start; //die Anfangsadresse wird gesetzt
	SPI_Master_Start(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird gestartet
	SPI_Master_Write(ucOp_Code); //das Command-Byte wird übertragen
	for(uint8_t ucI = 0; ucI < ucbuffer_length; ucI++)
	{
		SPI_Master_Write(ucreg_out[ucI]);
	}
	SPI_Master_Stop(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird beendet
}


//****************************************************************************************
//Funktion: MCP2515_Send_Message
//Datum: 08.03.2017
//Autor:Professor
//Version: 1.1
//Beschreibung:	lädt die Senderegister und startet die Übertragung der Nachricht
//Aufgerufene Funktionen:	SPI_Master_Start, SPI_Master_Write, SPI_Master_Stop,
//							MCP2515_Read_Status, MCP2515_Load_TXBuffer, MCP2515_RequestToSend
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							sframe: die Adresse einer Datenstruktur mit der ID, Länge und Daten
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
uint8_t MCP2515_Send_Message(MCP2515_pins sdevice_pins, can_frame *sframe)
{
	uint8_t ucBuffer = 0, ucLength, ucTxBuffer;
	uint8_t ucTxBufferStart[3] = {0, 2, 4};
	uint32_t ulData_ID;
	
	ucBuffer = MCP2515_Read_Status(sdevice_pins);
	if(!(ucBuffer & TXREQ0)) ucTxBuffer = 0; //es wird geprüft welcher Sendepuffer frei ist
	else if(!(ucBuffer & TXREQ1)) ucTxBuffer = 1;
	else if(!(ucBuffer & TXREQ2)) ucTxBuffer = 2;
	else return TX_BUFFER_FULL; //kein freier Sendepuffer
	
	for(uint8_t ucI = 0; ucI < 13; ucI++) ucDataBuffer[ucI] = 0; //der Datenpuffer wird gelöscht
	if(!(sframe->EIDE_Bit))
	{	//Standard ID wird zusammengesetzt
		ucDataBuffer[1] = (uint8_t) (sframe->ulID & 0x07) << 5; //zu testen
		ucDataBuffer[0] = (uint8_t) (sframe->ulID >> 3);
	}
	else
	{	//Extended ID wird zusammengesetzt
		ulData_ID = sframe->ulID;
		ucDataBuffer[3] = ulData_ID;
		ulData_ID = ulData_ID >> 8;
		ucDataBuffer[2] = ulData_ID;
		ulData_ID = ulData_ID >> 8;
		ucDataBuffer[1] = ((uint8_t) (ulData_ID & 0x03)) | (((uint8_t) ulData_ID & 0x1C) << 3) | sframe->EIDE_Bit;
		ucDataBuffer[0] = (uint8_t) (ulData_ID >> 5);
	}
	ucLength = sframe->ucLength; 
	ucDataBuffer[4] = ucLength | sframe->RTR_Bit;
	for(uint8_t ucI = 0; ucI < ucLength; ucI++) ucDataBuffer[ucI + 5] = sframe->ucData[ucI];
	ucLength = ucLength + 5;
	//der Datenpuffer wird in den freien Sendepuffer transferiert
	MCP2515_Load_TXBuffer(sdevice_pins, ucTxBufferStart[ucTxBuffer], ucLength, ucDataBuffer);
	ucDataBuffer[0] = 0;
	MCP2515_Write_Reg(sdevice_pins, CANINTE, 1, ucDataBuffer); //alle CAN-Interrupts disabled
	MCP2515_RequestToSend(sdevice_pins, ucTxBuffer); //die Übertragung aus dem Sendepuffer wird initiert
	return NO_ERROR; //das Senden der Nachricht ist beendet
}


//****************************************************************************************
//Funktion: MCP2515_RequestToSend
//Datum: 09.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	lädt die Senderegister und startet die Übertragung der Nachricht
//Aufgerufene Funktionen:	SPI_Master_Start, SPI_Master_Write, SPI_Master_Stop
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							uctx_buffer: Sendepuffer
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_RequestToSend(MCP2515_pins sdevice_pins, uint8_t uctx_buffer)
{
	uint8_t ucOpCode;
	ucOpCode = RTS_CODE | (1 << uctx_buffer);
	SPI_Master_Start(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird gestartet
	SPI_Master_Write(ucOpCode); //das Command-Byte wird übertragen
	SPI_Master_Stop(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird beendet
}


//****************************************************************************************
//Funktion: MCP2515_Set_Filter_Mask
//Datum: 11.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion speichert die kodierten Filter und Masken für den Empfang
//Aufgerufene Funktionen:	MCP2515_Read_Reg, MCP2515_Set_OpMode, MCP2515_Change_Reg
//							MCP2515_Build_Filter_Frame, MCP2515_Write_Reg
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							sfilter: die Adresse einer Datenstruktur mit den Empfangsfilter und -masken
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Set_Filter_Mask(MCP2515_pins sdevice_pins, can_filter *sfilter)
{
	uint32_t ulFilter_ID;
	uint8_t ucFrameTyp, ucOpMode, ucRestoreOpMode = 0;
	
	MCP2515_Read_Reg(sdevice_pins, CANCTRL, 1, &ucOpMode); //Arbeitsmodus des CAN-Controllers wird ermittelt
	if((ucOpMode & 0xE0) != 0x80)
	{ //der Controller befindet sich nicht im Konfigurationsmodus
		ucRestoreOpMode = 1; //merken dass der Arbeitsmodus geändert wurde
		MCP2515_Set_OpMode(sdevice_pins, CONFIG_MODE);
	}
	
	if(sfilter->Rec_Buff0_Rollover == ROLLOVER_ON)
	{ //wenn der Empfangspuffer voll ist, wird die ankommende Nachricht in den 2. Empfangspuffer weitergeleitet
		MCP2515_Change_Reg(sdevice_pins, RXB0CTRL, ROLLOVER_ON, ROLLOVER_ON);
	}
	//Zusammensetzen und speichern der 6 Empfangsfilter
	for(uint8_t ucI = 0; ucI < 6; ucI++)
	{
		ulFilter_ID = sfilter->ulRecBuff_Filter[ucI];
		if(ucI < 2) ucFrameTyp = sfilter->RecBuff_ID[0];
		else ucFrameTyp = sfilter->RecBuff_ID[1];
		MCP2515_Build_Filter_Frame(ulFilter_ID, ucFrameTyp);
		ucDataBuffer[1] = ucDataBuffer[1] | ucFrameTyp;
		MCP2515_Write_Reg(sdevice_pins, ucFilterAddress[ucI], 4, ucDataBuffer);
	}
	//Zusammensetzen und speichern der zwei Filtermasken
	for(uint8_t ucI = 0; ucI < 2; ucI++)
	{
		if(sfilter->Filter_RecBuff[ucI] == FILTER_OFF)
		{
			//wenn die Filterung abgeschaltet ist, werden alle Bits der Filterregister auf 0 gesetzt
			for(uint8_t ucJ = 0; ucJ < 4; ucJ++)
			ucDataBuffer[ucJ] = 0;
		}
		else
		{
			ulFilter_ID = sfilter->ulRecBuff_Mask[ucI];
			ucFrameTyp = sfilter->RecBuff_ID[ucI];
			MCP2515_Build_Filter_Frame(ulFilter_ID, ucFrameTyp);
		}
		MCP2515_Write_Reg(sdevice_pins, ucMaskAddress[ucI], 4, ucDataBuffer);
	}
	if(ucRestoreOpMode == 1)
	{//der ursprüngliche Konfigurationsmodus wird wiederhergestellt
		MCP2515_Write_Reg(sdevice_pins, CANCTRL, 1, &ucOpMode);
	}
}


//****************************************************************************************
//Funktion: MCP2515_Build_Filter_Frame
//Datum: 11.03.2017
//Autor: Professor
//Version: 1.0
//Beschreibung:	die Funktion bereitet die Filter- und Maskenrahmen vor
//Aufgerufene Funktionen:	keine
//Übergabeparameter:		ulfilter_id: Identifikationsnummer
//							ucframe_typ: Standard oder Extended Frame
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Build_Filter_Frame(uint32_t ulfilter_id, uint8_t ucframe_typ)
{
	uint32_t ulFilter;
	ulFilter = ulfilter_id;
	if(ucframe_typ == STANDARD_ID)
	{	//Standard ID
		ucDataBuffer[1] = (uint8_t) (ulFilter & 0x03) << 5;
		ucDataBuffer[0] = (uint8_t) (ulFilter >> 3);
		ucDataBuffer[2] = 0;
		ucDataBuffer[3] = 0;
	}
	else
	{	//Extended ID
		ucDataBuffer[3] = ulFilter;
		ulFilter = ulFilter >> 8;
		ucDataBuffer[2] = ulFilter;
		ulFilter = ulFilter >> 8;
		ucDataBuffer[1] = ((uint8_t) (ulFilter & 0x03)) | (((uint8_t) ulFilter & 0x1C) << 3);
		ucDataBuffer[0] = (uint8_t) (ulFilter >> 5);
	}
}


//****************************************************************************************
//Funktion: MCP2515_Read_RxStatus
//Datum: 11.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion liest relevante Status-Flags des Empfängers
//Aufgerufene Funktionen:	SPI_Master_Start, SPI_Master_Write, SPI_Master_Stop
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//Rückgabeparameter:		D7-D6 Received Message (Message in RXB0/RXB1)
//							D4-D3 Msg Type Received (Standard/Extended, Data/Remote)
//							D2-D0 Filter Match
//Revision:
//Status: freigegeben
//****************************************************************************************
uint8_t MCP2515_Read_RxStatus(MCP2515_pins sdevice_pins)
{
	uint8_t ucDummy = 0xFF, ucStatusByte;
	SPI_Master_Start(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird gestartet
	SPI_Master_Write(READ_RX_STATUS_CODE); //das Command-Byte wird übertragen
	ucStatusByte = SPI_Master_Write(ucDummy); //das Statusbyte wird gelesen
	SPI_Master_Write(ucDummy);
	SPI_Master_Stop(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird beendet
	return ucStatusByte;
}


//****************************************************************************************
//Funktion: MCP2515_Check_Message
//Datum: 11.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion prüft ob eine CAN-Nachricht empfangen wurde und wenn ja, liest, dekodiert
//				und speichert sie in eine Datenstruktur vom Typ can_frame
//Aufgerufene Funktionen:	MCP2515_Read_RxStatus, MCP2515_Read_RxBuffer
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							sframe: Adresse einer Struktur in die die empfangene Nachricht gespeichert wird
//Rückgabeparameter:		Status des Empfängers
//Revision:
//Status: freigegeben
//****************************************************************************************
uint8_t MCP2515_Check_Message(MCP2515_pins sdevice_pins, can_frame *sframe)
{
	uint8_t ucLength, ucRxBuffer, ucRxStatus;
	uint8_t ucRxBufferStart[4] = {0, 2, 4, 6};
	uint32_t ulData_ID = 0;
	
	ucRxStatus = MCP2515_Read_RxStatus(sdevice_pins);
	if(!(ucRxStatus & 0xC0))
	{
		//es wurde keine Nachricht empfangen die dem gesetzten Filter entspricht
		return NO_MESSAGE;
	}
	else
	{
		if(ucRxStatus & 0x40) ucRxBuffer = 0x00;
		else if(ucRxStatus & 0x80) ucRxBuffer = 0x02;
		MCP2515_Read_RxBuffer(sdevice_pins, ucRxBufferStart[ucRxBuffer], 13, ucRxDataBuffer);
		ucLength = ucRxDataBuffer[4] & 0x0F;
		sframe->ucLength = ucLength;

		for(uint8_t ucI = 0; ucI < ucLength; ucI++)
		{
			sframe->ucData[ucI] = ucRxDataBuffer[ucI +5];
		}
		ulData_ID = ucRxDataBuffer[0];
		ulData_ID = ulData_ID << 3;
		ulData_ID = ulData_ID | (ucRxDataBuffer[1] >> 5);
		if(!(ucRxDataBuffer[1] & 0x08)) //wenn das IDE Bit nicht gesetzt ist, Standard Frame
		{
			sframe->EIDE_Bit = STANDARD_ID;
			if(ucRxDataBuffer[1] & 0x10) sframe->RTR_Bit = REMOTE_FRAME;
			sframe->ulID = ulData_ID;
		}
		else //Extended Frame
		{
			sframe->EIDE_Bit = EXTENDED_ID;
			if(ucRxDataBuffer[4] & 0x40) sframe->RTR_Bit = REMOTE_FRAME;
			ulData_ID = ulData_ID << 2;
			ulData_ID = ulData_ID | (ucRxDataBuffer[1] & 0x03);
			ulData_ID = (ulData_ID << 8) | ucRxDataBuffer[2];
			ulData_ID = (ulData_ID << 8) | ucRxDataBuffer[3];
			sframe->ulID = ulData_ID;
		}
	}
	return MESSAGE_RECEIVED; //eine gültige Nachricht wurde empfangen
}


//****************************************************************************************
//Funktion: MCP2515_Read_RxBuffer
//Datum: 14.03.2017
//Autor: Professor
//Version: 1.1
//Beschreibung:	die Funktion liest die Empfangsregister
//Aufgerufene Funktionen:	SPI_Master_Start, SPI_Master_Write, SPI_Master_Stop
//Übergabeparameter:		sdevice_pins:	siehe MCP2515_Init-Funktion
//							ucbuffer_start: codierte Anfangsadrese des Registerbereiches
//							ucbuffer_length: Anzahl der Register
//							ucreg_in: Adresse eines Datenvektors der die Inhalte der
//										Register speichert
//Rückgabeparameter:		kein
//Revision:
//Status: freigegeben
//****************************************************************************************
void MCP2515_Read_RxBuffer(MCP2515_pins sdevice_pins, uint8_t ucbuffer_start, uint8_t ucbuffer_length, uint8_t *ucreg_in)
{
	uint8_t ucOp_Code;
	ucOp_Code = READ_RX_BUFFER_CODE | ucbuffer_start;
	SPI_Master_Start(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird gestartet
	SPI_Master_Write(ucOp_Code); //das Command-Byte wird übertragen
	for(uint8_t ucI = 0; ucI < ucbuffer_length; ucI++)
	{
		ucreg_in[ucI] = SPI_Master_Write(0xFF);
	}
	SPI_Master_Stop(sdevice_pins.MCP2515spi); //die SPI-Übertragung wird beendet
}


