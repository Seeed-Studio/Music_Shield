//  File digitalInputControl.cpp
//  Demo Function:The key is disable.Connect the Grove - Button to the
//  digital port of Arduino such as port 2 and define its function is to 
//  make volume up.
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

void setup(void)
{
  Serial.begin(9600);
  player.keyDisable(); //keys disable first;
  player.digitalControlEnable();
  player.begin();
  //player.attachDigitOperation(2,playNextSong,HIGH);
  //player.attachDigitOperation(2,playPause,HIGH);
  player.attachDigitOperation(2, volumeUp, HIGH); //the button connected to pin2,and HIGH leval is valid
  player.setPlayMode(PM_NORMAL_PLAY);
  player.scanAndPlayAll(); //If the current playlist is empty,it will add all the songs in the root directory to the playlist.
}
void loop()
{
  player.play();
}
void playNextSong()//User-defined function
{
  player.opNextSong();
}
void playPause()//User-defined function
{
  static boolean flag_on = 1;
  if(flag_on) player.opPlay();
  else player.opPause();
  flag_on = !flag_on;
}
void volumeUp()//User-defined function
{
  player.opVolumeUp();
}
