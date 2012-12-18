//  File playWithName.cpp
//  Demo Function:Play a song in the root directory with a name.
//
//  For more details about the product please check http://www.seeedstudio.com/depot/
//  Copyright (c) 2012 seeed technology inc.
//  Author: Frankie.Chu
//  Version: 1.7
//  Time: June 10, 2012
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
#include <Fat16.h>
#include <Fat16Util.h>
#include <NewSPI.h>
#include <arduino.h>
#include "pins_config.h"
#include "vs10xx.h"
#include "newSDLib.h"
#include "MusicPlayer.h"
MusicPlayer myplayer;
void setup()
{
  Serial.begin(9600);
  myplayer.begin();//will initialize the hardware and set default mode to be normal.
}
void loop()
{
  myplayer.setPlayMode(MODE_REPEAT_ONE);//set mode to repeat to play a song
  myplayer.playSong("test.wav");//play a song named with test.wav
  while(1);//If the mode is normal, it will stop when it finished playing all the songs in the playlist
}
