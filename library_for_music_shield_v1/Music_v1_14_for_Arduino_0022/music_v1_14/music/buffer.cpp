/** \file buffer.c
 * Global buffer for file I/O.
 * Supported data types: Raw buffer, FAT entries.
 */


#include "buffer.h"
//#include "function.h"

/** File I/O buffer */
 DiskBlockType diskSect;
FatType fatInfo;
	
/** Global Data Buffer pointer */
 unsigned char *dataBufPtr;

addressType sectorAddress;

//ALBUM_TABLE album[ALBUM_MAX_NUMBER];


