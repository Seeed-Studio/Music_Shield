/** \file vs10xx.h
 * Headers for interfacing with the mp3 player chip.
 * Interfacing the New Way, not handling BSYNC -> not compatible with VS1001.
 */


#ifndef VS10XX_H
#define VS10XX_H

//#include "board.h"
#include "ui.h"
#include "config.h"

//extern u8 isWritingVs;
//extern u8 isLoadedPatch; //load patch flag

/** VS10xx SCI Write Command byte is 0x02 */
#define VS_WRITE_COMMAND 0x02

/** VS10xx SCI Read Command byte is 0x03 */
#define VS_READ_COMMAND 0x03


#define SPI_MODE	0x0   /**< VS10xx register */
#define SPI_STATUS	0x1   /**< VS10xx register */
#define SPI_BASS	0x2   /**< VS10xx register */
#define SPI_CLOCKF	0x3   /**< VS10xx register */
#define SPI_DECODE_TIME	0x4   /**< VS10xx register */
#define SPI_AUDATA	0x5   /**< VS10xx register */
#define SPI_WRAM	0x6   /**< VS10xx register */
#define SPI_WRAMADDR	0x7   /**< VS10xx register */
#define SPI_HDAT0	0x8   /**< VS10xx register */
#define SPI_HDAT1	0x9   /**< VS10xx register */
#define SPI_AIADDR	0xa   /**< VS10xx register */
#define SPI_VOL		0xb   /**< VS10xx register */
#define SPI_AICTRL0	0xc   /**< VS10xx register */
#define SPI_AICTRL1	0xd   /**< VS10xx register */
#define SPI_AICTRL2	0xe   /**< VS10xx register */
#define SPI_AICTRL3	0xf   /**< VS10xx register */

#define SM_DIFF		0x01   /**< VS10xx register */
#define SM_JUMP		0x02   /**< VS10xx register */
#define SM_RESET	0x04   /**< VS10xx register */
#define SM_OUTOFWAV	0x08   /**< VS10xx register */
#define SM_PDOWN	0x10   /**< VS10xx register */
#define SM_TESTS	0x20   /**< VS10xx register */
#define SM_STREAM	0x40   /**< VS10xx register */
#define SM_PLUSV	0x80   /**< VS10xx register */
#define SM_DACT		0x100   /**< VS10xx register */
#define SM_SDIORD	0x200   /**< VS10xx register */
#define SM_SDISHARE	0x400   /**< VS10xx register */
#define SM_SDINEW	0x800   /**< VS10xx register */
#define SM_ADPCM        0x1000   /**< VS10xx register */
#define SM_ADPCM_HP     0x2000   /**< VS10xx register */


/** Playing State Global, 0=normal playing, 1=abort playing */
///extern unsigned char playingState;

void TestVsRegister();

/** Execute VS1011/VS1002 Sine Test Function */
void VsSineTest();

void Mp3Reset();

/** Soft Reset of VS10xx (Between songs) */
void Mp3SoftReset();


void Mp3WriteRegister(unsigned char addressbyte,unsigned char highbyte,unsigned char lowbyte);

/** Set VS10xx Volume Register */
#define Mp3SetVolume(leftchannel,rightchannel){\
 Mp3WriteRegister(11,(leftchannel),(rightchannel));}

/** Read the 16-bit value of a VS10xx register */
unsigned int Mp3ReadRegister (unsigned char addressbyte);

/** Send 2048 zeros. \todo Timeouts for all DREQ busy loop waits! */
void SendZerosToVS10xx();

/** Play disk sectors from disk */
unsigned char PlayDiskSectors (unsigned int nSectorsToPlay);


/** This is called when there is free processor time, implement externally. */
void AvailableProcessorTime();


/** Soft reset without loading patch */
void Mp3SoftResetWithoutPatch();

void Mp3PowerOff();

/** Load User Patch */
//void LoadUserPatch(const unsigned short patch[], u32 length) ;

//extern const unsigned short patch1[];
//extern const unsigned short patch2[];
//#define PLUGIN_SIZE1 605
//#define PLUGIN_SIZE2 323

//read a short data from flash
//unsigned short pgm_read_short(const unsigned short *p);

#endif
