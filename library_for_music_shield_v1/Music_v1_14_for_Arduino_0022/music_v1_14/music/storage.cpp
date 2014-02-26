
#include "storage.h"
#include "mmc.h"
#include "buffer.h"

/** Storage status flags.  
 * - Bit 1 (LSB): 1=Device does not support interrupted read 
 * - Bit 2: 1=A sector is seeked and ready for reading
 * - Bit 3: 1=No storage
*/
 unsigned char storageFlags = 0;
 
/** Initialize the storage system.
 * - returns 0 when successful 
 * - returns 1 if mmc was not properly initialized
 */
unsigned char InitStorage()
{
  unsigned char result;

  result=InitMMC();//Intial ok,support for seek-before-read 11:17 1/12/2010 icing
  if (result==0x0e){ //ok, no support for seek-before-read
    //Serial.print("\r\nStorage initialized in seek-and-read mode.\r\n");
    storageFlags = 1;
    return 0;
  }

  if (result){ //error resulted in MMC startup
    //Serial.print("\r\nInitStorage: Can't start MMC. ");
    storageFlags = 4;
    return 1; //MMC Init Error
  }

  //Serial.print("\r\nInitStorage ok.\r\n");
  storageFlags = 0;
  return 0;
}

unsigned char PrepareToReadDiskSector(unsigned long sectorN)
{

  if (!storageFlags)
  {
    //Storage device supports seek-before-read
    if (SeekSector(sectorN))
	{
      return 0x0f; //seek error code
    }
    storageFlags |= 0x02; //flag: a sector is seeked for reading

  }

  return 0; //ok return
}


/** Read one 512 byte disk sector to extern global char[512] diskSect.
 * Returns 0 when successful, error code 6 otherwise.
 */
unsigned char ReadDiskSector(unsigned long sectorN)
{



  //if a sector has not already been seeked, seek now.
  if (!(storageFlags&0x02)){ 

    if (SeekSector(sectorN)) return 0x0f; //seek error
  }
  
  storageFlags &= 0xfd; //clear sector-already-seeked flag
  if (ReadPhysicalSector()){
  	//seek again
  	SeekSector(sectorN);
	if(ReadPhysicalSector())
    	{
    		//Serial.print("\r\nerror read>\r\n");
    		return 0x10; //read error
	}
  }



  return 0; /* All OK return */
}

#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
/** Write one 512  byte disk sector from extern global diskSect.
 * Sector address (0=first 512B sector, 1=2nd. sector, etc.)
 * is in extern global unsigned long sectorAddress
 * \warning Unimplemented!
 */

void WriteDiskSector(unsigned long sectorN){
  sectorAddress.l = sectorN;
  dataBufPtr = diskSect.raw.buf;
  WritePhysicalSector();
}
#endif


