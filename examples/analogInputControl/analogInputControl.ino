//  File analogInputControl.cpp
//  Demo Function:The key is disable and connect the Grove - Rotary sensor on
//  sail to A4 of Arduino and the rotary sensor can contol the volume.
//
//  For more details about the product please check http://www.seeedstudio.com/depot/
//  Copyright (c) 2012 seeed technology inc.
//  Author: Frankie.Chu
//  Version: 1.7
//  Time: June 10, 2012
//  Changing records:
//     V1.8 by Jacky Shao
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
#include <SD.h>
#include <SPI.h>
#include <arduino.h>
#include <MusicPlayer.h>

void setup(void)
{
  Serial.begin(9600);
  player.keyDisable(); //keys disable first;
  player.analogControlEnable(); //enable to scan the A4/A5
  player.begin(); //will initialize the hardware and set default mode to be normal.
  player.attachAnalogOperation(A4, adjustVolume); //Grove - Rotary Angle Sensor connected to A4,to control the volume
  player.scanAndPlayAll(); //If the current playlist is empty,it will add all the songs in the root directory to the playlist.
}
void loop(void)
{
  player.play();
}
void adjustVolume(void) //User-defined function
{
  unsigned int vol_temp = analogRead(A4);
  unsigned char volume = vol_temp / 12;
  if (volume == 0x55) volume = MAXVOL; //MAXVOL = 0xfe;
  player.setVolume(volume);
}
