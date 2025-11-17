/*
 * 2023Entfernungsmesser.c
 *
 * Created: 09.01.2023 14:17:11
 * Author : Students
 */ 

#include <avr/io.h>
#include "MCP2515_HHN.h"
#include "PCA9534.h"
#include "Timer0.h"
#include "Timer1.h"
#include "TWI_ATMEGA.h"
#include "display_funktionen.h"

#define F_CPU 18432000UL																		//Clock des Board in Hz
#define TWI_SCL_FREQ 400000UL																	//gew�nschte TWI_Taktfrequenz
#define TWI_MASTER_CLOCK (((F_CPU/TWI_SCL_FREQ)-16)/2 +1)										//Wert des TWBR_Registers

#define OFF		0
#define ON		1

unsigned char ucMessungFlag = OFF;																//Variable zum Ausl�sen des Messtriggers
unsigned short usAbstand = 0;																	//Variable zum Speichern des gemessenen Abstands
unsigned short usSW = 3;																		//Variable zum Speichern des empfangenen Schwellenwerts
can_frame cfreceiveFrame;																		//Variable zum Speichern des per CAN empfangenen Datenpakets
can_frame cfsendFrame;																			//Variable zum Speichern des zu sendenden CAN-Frames

MCP2515_pins MCP2515_1 = {{&DDRB, &PORTB, PB2, ON}};											//Konfiguration der PINs zur Verwendung f�r CAN

void Lichtstreifen_ansteuern(unsigned short usSchwellenwert, unsigned short usd);
void Init();

int main(void)
{
	Init();																						//Initialisierung
	Display_Clear();																			//Leeren des Displays
	
    while (1) 
    {
		//CAN-Botschaft empfangen
		if(MCP2515_Check_Message(MCP2515_1, &cfreceiveFrame) == MESSAGE_RECEIVED)				//�berpr�fen, ob ein CAN-Frame empfangen wurde
		{
			if (cfreceiveFrame.ulID == 0x301)													//Adresse, auf die reagiert werden soll
			{
				usSW = ((cfreceiveFrame.ucData[1]<<8) | cfreceiveFrame.ucData[0]);				//Schwellenwert aus dem empfangenen Frame berechnen und abspeichern
			}
		}
		
		if(Timer0_get_1sState()== TIMER_TRIGGERED)												//Jede Sekunde
		{
			ucMessungFlag = ON;																	//Messtrigger starten
			
			usAbstand =  uszaehler/4;															//gemessener Abstand in cm umrechnen
			Lichtstreifen_ansteuern(usSW,usAbstand);											//Abstand durch LEDs veranschaulichen
			
			//CAN Botschaft senden
			cfsendFrame.EIDE_Bit = STANDARD_ID;													//zu sendende CAN-Botschaft besitzt einen Standard-ID
			cfsendFrame.RTR_Bit = DATA_FRAME;													//es wird ein Daten-Frame gesendet
			cfsendFrame.ulID = 0x300;															//Adresse, des zu versendenden Frames
			cfsendFrame.ucLength = 3;															//CAN-Botschaft besteht aus 3 Bytes
			cfsendFrame.ucData[0] = (unsigned char) usAbstand;									//Low Bite auf das zu sendende Frame schreiben
			cfsendFrame.ucData[1] = (unsigned char) (usAbstand >> 8);							//High Bite auf das zu sendende Frame schreiben
			
			if (usAbstand <= usSW)																//falls der gemessene Abstand kleiner ist als der Schwellenwert
			{
				cfsendFrame.ucData[2] = 0x55;													//wird auf dem CAN-Frame ein Alarm auf ON gesetzt
			}
			else																				//andernfalls
			{
				cfsendFrame.ucData[2] = 0x00;													//wird der Alarm auf OFF gesetzt
			}
			
			MCP2515_Send_Message(MCP2515_1, &cfsendFrame);										//zuvor definierte CAN-Botschaft wird gesendet
			
			//Abstand auf Display ausgeben
			Display_SetCursor(0,8);
			Display_Write ((usAbstand%10)+48);
			usAbstand/=10;
			Display_SetCursor(0,7);
			Display_Write((usAbstand%10)+48);
			usAbstand/=10;
			Display_SetCursor(0,6);
			Display_Write((usAbstand%10)+48);
			
		}
		
		if (Timer0_get_10msState() == TIMER_TRIGGERED)											//alle 10ms
		{
			//Messtrigger ausl�sen
			if (ucMessungFlag == ON)															//Falls das Trigger-Flag gesetzt wurde
			{
				PORTD |= (1<<PD3);																//Trigger-Signal auf HIGH
				ucMessungFlag = OFF;															//Flag ist nicht mehr gesetzt
			}
			else																				//andernfalls (Flag nicht gesetzt)
			{
				PORTD &= ~(1<<PD3);																//Trigger wieder auf LOW (Triggersignal war 10ms HIGH)
			}
				
		}
    }
}

void Lichtstreifen_Init()																		//Initialisierung des LED-Streifens auf Zusatzplatine
{
	PCA9534_Write_Register(0x00,CONFIGURATION_REGISTER,0x00);									//setzt alle Pins auf OUTPUT
	PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0xFF);											//0xFF schaltet alle LEDs auf LOW
}

void Ultraschallsensor_Init()																	//Initialisierung des Ultraschallsensors
{
	DDRD &= ~(1 << DDD2);																		//Echo-Signal des Sensors wird als Eingang konfiguriert
	DDRD |= (1 << DDD3);																		//Trigger-Signal des Sensors wird als Ausgang konfiguriert
	PORTD &= ~(1 << DDD3);																		//Trigger-Ausgang wird auf LOW gesetzt
}

void Init()																						//Zusammenfassung aller Initialisierungen in einer Init-Funktion
{
	TWI_Master_Init (TWI_MASTER_CLOCK);															//Definiert Microcontroller als Master, �bergibt gew�nschten Takt
	MCP2515_Init(MCP2515_1, BAUDRATE_250_KBPS);													//CAN: PIN & Baudrate 
	Lichtstreifen_Init();																		//Initialisierung des LED-Streifens
	Ultraschallsensor_Init();																	//Initialisierung des Ultraschallsensors
	EI_Init();																					//Initialisierung des externen Interrupts zum Erfassen der Flanken des Echo-Signals
	Timer0_Init();																				//Initialisierung des Timers0 zum Starten der Messung
	Timer1_Init();																				//Initialisierung des Timers1 zum Messen der Zeitdauer des Echos
	Display_Init();																				//Initialisierung des Displays zum Anzeigen des gemessenen Abstands
}

void Lichtstreifen_ansteuern(unsigned short usSchwellenwert, unsigned short usd)				//Funktion zum anschalten der LEDs entsprechend des Abstands
{
	unsigned short ussw;																		//Variable zum Speichern der Schrittweite, nach welcher die n�chste LED der Reihe ein- bzw. ausgeschalten wird
	ussw = (300 - usSchwellenwert) / 8;															//Berechnung der Schrittweite
	unsigned short usLED = (usd-usSchwellenwert)/ussw;											//Berechnung, um LEDs anhand einer switch-case-Funktion ansteuern zu k�nnen
	
	switch(usLED)
	{
		case 0:
			PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b00000000);							//alle LEDs an; Abstand zu einem Objekt sehr gering
			break;
		case 1:
			PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b10000000);							//erste LED aus
			break;
		case 2:
			PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b11000000);							//erste & zweite LED aus					
			break;
		case 3:
			PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b11100000);							//erste, zweite und dritte LED aus
			break;
		case 4:
			PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b11110000);							//erste, zweite, dritte und vierte LED aus
			break;
		case 5:
			PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b11111000);							//erste, zweite, dritte, vierte und f�nfte LED aus
			break;
		case 6:
			PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b11111100);							//erste, zweite, dritte, vierte, f�nfte und sechste LED aus
			break;
		case 7:
			PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b11111110);							//erste, zweite, dritte, vierte, f�nfte, sechste und siebte LED aus
			break;
		case 8:
			PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b11111111);							//alle LEDs aus; weitester Abstand (>=300cm)
			break;
		default:																				//Abstand liegt au�erhalb der Grenzen
			if (usd <= usSchwellenwert)															//falls der Abstand unterhalb oder gleich des Schwellenwertes ist
			{
				PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b00000000);						//alle LEDs an
			}
			else																				//falls der Abstand oberhalb des Schwellenwertes ist
			{
				PCA9534_Write_Register(0x00,OUTPUT_REGISTER,0b11111111);						//alle LEDs aus
			}
		break;
	}
}