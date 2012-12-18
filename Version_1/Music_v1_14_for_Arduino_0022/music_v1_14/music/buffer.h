/** \file buffer.h
 * Global buffer for file I/O.
 * Supported data types: Raw buffer, FAT entries.
 */
#include "WProgram.h"

#ifndef BUFFER_H
#define BUFFER_H

//typedef unsigned char byte;	/**< 8 bits, unsigned */
//typedef unsigned short unsigned short;	/**< 16 bits (make sure!), unsigned */
typedef unsigned long u_32;	/**< 32 bits (make sure!), unsigned */


/** FAT/VFAT directory record union */
union DirRecordUnion {
  /** Standard directory entry */
  struct Entry {
    byte Name[11];
    byte Attr;
    byte NTRes;
    byte CrtTimeTenth;
    unsigned short CrtTime;
    unsigned short CrtDate;
    unsigned short LstAccDate;
    unsigned short FstClusHi;
    unsigned short WrtTime;
    unsigned short WrtDate;
    unsigned short FstClusLo;
    u_32 FileSize;
  } entry;
#if 0
  /** Extended directory entry */
  struct LongEntry {
    byte Ord;
    unsigned short Name1[5]; /**< characters 1-5 */
    byte Attr;
    byte Type; /**< entry type, zero=long name component */
    byte Chksum;
    unsigned short Name2[6]; /**< characters 6-11 */
    unsigned short FstClusLO; /**< zero for long entry */
    unsigned short Name3[2]; /**< characters 12-13 */
  } longentry;
#endif
};
 
/** FAT/VFAT directory record type */
typedef union DirRecordUnion dirrecordtype;


 typedef struct FatInfo {		
    byte BS_jmpBoot[3];		/**< x86 Boot Jump Code */
    byte BS_OEMName[8];		/**< Formatter's name, usually "MSWIN4.1" */
    
    unsigned short BPB_BytsPerSec;	/**< Bytes per sector (512) */
    byte BPB_SecPerClus;	/**< Sectors per Cluster (1,2,4,8,..,128) */
    unsigned short BPB_RsvdSecCnt;	/**< Reserved sectors (1 (32 for FAT32)) */
    byte BPB_NumFATs;		/**< Number of FATs (2) */
    unsigned short BPB_RootEntCnt;	/**< FAT12/16 n of root dir entries */
    u_32 BPB_TotSec16;		/**< Old sector count (0 for FAT32) */
    byte BPB_Media;		/**< Media Type (eg 0xF8) */
    unsigned short BPB_FATSz16;		/**< Size of one FAT16 in sectors */
    unsigned short BPB_SecPerTrk;		/**< Old CHS Sectors Per Track */
    unsigned short BPB_NumHeads;		/**< Old CSH Number of Heads */
    u_32 BPB_HiddSec;		/**< n of sectors before this volume */
    u_32 BPB_TotSec32;		/**< New sector count (0 for FAT12/16) */
   
    /** FAT type specific extensions */
    union ExtensionsInfo{	

      /** FAT12/16 specific extensions to Bios Parameter Block*/
      struct Fat16SpecificInfo {	
	byte BS_DrvNum;		/**< DOS INT13 Drive Number (0x80=HD)*/
	byte BS_Reserved1;	/**< For WINNT; Format to 0 */
	byte BS_BootSig;	/**< 0x29 if next 3 fields are present */
	byte BS_VolID[4];	/**< Volume ID (usually format datetime) */
	byte BS_VolLab[11];	/**< Volume Label */
	byte BS_FilSysType[8];	/**< Decorative name of fs, eg "FAT16   "*/
      } _16;

      /** FAT32 specific extensions to Bios Parameter Block*/
      struct Fat32SpecificInfo {	
        u_32 BPB_FATSz32;	/**< Size of one FAT32 in sectors */
	unsigned short BPB_ExtFlags;	/**< Flags; active FAT number etc */
	unsigned short BPB_FSVer;		/**< File System Version (0x0000) */
	u_32 BPB_RootClus;	/**< Start cluster of Root Dir. (2) */
	unsigned short BPB_FSInfo;	/**< Start sector of FSINFO in Resvd area */
	unsigned short BPB_BkBootSec;	/**< Sector in Resvd area for BkBoot (6) */
	byte BPB_Reserved[12];	/**< Reserved, Always 0. */
	byte BS_DrvNum;      	/**< DOS INT13 Drive Number (0x80=HD) */
	byte BS_Reserved1;      /**< For WINNT, Format to 0 */
	byte BS_BootSig;	/**< 0x29 if next 3 fields are present */
	byte BS_VolID[4];	/**< Volume ID (usually format datetime) */
	byte BS_VolLab[11];	/**< Volume Label */
	byte BS_FilSysType[8];  /**< Decorative name of fs, eg "FAT32   "*/
      } _32;
    } ext;
  } FatType;
  
/** Disk Block type including RAW data and FAT deciphering structures. */
/* FAT structure definition from Microsoft's FAT32 File System
 * Specification, version 1.03. Always use 512 byte disk block. */

typedef union DiskBlock {
  
  /** Boot Sector / Bios Parameter Block structure*/
  struct Fat {		
    byte BS_jmpBoot[3];		/**< x86 Boot Jump Code */
    byte BS_OEMName[8];		/**< Formatter's name, usually "MSWIN4.1" */
    
    unsigned short BPB_BytsPerSec;	/**< Bytes per sector (512) */
    byte BPB_SecPerClus;	/**< Sectors per Cluster (1,2,4,8,..,128) */
    unsigned short BPB_RsvdSecCnt;	/**< Reserved sectors (1 (32 for FAT32)) */
    byte BPB_NumFATs;		/**< Number of FATs (2) */
    unsigned short BPB_RootEntCnt;	/**< FAT12/16 n of root dir entries */
    unsigned short BPB_TotSec16;		/**< Old sector count (0 for FAT32) */
    byte BPB_Media;		/**< Media Type (eg 0xF8) */
    unsigned short BPB_FATSz16;		/**< Size of one FAT16 in sectors */
    unsigned short BPB_SecPerTrk;		/**< Old CHS Sectors Per Track */
    unsigned short BPB_NumHeads;		/**< Old CSH Number of Heads */
    u_32 BPB_HiddSec;		/**< n of sectors before this volume */
    u_32 BPB_TotSec32;		/**< New sector count (0 for FAT12/16) */
   
    /** FAT type specific extensions */
    union Extensions{	

      /** FAT12/16 specific extensions to Bios Parameter Block*/
      struct Fat16Specific {	
	byte BS_DrvNum;		/**< DOS INT13 Drive Number (0x80=HD)*/
	byte BS_Reserved1;	/**< For WINNT; Format to 0 */
	byte BS_BootSig;	/**< 0x29 if next 3 fields are present */
	byte BS_VolID[4];	/**< Volume ID (usually format datetime) */
	byte BS_VolLab[11];	/**< Volume Label */
	byte BS_FilSysType[8];	/**< Decorative name of fs, eg "FAT16   "*/
      } _16;

      /** FAT32 specific extensions to Bios Parameter Block*/
      struct Fat32Specific {	
        u_32 BPB_FATSz32;	/**< Size of one FAT32 in sectors */
	unsigned short BPB_ExtFlags;	/**< Flags; active FAT number etc */
	unsigned short BPB_FSVer;		/**< File System Version (0x0000) */
	u_32 BPB_RootClus;	/**< Start cluster of Root Dir. (2) */
	unsigned short BPB_FSInfo;	/**< Start sector of FSINFO in Resvd area */
	unsigned short BPB_BkBootSec;	/**< Sector in Resvd area for BkBoot (6) */
	byte BPB_Reserved[12];	/**< Reserved, Always 0. */
	byte BS_DrvNum;      	/**< DOS INT13 Drive Number (0x80=HD) */
	byte BS_Reserved1;      /**< For WINNT, Format to 0 */
	byte BS_BootSig;	/**< 0x29 if next 3 fields are present */
	byte BS_VolID[4];	/**< Volume ID (usually format datetime) */
	byte BS_VolLab[11];	/**< Volume Label */
	byte BS_FilSysType[8];  /**< Decorative name of fs, eg "FAT32   "*/
      } _32;
    } ext;
  } fat;

  /** Raw Data (interpret sector as array of 8-bit chars)*/
  struct Raw {
    byte buf[512];		/**< Raw Data */
  } raw;

  /** Interpret sector as directory records */
  dirrecordtype dir[16];
  
  /** Interpret sector as array of 16-bit integers */
  unsigned short Fat16Table[256];

  /** Interpret sector as array of 32-bit integers */
  u_32 Fat32Table[128];



} DiskBlockType;



/** 32-bit data type that is also "byte addressable".
 * \warning Byte order is for LITTLE-ENDIAN ARCHITECTURES, change
 * byte order for big-endian compilers/architectures! */
union Address {
  unsigned long l;
  struct B {
    unsigned char b0;
    unsigned char b1;
    unsigned char b2;
    unsigned char b3;
  } b;
};

/** Address type */
typedef union Address addressType;

/** Dirty but powerful global variable that is used for
 * 1) remembering which sector is in the RAM disk buffer diskSect and
 * 2) possibly other weird and dirty purposes, be very careful with this!
 */
extern addressType sectorAddress;

/** File I/O buffer */
extern  DiskBlockType diskSect;
extern FatType fatInfo;

/** Global Data Buffer pointer 
 * Used mainly for filling the buffer and reading MP3 data to send to
 * the codec, be very careful with what you do with this global pointer!
*/
extern unsigned char *dataBufPtr;



#endif


