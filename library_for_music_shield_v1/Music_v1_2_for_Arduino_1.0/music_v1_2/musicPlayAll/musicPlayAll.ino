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
#include "filesys.h"
#include "player.h"
#include "vs10xx.h"
#include "record.h"
#include <SoftwareSerial.h>

SoftwareSerial mySerial(2, 3);//pin2-Rx,pin3-Tx(note: pin3 is actually later used as volume down input)

void setup()
{
  Serial.begin(9600);
  //Serial.println("Hello test!");
  mySerial.begin(19200);// used for receiving command data from the iPod dock.
  
  InitSPI();

  InitIOForVs10xx();

  InitIOForKeys();
  
  InitIOForLEDs();

  InitFileSystem();

  //VsSineTest();

  Mp3Reset();
  
}


void loop()
{ 
#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
   // If play/stop button is pressed during boot, enter recording.
   if (0== PSKey)
   {
  	 delay(20);
	 while(0 == PSKey);
	 delay(20);
        Record();
   }
#endif

   Play();  
   while(1);
}


