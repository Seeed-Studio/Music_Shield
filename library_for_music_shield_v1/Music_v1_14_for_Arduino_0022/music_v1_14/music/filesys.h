
#ifndef FILE_SYS_H
#define FILE_SYS_H

#include "buffer.h"


/** Use PSW User Definable Flag to indicate FAT16/32 */
//#define IS_FAT_32 UDF
extern byte IS_FAT_32;

extern unsigned long fatStart;
extern unsigned long dataStart;
 
extern unsigned long fileSize;



/** Maximum allowable number of fragments in file */
#define MAX_NUMBER_FRAGMENTS 10

/** Fragment Table. */
 struct fragmentEntry {
  unsigned long start; /**< Starting sector of fragment */
  unsigned long length; /**< Length of fragment in sectors */
};

extern struct fragmentEntry  fragment[MAX_NUMBER_FRAGMENTS];

/** 8 first characters of current file name */
extern char currentFileName[12];

/** Start the filing system and initialize all necessary subsystems.
    Init storage and file system. FAT16 and FAT32 are supported */
unsigned char InitFileSystem();

 /** Open a file for reading. 
 * Prepares the Filing System to read a data file from the storage.
 * Files are referred to by their numbers, not file names. This makes the
 * system generic, not necessarily needing a complex file system such as
 * FAT. The way to assign numbers to files is implementation dependent.
 * Returns 0 when ok, error code otherwise.
 * \param fileNumber number of file, starting from beginning of storage,
 * to open. 
 *
 * What this function actually does, is: it starts reading the FAT
 * records from start of the root directory, traversing through
 * subdirectories as it encounters them. When the fileNumber'th valid
 * record is encountered, it sets sectorAddress to point to its
 * first data sector but doesn't load any sector. fileSize is loaded
 * with the size in bytes indicated in the FAT record.
*/
unsigned char OpenFile (unsigned int fileNumber);

/** Decipher structure of FAT volume and set globals accordingly. */ 
unsigned char FatInitGlobals();

/** Build a fragment table starting from current sector. 
 * Returns number of fragments in song. 
 * This function is used to get fast access to the filesystem when
 * playing so cluster chain needs not be followed when the song
 * is playing, that would be too slow with MMC cards without 
 * extra buffer memory for the file allocation tables.
 * \warning This function is too slow! (several seconds per MP3 file),
 * todo: write specialized routines for FAT16 and FAT32. */

unsigned char BuildFragmentTable(void);


/** Calculate next sector from currentSector.
 *  Return sector address with MSB set if end of file. 
 *
 * This is the essence of FAT filesystem: traversing through
 * the file allocation table to scan for cluster chains.
*/
unsigned long GetNextSector(unsigned long currentSector);


#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
/** Search for next free sector.
 * If freeSector is zero, a new file should be allocated. */    
unsigned char ScanForFreeSector();

/** Next free cluster number */
extern unsigned long freeSector;

extern unsigned char fatSectorsPerCluster;

void WriteClusterChain();
#endif

#endif

