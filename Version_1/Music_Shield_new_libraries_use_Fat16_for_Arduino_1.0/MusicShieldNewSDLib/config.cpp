//  File config.cpp
//  Functions for configurating IOs,SPI to Music Shield
//  For more details about the product please check http://www.seeedstudio.com/depot/

//  Copyright (c) 2010 seeed technology inc.
//  Author: Icing Chang
//  Version: 1.0
//  Time: Oct 21, 2010
//  Changing records:
//    
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <avr/io.h>
#include "config.h"

union Temp temp;

void InitSPI()
{
	DDRB &= ~(1 << SPIDI);
	DDRB |= (1 << SPIDO);
	DDRB |= (1 << SPICLK);
	#if defined(__AVR_ATmega1280__)||defined(__AVR_ATmega2560__)
	DDRB |= (1 << SPICS) |(1<<PB0) ;//PB0 is must, because it the SS of SPI of the ATMega1280
	#else
	DDRB |= (1 << SPICS);
	#endif

	SPCR = (1 << SPE) | (1 << MSTR) |(1 << SPR0);// | (1 << SPR0) ;//SPICLK=CPU/16
        SPSR = 0;
	
	//PORTB |= (1 << SPICS)|(1<<PB0);//deselect mmc when initial
        PORTB |= (1 << SPICS);
  //Serial.println("Initial SPI ok!");
}

void SPIPutChar(unsigned char data)
{
	SPDR =data;
	while(!(SPSR & (1<<SPIF)));
}
unsigned char SPIGetChar()
{
  unsigned char data = 0;
  SPDR =0xFF;
  SPIWait();
  data = SPDR;
  return data;
}

//Initialize IO control ports of vs10xx
void InitIOForVs10xx()
{
  VS_DDR &= ~(1 << VS_DREQ);//input
  VS_DDR |= (1 << VS_XDCS);//output
  VS_DDR |= (1 << VS_XCS);//output
  VS_DDR |= (1 << VS_XRESET);//output
  
  VS_PORT |= 	(1 << VS_XDCS);//deselect vs_xdcs 
  VS_PORT |= 	(1 << VS_XCS);//deselect vs_xcs

}

//Initialize IO control ports of KEYS
void InitIOForKeys()
{
	#if defined(__AVR_ATmega1280__)||defined(__AVR_ATmega2560__)
	DDRH &= ~(1<<VD);
	DDRH &= ~(1<<BK);
	DDRE &= ~(1<<PS);
	DDRG &= ~(1<<VU);
	DDRE &= ~(1<<NT);
        //internal pull-high
	PORTH |= (1<<VD | 1<<BK);
	PORTE |= (1<<PS | 1<<NT);
	PORTG |= 1<<VU ;

	#else
	DDRD &= ~(1<<VD);
	DDRD &= ~(1<<BK);
	DDRD &= ~(1<<PS);
	DDRD &= ~(1<<VU);
	DDRD &= ~(1<<NT);
        //internal pull-high
	PORTD |= (1<<VD | 1<<BK | 1<<PS | 1<<VU | 1<<NT);
	#endif
}

//Initialize IO control ports of LEDS
void InitIOForLEDs()
{
	LED_DDR |= (1<<RED_LED);
	LED_DDR |= (1<<GREEN_LED);

	RED_LED_OFF();
	GREEN_LED_ON();
}

