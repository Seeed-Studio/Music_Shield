
#include <avr/io.h>
#include "config.h"
#include "filesys.h"
#include "player.h"
#include "vs10xx.h"
#include "record.h"
#include <NewSoftSerial.h>

NewSoftSerial mySerial(2, 3);//pin2-Rx,pin3-Tx(note: pin3 is actually later used as volume down input)

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


