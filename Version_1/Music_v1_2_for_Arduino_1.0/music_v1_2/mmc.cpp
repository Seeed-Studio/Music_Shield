
#include "mmc.h"
#include "config.h"
#include "buffer.h"


unsigned char InitMMC()
{
  unsigned char c;
  //delay(100);
  SelectMSD();
  /* Allow MMC some time and clock cycles to reset */
  for (c=0; c<200; c++)
  {    
    SPIPutChar(0xff);
  }
  //Serial.println("allow mmc time over!");
  //delay(100);
  
  if(RebootMMC())
  {
    //Serial.println("Reboot MMC failed");
    return 1;//not able to power up
  }
  
  return 0;
}

/** Try to switch on the MMC */
unsigned char RebootMMC(){
  unsigned char c;

  //Try (indefinitely) to switch on the MMC card
  do{
    SPI8Clocks(8);      
    /* MMC Don't use CRC - seems to be required by some MMC cards? */
    //MmcCommand(0x7B,0,0,0,0);
    /* MMC Init, command 0x40 should return 0x01 (idle) if all is ok. */
    c = MmcCommand(0x40,0,0,0,0);
    //Serial.print(c,HEX);
  }while (c!=0x01);
  // 0x01(idle) response detected.
  
  Serial.print("\r\nCard found, starting... ");
  //delay(50);//it is very important to give some time here, otherwise intialize will be failed.---Icing
  /*Try max 255 times MMC Wake-up call: set to Not Idle (mmc returns 0x00)*/
  c=20;//20 times maybe also ok
  while ((c--)&&(MmcCommand(0x41,0,0,0,16))){
    delay(50);//needs delay here
    if (c==1){
      //timeout
      Serial.print("Failed.\r\n");
      return 2; /* Not able to power up mmc */
    }
  }

  Serial.print("Ok.\r\n");
  return 0;
}

/** Do one MMC command and return the MMC SPI R1 response.
 * Returns 0xff in case of timeout (relies on weak pull-up on the MISO pin). 
 * Note that the parameter bytes are 
 * used for temporary storage after they are written out. */
unsigned char MmcCommand(unsigned char c1, 
			 unsigned char c2, 
			 unsigned char c3, 
			 unsigned char c4, 
			 unsigned char c5){


  volatile unsigned int i;
  volatile unsigned char temp;

  /* Note: c1, c2 are used for temporary variables after use! */  

  // Provide clock edges before and after asserting MMC CS
  
  DeselectMSD();
  SPI8Clocks(8); 
  SelectMSD();
  //while(1)
  SPI8Clocks(8); 
 
  i=0;
  // If card still seems to be busy, give it some time... 
  // changed 12/2005 to give quite a lot of time.
  
  //while ((temp = MMC_ReadByte() !=0xff) && (++i<20000));
  do
  {
  	SPDR =0xFF;
  	SPIWait();
  	temp = SPDR;
    //temp = MMC_ReadByte();
	++i;
  }while(temp != 0xff && i<2000);
	
  //read agin to ensure MISO  == 0xFF, this is very important
  //while ((temp = MMC_ReadByte() !=0xff) && (++i<20000));
  do
  {
  	SPDR =0xFF;
  	SPIWait();
  	temp = SPDR;
	//temp = MMC_ReadByte();
	++i;
  }while(temp != 0xff && i<2000);

  //read agin to ensure MISO == 0xFF , this is very important
  //while ((temp = MMC_ReadByte() !=0xff) && (++i<20000));
  do
  {
  	SPDR =0xFF;
  	SPIWait();
  	temp = SPDR;
	//temp = MMC_ReadByte();
	++i;
  }while(temp != 0xff && i<2000);
  
  //Serial.println("test 1");

  // The bus should be stable high now
///  if ((i=SPI_RESULT_BYTE) != 0xff){
  //temp = MMC_ReadByte();
  SPDR =0xFF;
  SPIWait();
  temp = SPDR;

  if ( temp != 0xff){
    //Serial.print("\r\nUnexpected busy signal from MMC. ");
    //Serial.print(i,HEX);
    //Serial.print("\r\nGet data = ");
    //Serial.print(temp,HEX);

    DeselectMSD();
    //DelayMs(1000);
    return 0x81; //MMC unexpectedly Busy
  }
  
  // Send the MMC command
  /*MSPIPutCharWithoutWaiting(c1);
  MMC_WriteByte(c2);
  MMC_WriteByte(c3);
  MMC_WriteByte(c4);
  MMC_WriteByte(c5);
  MMC_WriteByte(0x95); 	// Valid CRC for init, then don't care 
  MSPIWait();*/
  SPDR =c1;
  SPIWait();
  SPDR =c2;
  SPIWait();
  SPDR =c3;
  SPIWait();
  SPDR =c4;
  SPIWait();
  SPDR =c5;
  SPIWait();
  SPDR =0x95;
  SPIWait();
  //delay(100);
  /* Now ok to use c1..c5 as temporaries (dirty but kool) */
  {
    i=100;
	do
	{
		SPDR =0xFF;
  		SPIWait();
  		temp = SPDR;
		//temp = MMC_ReadByte();
		i--;
	}while((i--)&&(temp&0x80));
	//while((i--)&&((temp=MMC_ReadByte())&0x80)); //wait for R1 or timeout
      
  }
  return temp; //return the R1 response
}

/** Unconditionally (really just do it!) seek MMC at offset sectorN*512.
  In English: Send the Read Sector command to MMC
*/
unsigned char SeekSector(unsigned long sectorN){
  unsigned char c, retries;
  
  //SPISpeed(SPI2, SPI_BaudRatePrescaler_256);
  	
  retries = 0;
  do{
  	//LED_Sel();
    retries++;
    sectorAddress.l = sectorN * 2; //convert to bytes (combined with 8bit shift)
    c=MmcCommand(0x51,sectorAddress.b.b2,sectorAddress.b.b1,//read one block
		 sectorAddress.b.b0, 0);
    sectorAddress.l = sectorAddress.l >> 1; //convert back to blocks

    // If MMC works properly, it returns Busy (== Not idle) at this stage.    
   /* if (c!=0x00){
      if (c != 0xff){ 
        //MMC returns something else than Busy or "Idle Bus", print what it is
        //ConsoleDecipherMMCResponse(c);
		Serial.print(c,HEX);
      }
      // Something is wrong, take the standard action...
      RebootMMC();
      if (retries > 10){  
        return 7; // failed to execute mmc command 
      }
    }*/
  }while(c!=0x00); //repeat until we get busy signal from MMC.
    
  DeselectMSD();
  
  //SPISpeed(SPI2, SPI_BaudRatePrescaler_32);
  
  return 0; //ok return
}


/* Wait for data start token and read 512 bytes to global buffer */
unsigned char ReadPhysicalSector(){

  // RED_LED = LED_ON; /* Disk Read LED on */
//LED_ON(RED_LED);
  
  SelectMSD();
  if(MmcWaitForData())
  {
  	return 5;
  }
  PerformBlockRead();

  /* generate SPI clock edges to finish up the command */

  SPI8Clocks(4); //Send 8*4=32 clocks (4 ff's) to MMC to be nice.
  DeselectMSD();
  SPI8Clocks(4); //Again, give the poor MMC some clocks, it likes them.

  //RED_LED = LED_OFF; /* Disk Read LED off */
//LED_OFF(RED_LED);
  
  return 0; //ok return
}

/** MMC Wait For Data start token 0xfe. 
 * First any 0xFF's are read off the mmc.
 * The first non-ff octet is examined. If it's the 0xfe data start token,
 * everything is fine. If not, then some error has occurred.
 * Since we're in an embedded system, it's unclear what we should do then.
 * Current approach is to say all ok but make read block return 0xff's
 * by dropping the MMC card offline. Before that we read "lots" of
 * octets from the MMC to flush any pending data. Finally we return "ALL OK".
 * It's not disasterous at least as long as we don't WRITE to MMC.
 * 12/2005: Well, now we do write to mmc...
 */
unsigned char MmcWaitForData(){

  unsigned char c;
  unsigned int i; 
  
  

    
  i = 60000; //try max. 60000 bus cycles
  // Wait until something else than 0xff is read from the bus
  do {
    c=SPIGetChar();
    --i;
  } while ((c == 0xff) && (i));

  // Something was received from the bus? Might it actually be te 
  // desired 0xFE data start token? 
  if (c != 0xfe){
    // No data start token, read fail. In an OS an error message would display.
    // Since we're in an embedded system, it's unclear what we should do now.
    // Current approach is to say all ok but make read block return 0xff's.
    // It's not disasterous at least as long as we don't WRITE to MMC.

    // Flush any data that might be pending from the MMC.
#ifdef MMCLONGDEBUG    
    {
      unsigned int i;
      ConsoleWrite("\r\nMMCWaitForData failed. \r\n");
      ConsoleWrite("\r\nExpected 0xFE token, received: ");
      for (i=0;i<550;i++){
	ConsolePutHex8(c);
	c=SPIGetChar();
      }
    }
#else
    Serial.print("\r\n NoData ");
    SPI8Clocks(200); /* Flush MMC by sending lots of FF's to it */
    SPI8Clocks(200);
    SPI8Clocks(200);
#endif
    
    DeselectMSD();
    
    return 5; //Return error
  }
  
  return 0;
}

/** Perform the actual reading of 512 octets from MMC.
 * Note: This is the fast routine to read complete disk block 
 * If you have DMA, write this to use it!
 */
void PerformBlockRead(){

  //SPI_DMACmd(SPI2, SPI_DMAReq_Rx, ENABLE); //ADD DMA
  //while(!DMA_GetFlagStatus(DMA_FLAG_TC4));
  


  /* Use shared global buffer pointer for speed*/
  /* Loop unrolled 16 times for SPEED! :) */
  dataBufPtr = diskSect.raw.buf;
  while (dataBufPtr < diskSect.raw.buf+512){
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    *dataBufPtr++ = SPIGetChar();
    //LED_Sel();
  }


}



#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
/** Perform MMC block write from diskSect */
unsigned char WritePhysicalSector()
{
  unsigned char c;  




  //RED_LED = LED_ON;
  sectorAddress.l = sectorAddress.l * 2; //convert to bytes (combined with 8bit shift)
  c=MmcCommand(0x40 | 24, sectorAddress.b.b2, sectorAddress.b.b1,
	       sectorAddress.b.b0, 0);
  sectorAddress.l = sectorAddress.l >> 1; //convert back to blocks

  //ConsolePutChar('w');
  //ConsolePutHex8(c);
  
  if (c!=0x00) return (c); //Error - MMC did not go to write mode
  
/*
  while (c!=0x00) { //wait for BUSY token, if you get 0x01(idle), it's an ERROR!
    c = SPIGetChar();
    ConsolePutHex8(c);
  }      
*/  
  dataBufPtr = diskSect.raw.buf;
  SPIPutCharWithoutWaiting(0xFE);
  SPIWait();
  
  for (c=0;c<128;c++){
    SPIPutCharWithoutWaiting(*dataBufPtr++);   
    SPIWait();
    SPIPutCharWithoutWaiting(*dataBufPtr++);   
    SPIWait();
    SPIPutCharWithoutWaiting(*dataBufPtr++);   
    SPIWait();
    SPIPutCharWithoutWaiting(*dataBufPtr++);   
    SPIWait();
  }
  //ConsolePutChar('-');

  c = SPIGetChar();  //crc 1st byte (sends 0xff)
  c = SPIGetChar(); //crc 2nd byte (sends 0xff)
  c = SPIGetChar();
  //ConsolePutHex8(c); //This prints xxx00101, (usually e5) when data ok
  
//  while (SPIGetChar()!=0xff)      //busy wait moved to mmcCommand
//    ; // Wait until MMC not busy.     



  
  SPI8Clocks(16);
  DeselectMSD();
  SPI8Clocks(16);

  RED_LED_OFF();

  return 0;
}

#endif



