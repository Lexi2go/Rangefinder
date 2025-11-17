/*
 * PCA9534.h
 *
 * Created: 30.08.2015 18:24:34
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


#ifndef PCA9534_H_
#define PCA9534_H_

#include <avr/io.h>
#include "TWI_ATMEGA.h"

uint8_t PCA9534_Write_Register(uint8_t ucdevice_address, uint8_t ucregister, uint8_t ucreg_value);
uint8_t PCA9534_Write_Port(uint8_t ucdevice_address, uint8_t ucport_value);
uint8_t PCA9534_Read_Port(uint8_t ucdevice_address, uint8_t *ucport_value);
uint8_t PCA9534_ReRead_Port(uint8_t ucdevice_address, uint8_t *ucport_value);
uint8_t PCA9534_Read_Register(uint8_t ucdevice_address, uint8_t ucregister, uint8_t *ucport_value);

#define INPUT_REGISTER			0
#define OUTPUT_REGISTER			1
#define POLARITY_REGISTER		2
#define CONFIGURATION_REGISTER	3

#define PCA9534_DEVICE_TYP_ADDRESS	0x40

#endif /* PCA9534_H_ */