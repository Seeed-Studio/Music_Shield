/** \file mmc.h
 * MMC interface routines for storage.c.
 */

#ifndef MMC_H
#define MMC_H

extern unsigned char storageFlags;

/** Start-up the MMC card.
 * - Returns 0 when successful and supports seek-before-read
 * - Returns 0x0e when successful but no support for seek-before-read 
 * - Returns 1 when MMC card is not found
 * - Returns 2 when MMC card seems to be present but can't be read from.
*/
unsigned char InitMMC();

unsigned char RebootMMC();

unsigned char MmcCommand(unsigned char c1, unsigned char c2, unsigned char c3, unsigned char c4, unsigned char c5);

/** Perform MMC Seek Command for offset sectorN*512 */
unsigned char SeekSector(unsigned long sectorN);

/** Perform block read of previously sought sector to diskSect. */
unsigned char ReadPhysicalSector();

#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
/** Perform MMC block write from <b>*dataBufPtr</b> to sector 
    sectorAddress.l.*/
unsigned char WritePhysicalSector();
#endif

unsigned char MmcWaitForData();

/** Perform the actual reading of 512 octets from MMC.
 * Note: This is the fast routine to read complete disk block 
 * If you have DMA, write this to use it!
 */
void PerformBlockRead();

#endif

