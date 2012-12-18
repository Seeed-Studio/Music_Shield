/** \file storage.h
 * Disk read/write routines.
 * Implemented Disk interface: MMC.
 * This module reads and writes blocks of disk data to/from a global
 * buffer. The logical place to define these globals is where they are
 * needed most: the module that contains the file system functions.
 * By default that means filesys.c.
 * These globals are needed (see filesys.h):
 * - sectorAddress - contains the absolute
 * disk sector number
 * - diskSect[512] for data read/write
 */

#ifndef STORAGE_H
#define STORAGE_H


/** Initialize the storage system */
unsigned char InitStorage();

/** Give a hint to the storage that the next sector we will
 * read is sectorN. If the storage device supports performing
 * a separate seek before the data is read (for instance MMC card
 * that can be taken off-line between seek and read), storage
 * seeks in this function. The user should not need to worry
 * about this and calling PrepareToReadDiskSector is voluntary. */
unsigned char PrepareToReadDiskSector(unsigned long sectorN);

/** Read a disk sector to global buffer. 
 * 
 * Hint: You can call PrepareToReadDiskSector() to start seeking
 * for a sector before calling this function. If the storage
 * device supports seek-before-read, the time between seek and
 * read can be used for other purposes. If you don't seek with
 * a separate call to PrepareToReadDiskSector() or the storage
 * device does not support separate seeks, a seek is
 * automatically performed in this function. */
unsigned char ReadDiskSector(unsigned long sectorN);

/** Do a hex dump on console of current sector in memory */
//void DumpDiskSector();

#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
/** Write current buffer memory to sectorN
 * \warning Unimplemented!
 * \todo Implement WriteDiskSector()
 */
void WriteDiskSector(unsigned long sectorN);
#endif




#endif

