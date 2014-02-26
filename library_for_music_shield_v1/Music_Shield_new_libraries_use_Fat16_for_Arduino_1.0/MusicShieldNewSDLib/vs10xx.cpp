// file vs10xx.c
// Functions for interfacing with the mp3 player chip.
//
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

#include <Arduino.h>
#include "vs10xx.h"

#include "config.h"
#include  <avr/pgmspace.h>

void Mp3WriteRegister(unsigned char addressbyte,unsigned char highbyte,unsigned char lowbyte)
{ 
  //SPSR = 0;	
  
  Mp3DeselectData();
  Mp3SelectControl(); 
  //SPIPutCharWithoutWaiting(VS_WRITE_COMMAND); 
  SPIPutChar(VS_WRITE_COMMAND); 
  //delay(1);
  SPIPutChar((addressbyte)); 

  SPIPutChar((highbyte)); 
  SPIPutChar((lowbyte)); 
  SPIWait(); 
  Mp3DeselectControl(); 
  
  //SPSR = (1<<SPI2X);	
}
void Mp3WriteRegisterWithDelay(unsigned char addressbyte,unsigned char highbyte,unsigned char lowbyte,int delayMicroSec)
{ 
  //SPSR = 0;	
  
  Mp3DeselectData();
  Mp3SelectControl(); 
  //SPIPutCharWithoutWaiting(VS_WRITE_COMMAND); 
  SPIPutChar(VS_WRITE_COMMAND); 
  delayMicroseconds(delayMicroSec);
  SPIPutChar((addressbyte)); 

  SPIPutChar((highbyte)); 
  SPIPutChar((lowbyte)); 
  SPIWait(); 
  Mp3DeselectControl(); 
  
  //SPSR = (1<<SPI2X);	
}

void Mp3WriteRegisterWithDelay(unsigned char addressbyte,unsigned char highbyte,unsigned char lowbyte)
{ 
  //SPSR = 0;	
  
  Mp3DeselectData();
  Mp3SelectControl(); 
  //SPIPutCharWithoutWaiting(VS_WRITE_COMMAND); 
  SPIPutChar(VS_WRITE_COMMAND); 
  delay(10);
  SPIPutChar((addressbyte)); 

  SPIPutChar((highbyte)); 
  SPIPutChar((lowbyte)); 
  SPIWait(); 
  Mp3DeselectControl(); 
  
  //SPSR = (1<<SPI2X);	
}

/** Read the 16-bit value of a VSer */
unsigned int Mp3ReadRegister (unsigned char addressbyte){
  	unsigned short resultvalue = 0;
    unsigned char h = 0;
    unsigned char l = 0;
	Mp3DeselectData();
	Mp3SelectControl();//XCS = 0
	SPIPutChar(VS_READ_COMMAND); //send read command
	SPIPutChar(addressbyte);	 //send register address
        
    //be careful: when read 0xff,dump it,and read agin
    //while((h = SPIGetChar())== 0xff);
     h = SPIGetChar();
         
    //while((l = VsReadByte())== 0xff);
     l = SPIGetChar();
        
     resultvalue = ((h << 8) | l);

        
	Mp3DeselectControl();
	return resultvalue;
}

/** Soft Reset of VS10xx (Between songs) */
void Mp3SoftReset(){
  /* Soft Reset of VS10xx */
  Mp3WriteRegister (SPI_MODE, 0x08, 0x04); /* Newmode, Reset, No L1-2 */
  //Delay(1);
  while (!MP3_DREQ); /* wait for startup */
    
  /* Set clock register, doubler etc. */
  Mp3WriteRegisterWithDelay(SPI_CLOCKF, 0xB8, 0x00); 
  
    //setup I2S (see page77 of the datasheet of vs1053 )
  //set GPIO0 as output
  Mp3WriteRegister(SPI_WRAMADDR, 0xc0, 0x17);
  Mp3WriteRegister(SPI_WRAM, 0x00, 0xf0);
  //enable I2S (MCLK enabled, 48kHz sample rate)
  Mp3WriteRegister(SPI_WRAMADDR, 0xc0, 0x40);
  Mp3WriteRegister(SPI_WRAM, 0x00, 0x0C);
  
  while (!MP3_DREQ);
  
  //LoadUserPatch();

}

/** Soft Reset of VS10xx (Between songs) */
void Mp3SoftResetWithoutPatch(){
  /* Soft Reset of VS10xx */
  Mp3WriteRegister (SPI_MODE, 0x08, 0x04); /* Newmode, Reset, No L1-2 */
  delay(1);
  while (!MP3_DREQ) /* wait for startup */
    ; 
  /* Set clock register, doubler etc. */
  Mp3WriteRegisterWithDelay(SPI_CLOCKF, 0xB8, 0x00); 
  while (!MP3_DREQ);

  
  Mp3WriteRegister(SPI_AUDATA, 0xAC,0x45);
  while (!MP3_DREQ);  

}


/** Reset VS10xx */
void Mp3Reset()
{
  //ConsolePutChar(13);

  Mp3PutInRest();
  delay(100);

  /* Send dummy SPI byte to initialize atmel SPI */
  SPIPutCharWithoutWaiting(0xFF);
  
  /* Un-reset MP3 chip */
  Mp3DeselectControl();
  Mp3DeselectData();
  Mp3ReleaseFromReset();
  while (!MP3_DREQ);

  //TestVsRegister();
   //while(1);
  
  /* Set clock register, doubler etc. */
  Mp3WriteRegisterWithDelay(SPI_CLOCKF, 0xB8, 0x00); 
  
#if 0
  Serial.print("\r\nClockF:");
  Serial.println(Mp3ReadRegister(SPI_CLOCKF),HEX);
#endif
  /* Wait for DREQ */
  while (!MP3_DREQ);
  
  
  Mp3SoftReset();//comment this, as it will be executed everytime playing a music file.
  //Mp3SoftResetWithoutPatch();

  Mp3WriteRegister(SPI_WRAMADDR, 0xc0, 0x13);
#if 0
  ConsoleWrite("\r\n0xC013:");
  ConsolePutHex16 (Mp3ReadRegister(SPI_WRAM));
  ConsolePutHex16 (Mp3ReadRegister(SPI_WRAM));
  ConsoleWrite("\r\n");
#endif

  /* Switch on the analog parts */
  Mp3SetVolume(40,40);
  //Mp3SetVolume(0xff,0xff);
  
  ///SPISetFastClock();
  //Serial.print("\r\nInit: VS10XX\r\n");
  
}

/** VS10xx Sine Test Function - Good getting started example */ 
void VsSineTest()
{
	//volatile unsigned short t1;
  //ConsoleWrite("Not For VS1053!"); /* Needs adjustment */

  /* Reset MP3 chip */
  Mp3PutInRest();       /* Pull xRESET low -> hardware reset */
  delay(100);            /* 100 ms delay */

  /* Send dummy SPI byte to initialize SPI of Atmel microcontroller */
  SPIPutCharWithoutWaiting(0xFF);

  /* Un-reset MP3 chip */
  Mp3DeselectControl();  /* Pull xCS high    */
  Mp3DeselectData();     /* Pull xDCS high   */
  Mp3ReleaseFromReset(); /* Pull xRESET high */
  delay(100);            /* 100 ms delay     */  

  /* VS10xx Application Notes, chapter 4.8 ---------------------------------*/
  /* As an example, let's write value 0x0820 to register 00 byte by byte    */
  Mp3SelectControl();    /* Now SPI writes go to SCI port                   */
  SPIPutChar(VS_WRITE_COMMAND);      /* Send SPI Byte, then wait for byte to be sent.   */
  SPIPutChar(SPI_MODE);      /* 0x02 was WRITE command, 0x00 is register number */
  SPIPutChar(0x08);      /* This byte goes to MSB                           */
  SPIPutChar(0x20);      /* ..and this is LSB. (0x20=Allow Test Mode)       */
  SPIWait();             /* Wait until Atmel MCU signals SPI write complete */  
  Mp3DeselectControl();  /* Now SPI writes don't go to SCI port             */
  //test SPI read
  //Mp3WriteRegister(SPI_BASS, 0xAA, 0x55);
  //t1 = Mp3ReadRegister(SPI_BASS);
  //ConsolePutHex16(t1);
  //Mp3WriteRegister(SPI_MODE, t1>>8, t1&0xff);
  //TestVsRegister();
  //while(1);

  while (!MP3_DREQ);      /* Wait for DREQ = 1                               */
     			 /* Do nothing while waiting for DREQ = 1           */

  /* Send a Sine Test Header to Data port                                   */
  Mp3SelectData();       /* Now SPI writes go to SDI port                   */

  SPIPutChar(0x53);      /* - This is a special VLSI Solution test header - */
  SPIPutChar(0xef);      /* - that starts a sine sound. It's good for     - */
  SPIPutChar(0x6e);      /* - testing your code, the chip and also for    - */
  SPIPutChar(0x44);//7e      /* - seeing if your MP3 decoder was manufactured - */
  SPIPutChar(0x00);      /* - by VLSI Solution oy. ------------------------ */
  SPIPutChar(0x00);
  SPIPutChar(0x00);
  SPIPutChar(0x00);
  SPIWait();
  Mp3DeselectData();
  //while(1);//stop here
  
  //RED_LED = LED_OFF;
  
  delay(500);           /* 500 ms delay */
  //GREEN_LED = LED_OFF;
  
  
  /* Stop the sine test sound */
  Mp3SelectData();
  SPIPutChar(0x45);
  SPIPutChar(0x78);
  SPIPutChar(0x69);
  SPIPutChar(0x74);
  SPIPutChar(0x00);
  SPIPutChar(0x00);
  SPIPutChar(0x00);
  SPIPutChar(0x00);
  SPIWait();
  Mp3DeselectData();

  delay(500);            /* 500 ms delay */
}  



/** Send 2048 zeros. */
void SendZerosToVS10xx(){
  Mp3SelectData();
  SPIPutCharWithoutWaiting(0);
  for (temp.i=0; temp.i<1048; temp.i++)
  { // TESTING 1048 TESTING 
    while (!MP3_DREQ);
    SPIPutChar(0);
  }
  SPIWait();
  Mp3DeselectData();
}  




