//  File keyDisable.cpp
//  Demo Function:The key is disable, so you can use other method to control
//  the Music Shield.
//
//  For more details about the product please check http://www.seeedstudio.com/depot/
//  Copyright (c) 2012 seeed technology inc.
//  Author: Frankie.Chu
//  Version: 1.7
//  Time: June 10, 2012
//  Changing records:
//    Version 1.8 by Jack Shao
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

void setup()
{
  Serial.begin(9600);
  player.keyDisable();//keys disable first;All the keys will be invalid.
  player.begin(); //will initialize the hardware and set default mode to be normal.
  player.setPlayMode(PM_REPEAT_ONE); //set mode to repeat to play a song
  player.scanAndPlayAll(); //If the current playlist is empty,it will add all the songs in the root directory to the playlist.
}
void loop()
{
  player.play();
}
