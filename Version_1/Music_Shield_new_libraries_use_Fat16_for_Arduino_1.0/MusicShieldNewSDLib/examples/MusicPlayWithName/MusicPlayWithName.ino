//  File MusicPlayWithName.ino
//  Demo code for playing any existing music file on Micro SD card by file name
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

/**************************Note*******************************************
 To make this demo code run sucessfully, you need to download the fat16lib 
 into your Arduino library at the following address:
http://code.google.com/p/fat16lib/downloads/detail?name=fat16lib20101009.zip&can=2&q=
**************************************************************************/

#include <Arduino.h>
#include <avr/io.h>
#include "config.h"
#include "player.h"
#include "vs10xx.h"

#include <Fat16.h>
#include <Fat16Util.h>
#include "newSDLib.h"

void setup()
{
  Serial.begin(9600);

  InitSPI();

  InitIOForVs10xx();

  InitIOForKeys();

  InitIOForLEDs();

  Mp3Reset();
  if (!initialSDCard())
    return;

  RED_LED_ON();//light red LED
}


void loop()
{ 
  playFile("test.mp3"); //play the song with the name you give.
//playFile("test.wma");
//playFile("test.wav");
}



