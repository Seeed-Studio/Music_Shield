//  File player.cpp
//  Functions for user interface
//  For more details about the product please check http://www.seeedstudio.com/depot/
//
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
#include <Fat16.h>
#include <Fat16Util.h>
#include "string.h"
#include "player.h"
#include "config.h"
#include "vs10xx.h"
#include "newSDLib.h"

unsigned char playStop = 1; // play or stop flag,1-play,0-stop

//do led controlling
void controlLed()
{
  byte static flagOn = 1;
  int static count = 100000;
  if(--count == 0)
  {
    count = 100000;
    if(flagOn == 1) 
    {
      GREEN_LED_ON();
      flagOn = 0;
    }
    else
    {
      GREEN_LED_OFF();
      flagOn = 1;
    }
  }
}
/*******************************************************************/
unsigned char g_volume = 40;//used for controling the volume
int redPwm = 200;//used for controling the brightness of red led
//do key scaning
void scanKey()
{
  //static unsigned char volume = 40;
  static unsigned int vu_cnt = 1000;//volume up interval
  static unsigned int vd_cnt = 1000;//volume down interval

  
  if(0 == PSKey)
  {
  	playStop = 1-playStop;
	delay(20);
	while(0 == PSKey);
	delay(20);

  }
  else if(0 == VUKey)
  {
  	if(--vu_cnt == 0)
	{
    		if (g_volume-- == 0) g_volume = 0; //Change + limit to 0 (maximum volume)
    
		Mp3SetVolume(g_volume,g_volume);		

		redPwm = (175-g_volume)*3>>1;
		if(redPwm >255)
		{
			redPwm = 255;
		}
		if(redPwm < 0)
		{
			redPwm = 0;
		}
		
		//Serial.println(redPwm,DEC);
		vu_cnt = 1000;
	 }
  }
  else if (0 == VDKey)
  {
    if(--vd_cnt == 0)
	{
      		if (g_volume++ == 254) g_volume = 254; //Change + limit to 254 (minimum vol)
	
	 	Mp3SetVolume(g_volume,g_volume);

		redPwm = 305-(g_volume<<1);
		if(redPwm >255)
		{
			redPwm = 255;
		}
		if(redPwm < 0)
		{
			redPwm = 0;
		}
		//Serial.println(redPwm,DEC);
	  	vd_cnt = 1000;
	 }
       	         
  }  
}

//do some interactive things when vs1053 is busy
void AvailableProcessorTime()
{
  scanKey();
  controlLed();
}
int playFile(void)
{

  Mp3SoftReset();//it is necessary to play each music file
 /* if(!openFile(fileName)){//open music file
    return 0;
  }*/
  
  uint16_t offset = random(1,listoffset - 1);
  if(!openFile(*(indexlist + offset))){//open music file
    return 0;
  }
  
  int readLen = 0;
  byte readBuf[READ_BUF_LEN];
  byte *tp = readBuf;
  while(1)
  {
    readLen = readFile(readBuf,READ_BUF_LEN);//read file content length of 512 every time
    tp = readBuf;
    //Serial.println(readLen);

    Mp3SelectData();

    while (tp < readBuf+readLen)
    {
      if (!MP3_DREQ)
      {
        while (!MP3_DREQ)
        {
          Mp3DeselectData();
          while(1){
          AvailableProcessorTime();//here vs1053 is busy, so you can do some interactive things, like key scanning,led controlling.
          if(1 ==playStop)break;
          }
          Mp3SelectData();
        }
      }
      // Send music content data to VS10xx 
      SPIPutChar(*tp++);
    }

    SPIWait();
    Mp3DeselectData();

    if(readLen < READ_BUF_LEN)
    {
      Mp3WriteRegister(SPI_MODE,0,SM_OUTOFWAV);
      SendZerosToVS10xx();
      break;
    }
  };
  Serial.println("played over\r\n");

  if(file.close() == 0)//close file
  {
    error("close file failed");
  }
  return 0; //OK Exit
}

int playFile(char *fileName)
{

  Mp3SoftReset();//it is necessary to play each music file

  if(!openFile(fileName)){//open music file
    return 0;
  }
  
  int readLen = 0;
  byte readBuf[READ_BUF_LEN];
  byte *tp = readBuf;
  while(1)
  {
    readLen = readFile(readBuf,READ_BUF_LEN);//read file content length of 512 every time
    tp = readBuf;
    //Serial.println(readLen);

    Mp3SelectData();

    while (tp < readBuf+readLen)
    {
      if (!MP3_DREQ)
      {
        while (!MP3_DREQ)
        {
          Mp3DeselectData();
          while(1){
          AvailableProcessorTime();//here vs1053 is busy, so you can do some interactive things, like key scanning,led controlling.
          if(1 ==playStop)break;
          }
          Mp3SelectData();
        }
      }
      // Send music content data to VS10xx 
      SPIPutChar(*tp++);
    }

    SPIWait();
    Mp3DeselectData();

    if(readLen < READ_BUF_LEN)
    {
      Mp3WriteRegister(SPI_MODE,0,SM_OUTOFWAV);
      SendZerosToVS10xx();
      break;
    }
  };
  Serial.println("played over\r\n");

  if(file.close() == 0)//close file
  {
    error("close file failed");
  }
  return 0; //OK Exit
}


