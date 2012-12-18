
#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)

/** \file record.c ADPCM Recording */
#include "record.h"
#include "filesys.h"
#include "storage.h"
#include "vs10xx.h"
#include "hardwareserial.h"
#include "ui.h"
#include "config.h"
#include  <avr/pgmspace.h>

//extern unsigned char SPMax;



#include "venc44k2q05.h"
//#include "oggvorbis_44k2ch.plg"


void LoadUserCode(void) 
{
  int i = 0;
  int length = sizeof(plugin);
  int dataSize = sizeof(plugin[0]); 

  Serial.print("\r\nLoadUserCode ...");
  while (i<length/dataSize) 
  {
    unsigned short addr, n, val;
    addr = pgm_read_word(&plugin[i++]);
    n = pgm_read_word(&plugin[i++]);
    if (n & 0x8000U) { /* RLE run, replicate n samples */
      n &= 0x7FFF;
      val = pgm_read_word(&plugin[i++]);
      while (n--) {
        Mp3WriteRegister(addr, val>>8, val & 0xff);
      }
    } 
    else {           /* Copy run, copy n samples */
      while (n--) {
        val = pgm_read_word(&plugin[i++]);
        Mp3WriteRegister(addr, val>>8, val & 0xff);
      }
    }
  }
  Serial.println("over!");
}


const unsigned short linToDBTab[5] = {
  36781, 41285, 46341, 52016, 58386};

/*
  Converts a linear 16-bit value between 0..65535 to decibels.
 Reference level: 32768 = 96dB (largest VS1053 number is 32767 = 95dB).
 Bugs:
 - For the input of 0, 0 dB is returned, because minus infinity cannot
 be represented with integers.
 - Assumes a ratio of 2 is 6 dB, when it actually is approx. 6.02 dB.
 */
unsigned short LinToDB(unsigned short n) {
  int res = 96, i;

  if (!n)               /* No signal should return minus infinity */
    return 0;

  while (n < 32768U) { 	/* Amplify weak signals */
    res -= 6;
    n <<= 1;
  }

  for (i=0; i<5; i++)   /* Find exact scale */
    if (n > linToDBTab[i])
      res++;

  return res;
}


unsigned char Record()
{
  unsigned char blockNumber;
  unsigned long sectorCount;
  unsigned long lastSector;
  unsigned char stopRecording = 0;
  unsigned char continueRecording = 0;
  char newDisplayValue ;



  blockNumber = 0;
  sectorCount = 1;

  playingState = PS_RECORDING; //Inform the world that rec mode is on.


  // Locate free space
  /** This recording function can only save data into a continuous
   * area on disk. The Fragment Table fragment[] is used in a special way
   * for recording (to save microcontroller RAM space).
   * fragment[0].start is the first disk sector of the file
   * fragment[0].length will contain the number of disk sectors for the file.
   * 
   * \bug In case of fragmented filesystem, this version will not
   * function properly.
   */
  // Locate a free sector on storage. Uses global u_int freeSector
  freeSector = 0;
  ScanForFreeSector(); 
  sectorAddress.l = freeSector;
  fragment[0].start = freeSector;
  lastSector = freeSector;
  dataBufPtr = diskSect.raw.buf;
  blockNumber = 0;
  Serial.print("\r\nRecording, push button to stop..."); 


  // Kick vs10xx into action!
  Mp3SoftResetWithoutPatch();
  Mp3WriteRegister(SPI_CLOCKF,  0xc0,0x00); 
  delay(1); 
  while(!MP3_DREQ);
  Mp3WriteRegister(SPI_BASS, 0x00,0x00); 
  delay(1); 
  while(!MP3_DREQ);
  Mp3WriteRegister(SPI_AIADDR, 0x00,0x00);
  Mp3WriteRegister(SPI_WRAMADDR, 0xC0,0x1A);
  Mp3WriteRegister(SPI_WRAM, 0x00,0x02);
  LoadUserCode();
  Mp3WriteRegister(SPI_MODE, 0x58,0x00); 
  delay(1); 
  while(!MP3_DREQ);
  Mp3WriteRegister(SPI_AICTRL0, 0x00,0x00); 
  delay(1); 
  while(!MP3_DREQ);
  Mp3WriteRegister(SPI_AICTRL1, 0x00,0x00); 
  delay(1); 
  while(!MP3_DREQ);
  Mp3WriteRegister(SPI_AICTRL2, 0x10,0x00); 
  delay(1); 
  while(!MP3_DREQ);
  Mp3WriteRegister(SPI_AICTRL3, 0x00,0x00); 
  delay(1); 
  while(!MP3_DREQ);
  Mp3WriteRegister(SPI_AIADDR, 0x00,0x34);
  delay(1); 
  while(!MP3_DREQ);


  while(!stopRecording)
  {

    //Delay(100);

    //sample in block[0] and [1]
    signed int soundlevel;
    // the user interface (level bar) calculation

    if (0 == PSKey)
    {   //Request to stop recording
      delay(20);
      while(0 == PSKey);
      delay(20);

      Mp3WriteRegister(SPI_AICTRL3, 0x00,0x01);
    }

    // Ok to stop recording?
    if (Mp3ReadRegister(SPI_AICTRL3) & 1){
      stopRecording = 1;
    }

    soundlevel = Mp3ReadRegister(SPI_AICTRL0);
    Mp3WriteRegister(SPI_AICTRL0,0,0);

    if (soundlevel){
      soundlevel = LinToDB(soundlevel);
      soundlevel *= 3;
      soundlevel -= 185;
      if (soundlevel < 0) soundlevel = 0;
      newDisplayValue = soundlevel;
      //displayValue = newDisplayValue;
    }
    //AvailableProcessorTime();      

    
    
    if (Mp3ReadRegister(SPI_HDAT1) > 255){ 
      //there is a data block to be read...
      //Serial.println("test here 1");
      GREEN_LED_ON();
      blockNumber++;
      dataBufPtr = diskSect.raw.buf;

      for (temp.c=0;temp.c<128;temp.c++){
        unsigned int i;
        i = Mp3ReadRegister(SPI_HDAT0);		
        *dataBufPtr++ = (i>>8);
        *dataBufPtr++ = (i&0xff);
        i = Mp3ReadRegister(SPI_HDAT0);		
        *dataBufPtr++ = (i>>8);
        *dataBufPtr++ = (i&0xff);
      }

      //Release SCI chip select, we might want to use MMC card
      Mp3DeselectControl();

      RED_LED_ON();
      blockNumber = 0;
      sectorCount++;
      WriteDiskSector(sectorAddress.l);
      lastSector = freeSector;
      ScanForFreeSector(); //this would be the proper way to go...
      sectorAddress.l = freeSector; //keep all variables in proper values
      dataBufPtr = diskSect.raw.buf; //reset data buffer pointer
      if (freeSector!=(lastSector+1)){ //end of continuous space, must break!
        stopRecording = 1;
        Serial.println("\r\nFragment end - can't continue recording!");
        //InitDisplay(DS_STATIC,"FRAGMENT"," LIMIT!!",0);
        continueRecording = 1;
      }

      RED_LED_OFF();
      GREEN_LED_OFF();

      //if there was data to read
    } 
    else 
    {
      //Serial.println("test here 2");
      delay(1);
    }

  }//while not button - stop recording when BUTTON is pressed

  while (Mp3ReadRegister(SPI_HDAT1) > 0){ 
    int n = Mp3ReadRegister(SPI_HDAT1);
    n -= 256;


    //there is a data block to be read...
    GREEN_LED_ON();
    blockNumber++;
    dataBufPtr = diskSect.raw.buf;

    for (temp.c=0;temp.c<128;temp.c++){
      unsigned int i;
      i = Mp3ReadRegister(SPI_HDAT0);		
      *dataBufPtr++ = (i>>8);
      *dataBufPtr++ = (i&0xff);
      i = Mp3ReadRegister(SPI_HDAT0);		
      *dataBufPtr++ = (i>>8);
      *dataBufPtr++ = (i&0xff);
    }

    if (n<0) {
      for (temp.c = 0; temp.c<(-n); temp.c++){
        *--dataBufPtr = 0;
        *--dataBufPtr = 0;
      }
    }


    //Release SCI chip select, we might want to use MMC card
    Mp3DeselectControl();

    RED_LED_ON();
    blockNumber = 0;
    sectorCount++;
    WriteDiskSector(sectorAddress.l);
    lastSector = freeSector;
    ScanForFreeSector(); //this would be the proper way to go...
    sectorAddress.l = freeSector; //keep all variables in proper values
    dataBufPtr = diskSect.raw.buf; //reset data buffer pointer
    if (freeSector!=(lastSector+1)){ //end of continuous space, must break!
      stopRecording = 1;
      Serial.println("\r\nFragment end - can't continue recording!");
      //InitDisplay(DS_STATIC,"FRAGMENT"," LIMIT!!",0);
      continueRecording = 1;
    }

    RED_LED_OFF();
    GREEN_LED_OFF();

    if (n<0){
      break;
    }

  }


  fragment[0].length = sectorCount;    
  Serial.print("Registering... FStart:");
  Serial.print(fragment[0].start,DEC);
  Serial.print("\r\nRegistering... Size:");
  Serial.println(fragment[0].length,DEC);
  //Create FAT records.
  fragment[1].start = 0x0fffffff; //fragment 0 is the only fragment
  WriteClusterChain(); //register newly created file in FAT FS

  // \todo second FAT table update 

  sectorAddress.l = 0; //force sector reload for next access

  Mp3SoftReset();
  return PS_NEXT_SONG;

}
#endif




