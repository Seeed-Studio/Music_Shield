
#include "WProgram.h"
#include "filesys.h"
#include "config.h"
#include "storage.h"
#include "string.h"
#include "mmc.h"
#include "buffer.h"

byte IS_FAT_32;

/* Starting sector of FAT table. */
unsigned long fatStart;

/** Starting sector of root directory */
unsigned long rootStart;

/** Sector of (unexistent) data cluster 0. */
/** (In FAT the start is cluster 2, so dataStart actually
 ** points to 2 clusters before the start of data area) */
unsigned long dataStart;
 
/** FAT Global sectors per cluster */
 unsigned char fatSectorsPerCluster;



/** Maximum allowable number of fragments in file */
#define MAX_NUMBER_FRAGMENTS 10


/** Fragment Table. Maximum fragment size is (4G-1 sectors) */
struct fragmentEntry fragment[MAX_NUMBER_FRAGMENTS];

/** Size of current file to play */
 unsigned long fileSize;

/** Maximum number of nested subdirectories */
#define MAX_NUMBER_SUBDIRECTORIES 5

/** Directory traversing structure */
 struct directoryStack {
  unsigned long sector; /**< Directory table sector in disk */
  unsigned char entry;  /**< Entry number (0..15) */
} dirStack[MAX_NUMBER_SUBDIRECTORIES];

/** Current directory level, 0=Root directory */
 unsigned char dirLevel = 0;

#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
/** Next free cluster number */
unsigned long freeSector = 0;
#endif

/** Current file name */
char currentFileName[12];

/** Set if this is a FAT12 filesystem */
//bit IS_FAT_12 = 0;
byte IS_FAT_12 = 0;

#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
/** Convert sector number to cluster number */
void PConvertSectorToCluster(unsigned long *sector){
  *sector-=dataStart;
  *sector/=fatSectorsPerCluster;
}

/** Convert cluster number to sector number */
void PConvertClusterToSector(unsigned long *cluster){
  *cluster*=fatSectorsPerCluster;
  *cluster+=dataStart;
}
#endif

/** Return the FAT entry for cluster clusterNumber */
unsigned long GetFatEntry(unsigned long clusterNumber){
  unsigned char entryNumber; /* entry number inside page */

  if (!IS_FAT_32){ //FAT16
    entryNumber = clusterNumber & 0xff; 
    clusterNumber >>= 8; 
  }else{ //FAT32
    entryNumber = clusterNumber & 0x7f;
    clusterNumber >>= 7; 
  }
  clusterNumber += fatStart;
  if (sectorAddress.l != clusterNumber){
    sectorAddress.l = clusterNumber;
    ReadDiskSector(sectorAddress.l);
  }
  if (IS_FAT_32){
    clusterNumber=diskSect.Fat32Table[entryNumber];
    return clusterNumber;
  }
  if ((clusterNumber=diskSect.Fat16Table[entryNumber])==0xffff) return 0xffffffff;
  return clusterNumber;
}


unsigned char InitFileSystem()
{
    unsigned char c;
  
  /* Initialize variables to sane values in case of error exit. */
  fatStart = 0;
  rootStart = 0;
  dataStart = 0;

  #if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
  freeSector = 0;
  #endif
  
   /* Initialize the storage system */
  if ((c=InitStorage())){
    //Serial.print("\r\nStorage init returns error \r\n");
    //Serial.print((int)c);
    return c; /* Error in InitStorage */
  }

  //Serial.print("Storage ok.\r\nSector 0 read...");

  /* Load MBR */
  sectorAddress.l = 0; /* the first sector on disk */
  ReadDiskSector(0);


  //Serial.print("ok.\r\nSector signature...");

  /* Ok, it should be a MBR sector. Let's verify */
  if (diskSect.raw.buf[510] != 0x55)
    return 8; /* sector 0 is not MBR. */
  if (diskSect.raw.buf[511] != 0xaa)
    return 8; /* sector 0 is not MBR. */

  //Serial.print("ok.\r\nPartition 1...");

  if (!((diskSect.raw.buf[0x036]=='F')
      &&(diskSect.raw.buf[0x037]=='A')
      &&(diskSect.raw.buf[0x038]=='T')))
	{
   
	    /* This checks that partition 1 is active. Alter code to allow
    	 * other partition configurations. */
    	if (diskSect.raw.buf[0x1be] == 0x80){
      	sectorAddress.b.b0 = diskSect.raw.buf[0x1c6];
      	sectorAddress.b.b1 = diskSect.raw.buf[0x1c7];
      	sectorAddress.b.b2 = diskSect.raw.buf[0x1c8];
      	sectorAddress.b.b3 = diskSect.raw.buf[0x1c9];
      	//Serial.print (" active");
    	}
		else
		{
      		//-------- DEBUG: Uncomment if you want to explore the sector
      		//DumpDiskSector();
      		//while (!KEY_BUTTON);
      		sectorAddress.b.b0 = diskSect.raw.buf[0x1c6];
      		sectorAddress.b.b1 = diskSect.raw.buf[0x1c7];
      		sectorAddress.b.b2 = diskSect.raw.buf[0x1c8];
      		sectorAddress.b.b3 = diskSect.raw.buf[0x1c9];
    	}
    	//    return 9; /* No active partition*/
  	}

  //Serial.print(" at sector ");
  //Serial.print(sectorAddress.l,HEX);
  //Serial.print('\n');


  /* Now leave MBR and load sector 0 of partition */
  ReadDiskSector(sectorAddress.l);

  if (FatInitGlobals()){
    return 0x0a; /* FAT init failed */
  }
  return 0;
}

/** Decipher structure of FAT volume and set globals accordingly. */ 

unsigned char FatInitGlobals()
{

  memcpy(fatInfo.BS_jmpBoot, &diskSect.raw.buf[0x00],3);

  memcpy(fatInfo.BS_OEMName, &diskSect.raw.buf[0x03],8);

  memcpy(&fatInfo.BPB_BytsPerSec, &diskSect.raw.buf[0x0b],2);

  memcpy(&fatInfo.BPB_SecPerClus, &diskSect.raw.buf[0x0d],1);

 memcpy(&fatInfo.BPB_RsvdSecCnt, &diskSect.raw.buf[0x0e],2);

 memcpy(&fatInfo.BPB_NumFATs, &diskSect.raw.buf[0x10],1);

 memcpy(&fatInfo.BPB_RootEntCnt, &diskSect.raw.buf[0x11],2);

 memcpy(&fatInfo.BPB_TotSec16, &diskSect.raw.buf[0x13],2);

 if(0 == fatInfo.BPB_TotSec16 )
 {
	memcpy(&fatInfo.BPB_TotSec16, &diskSect.raw.buf[0x20],4);	
 }

 memcpy(&fatInfo.BPB_FATSz16, &diskSect.raw.buf[0x16],2);

 memcpy(&fatInfo.BPB_HiddSec, &diskSect.raw.buf[0x1c],4);

 memcpy(&fatInfo.BPB_TotSec32, &diskSect.raw.buf[0x20],4);	

memcpy(	fatInfo.ext._16.BS_VolLab, &diskSect.raw.buf[0x2b],11);	
  memcpy(fatInfo.ext._16.BS_FilSysType, &diskSect.raw.buf[0x36],8);	
  /*	
  Serial.print("\r\nFormatter signature:\r\n");
  for (temp.c=0; temp.c<8; temp.c++)
  {
    Serial.print(diskSect.fat.BS_OEMName[temp.c]);
  }

  Serial.print("\r\nBytes per sector: ");
  Serial.print(fatInfo.BPB_BytsPerSec);
  Serial.print("\r\nSectors per Cluster: ");
  Serial.print(fatInfo.BPB_SecPerClus,DEC);
  Serial.print("\r\nReserved sectors: ");
  Serial.print(fatInfo.BPB_RsvdSecCnt);
  Serial.print("\r\nNumber of FATs: ");
  Serial.print(fatInfo.BPB_NumFATs,DEC);
  Serial.print("\r\nRoot entries: ");
  Serial.print(fatInfo.BPB_RootEntCnt);
  Serial.print("\r\nTotal Sectors 16: ");
  Serial.print(fatInfo.BPB_TotSec16);
  Serial.print("\r\nFat Size 16: ");
  Serial.print(fatInfo.BPB_FATSz16);
  Serial.print("\r\nHidden Sectors: ");
  Serial.print(fatInfo.BPB_HiddSec);
  Serial.print("\r\nTotal Sectors 32: ");
  Serial.print(fatInfo.BPB_TotSec32);
  */


  /* Determine FAT Type (16/32) */
  /* This should be done better, but it'll do for now. */
  IS_FAT_32 = 1;
  if (fatInfo.BPB_RootEntCnt) 
    IS_FAT_32 = 0; /* FAT32 does not have separate root entries. */
  
  //Serial.print("\r\nFile system is ");
  if (IS_FAT_32){
    //Serial.print("FAT32\r\n");
  } else {
    //Serial.print("FAT16\r\n");
  }

  //Serial.print("\r\nFile system signature is ");
  for (temp.c=0;temp. c<8; temp.c++)
    if (IS_FAT_32){
      //Serial.print(diskSect.fat.ext._32.BS_FilSysType[temp.c]);
    } else {
      //Serial.print(fatInfo.ext._16.BS_FilSysType[temp.c]);
    }

  if (!IS_FAT_32){
    if (fatInfo.ext._16.BS_FilSysType[4]=='2'){
      IS_FAT_12 = 1;
      //Serial.print("Well, actually it's FAT12 filesystem.\r\n");
    }
  }    
  

  //Serial.print("\r\nVolume Label is ");
  for (temp.c=0; temp.c<11; temp.c++)
    if (IS_FAT_32){
      //Serial.print(diskSect.fat.ext._32.BS_VolLab[temp.c]);
    } else {
      //Serial.print(fatInfo.ext._16.BS_VolLab[temp.c]);
    }
  /* OK, let's calculate */
  /* First, let's get rid of the idea that we have byte addresses
     in the file system. Nope, let's only deal in physical disk
     sectors of 512 bytes. First we convert the FAT byter per sector
     value to "512B disk sectors per fat sector" value. */

  fatInfo.BPB_BytsPerSec /= 512;

  /* Then we adjust the Sector per Cluster to mean physical disk
     sectors. in 99% of the cases it is already so because bytes
     per sector almost always is 512 in FAT. Maximum cluster size
     is 65536 bytes (128 disk sectors). */

  fatSectorsPerCluster =
    fatInfo.BPB_SecPerClus *= fatInfo.BPB_BytsPerSec;
  /* Note: BPB_BytsPerSec has already been divided by 512 */  

  Serial.print("\r\nSectors per Cluster: ");
  Serial.print(fatSectorsPerCluster,DEC);
  
  fatStart = (unsigned long)sectorAddress.l
    + (unsigned long)fatInfo.BPB_RsvdSecCnt 
    * (unsigned long)fatInfo.BPB_BytsPerSec;

  Serial.print("\r\nFAT Start sector: ");
  Serial.print(fatStart);

  rootStart = fatInfo.BPB_FATSz16;
  if (rootStart==0){
    if (!IS_FAT_32) 
      return 0x0b; /* should be FAT32; can not find root directory */
    rootStart = diskSect.fat.ext._32.BPB_FATSz32;
  }
  rootStart *= fatInfo.BPB_NumFATs;
  rootStart *= fatInfo.BPB_BytsPerSec; /* ADJUSTED BytsPerSec! */
  rootStart += fatStart;

  Serial.print("\r\nRoot start sector: ");
  Serial.print(rootStart);

  dataStart = fatInfo.BPB_RootEntCnt >> 4;//???512/16 = 32
  dataStart += rootStart;
  dataStart -= (fatSectorsPerCluster*2); /*first cluster is cluster 2*/

  Serial.print("\r\nData start sector: ");
  Serial.print(dataStart);

  return 0;
}

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
 *
 * Additionally, if it's called with 0 as the fileNumber and it
 * happens to see an empty directory record, it registers a new
 * file with name RECnnnnn.WAV starting from cluster fragment[0].start
 * with file size from fragment[0].length
*/

unsigned char OpenFile(unsigned int fileNumber)
{
	  char tempc;

  //Serial.print("\r\nFilesystem: Looking for file ");
  //Serial.print(fileNumber,DEC);
  //Serial.print("...\r\n ");
  
  /* Start at the start of root directory. */
  dirLevel = 0; /* At root directory */
  dirStack[dirLevel].sector=rootStart;
  dirStack[dirLevel].entry=0;
  
  if (fileNumber==0){
    fileNumber = 32766; //use max-1 value for scanning for free entry
  }
 
  while (fileNumber){
    if (dirStack[dirLevel].entry==0){
      /* At the start of new dir, load first disk sector */
      while (ReadDiskSector(dirStack[dirLevel].sector)){
		InitMMC();//origin is InitMMC ???
      }
    }
    temp.c = dirStack[dirLevel].entry;

    /* We are now looking at FAT directory structure. */
    #if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
    /* Is current entry empty? */    
    if (((*(diskSect.dir+temp.c)).entry.Name[0]==0)
	||(diskSect.dir[temp.c].entry.Name[0]==0xe5)){      
      /* Yes. Are we supposed to make an entry now? */
      if (fileNumber>30000){ 
	//yes, this is actually a request to write file directory entry!;

	fileNumber = 32767-fileNumber;

	fileNumber = fragment[0].start;
	
	
	diskSect.dir[temp.c].entry.Name[0] ='R';
	diskSect.dir[temp.c].entry.Name[1] ='E';
	diskSect.dir[temp.c].entry.Name[2] ='C';
	diskSect.dir[temp.c].entry.Name[7] =(fileNumber%10)+'0'; 
	fileNumber /= 10;
	diskSect.dir[temp.c].entry.Name[6] =(fileNumber%10)+'0';
	fileNumber /= 10;
	diskSect.dir[temp.c].entry.Name[5] =(fileNumber%10)+'0';
	fileNumber /= 10;
	diskSect.dir[temp.c].entry.Name[4] =(fileNumber%10)+'0';
	fileNumber /= 10;
	diskSect.dir[temp.c].entry.Name[3] =(fileNumber%10)+'0';
	diskSect.dir[temp.c].entry.Name[8] ='O';
	diskSect.dir[temp.c].entry.Name[9] ='G';
	diskSect.dir[temp.c].entry.Name[10]='G';
	diskSect.dir[temp.c].entry.Attr = 0;
	diskSect.dir[temp.c].entry.FstClusHi = (fragment[0].start >> 16);
	diskSect.dir[temp.c].entry.FstClusLo = (fragment[0].start & 0xffff);
	diskSect.dir[temp.c].entry.FileSize = (fragment[0].length);

	Serial.println(" \r\nMaking Directory Entry");
	WriteDiskSector(sectorAddress.l);
	
	return 99; //created a FAT entry
      } //register file
    }
    #endif

    /* Does current entry point to  a regular file? */
    /* Attributes: NO directory, NO volume id, NO system, NO hidden */
    if (((diskSect.dir[temp.c].entry.Attr & 222) == 0) 
	&& (diskSect.dir[temp.c].entry.Name[0] != 0xe5) 
	&& (diskSect.dir[temp.c].entry.Name[0] != 0)  ){
      
      /* It is a regular file. */
      if (!(--fileNumber)){

	/* ------------ FILE FOUND ------------- */

	sectorAddress.l = 
	  ((unsigned long)diskSect.dir[temp.c].entry.FstClusHi<<16) 
	  + diskSect.dir[temp.c].entry.FstClusLo;
	sectorAddress.l *= fatSectorsPerCluster;
       	sectorAddress.l += dataStart;
	
	fileSize = diskSect.dir[temp.c].entry.FileSize;
      	dataBufPtr = 0; /* Reset data buffer ptr for FGetChar */

	Serial.print("\r\n\r\nFound file name is ");
       	//for (tempc=0; tempc<11; tempc++){
	  //Serial.print(diskSect.dir[temp.c].entry.Name[tempc]);
	//}
	Serial.write(diskSect.dir[temp.c].entry.Name,11);

	//Store file name nicely for displaying on screen :)
       	for (tempc=0; tempc<8; tempc++){
	  currentFileName[tempc]=diskSect.dir[temp.c].entry.Name[tempc];
	}
	currentFileName[8]='.';
	currentFileName[9]=diskSect.dir[temp.c].entry.Name[8];
	currentFileName[10]=diskSect.dir[temp.c].entry.Name[9];
	currentFileName[11]=diskSect.dir[temp.c].entry.Name[10];
	///displayText[0]=' ';
	////for (tempc=0; tempc<12; tempc++){
	  ///displayText[tempc+1]=currentFileName[tempc];
	///}

	Serial.print("\r\nFile size: ");
	Serial.print(fileSize,DEC);
	Serial.print("bytes. \r\n");

	/*Serial.print("Start cluster: ");
	Serial.print(diskSect.dir[temp.c].entry.FstClusHi,HEX);
 	Serial.print(diskSect.dir[temp.c].entry.FstClusLo,HEX);
	Serial.print("h, sector ");
	Serial.print(sectorAddress.l,DEC);
	Serial.print("decimal.\r\n");
	*/
	return 0; /* File found, All OK return */
      }
    } /* it was a regular file */
    


    /* Is it a subdirectory? */
    if (((diskSect.dir[temp.c].entry.Attr & 16) != 0) //0x10 subdirectory
        &&((diskSect.dir[temp.c].entry.Attr & 0x02) != 0x02)//Hidden
	&& (diskSect.dir[temp.c].entry.Name[0] != '.') /* skip . and .. */ 
	&& (diskSect.dir[temp.c].entry.Name[0] != 0xe5) 
	&& (diskSect.dir[temp.c].entry.Name[0] != 0)  ){

      /* It is a subdirectory. */

      if (dirLevel<MAX_NUMBER_SUBDIRECTORIES-1){
	/* Yes, we have room in dirStack to traverse deeper. */
	dirLevel++; /* Advance to next directory level */	
	sectorAddress.l = 
	  ((unsigned long)diskSect.dir[temp.c].entry.FstClusHi<<16) 
	  + diskSect.dir[temp.c].entry.FstClusLo;
	sectorAddress.l *= fatSectorsPerCluster;
	sectorAddress.l += dataStart;
      
	/* Prepare for loading. */
	dirStack[dirLevel].sector = sectorAddress.l;
	dirStack[dirLevel].entry = 255; /* Magic number */
		
      } /* we had room in dirStack */      
    } /* it was a subdirectory */
      

    /* Have we reached the end of the directory? */
    if (diskSect.dir[temp.c].entry.Name[0] == 0){
      /* It's the end of directory. */

      /* Is it the end of root directory? */
      if (dirLevel == 0){
	/* End of root directory, end of all files in volume */
	Serial.print("\r\nFile not found.\r\n");
	sectorAddress.l = dataStart; 
	/* when in error point to start of data */
	return 0x0c; /* File Not Found return */
      } 

      /* End of subdirectory, return from subdirectory */
      dirLevel--;
      ReadDiskSector(dirStack[dirLevel].sector);
      /* restore temp entry pointer */
      temp.c = dirStack[dirLevel].entry;

    } /* it was end of directory */


    /* Advance to next entry */
    temp.c++;
    
    /* If we just went to a subdir, set temp entry pointer to 0 */
    if (dirStack[dirLevel].entry == 255){
      /* Magic Number 255: we have gone to a subdirectory */
      temp.c=0;
    }
      
    if (temp.c==16){ /* End of sector */
      /* Prepare to load next sector */
      dirStack[dirLevel].sector = GetNextSector (dirStack[dirLevel].sector);
      temp.c=0;
    }

    dirStack[dirLevel].entry = temp.c;
  }

  /* Control should never reach this far, end of root directory should
     occur first. */

  sectorAddress.l = dataStart; /* when in error point to start of data */
  return 0x0c; /* File Not Found return */

}

/** Build a fragment table starting from current sector. 
 * Returns number of fragments in song. 
 * This function is used to get fast access to the filesystem when
 * playing so cluster chain needs not be followed when the song
 * is playing, that would be too slow with MMC cards without 
 * extra buffer memory for the file allocation tables.
 *
 * Note: filesys.c module does not use the fragment table internally.
 * it is written to be a service to the player routine, which
 * uses storage.c module directly for disk access after using the
 * filesys.c module for finding a file to play. I am listening to
 * Darude's Sandstorm while coding this. 
 *
 * In terms of memory allocation, this function is devilish. At one stage
 * temp.l is used just to make compiler use memory at temp.l instead
 * of spilling to another unnecessary temp variable...
 */

unsigned char BuildFragmentTable(){

  unsigned char c=0;
  addressType cthis,next;
  addressType fatSector;
  unsigned char entryNumber;


  if (IS_FAT_12){
    Serial.print("\r\nSingle fragment support for FAT12\r\n");
    fragment[0].start = sectorAddress.l;
    fragment[0].length = fileSize/512;
    return(1); //Return 1 fragment
  }


  //as long as the sectorAddress is "sane"...
  while (!(sectorAddress.b.b3 & 0x80)){
        
    fragment[c].start = sectorAddress.l;
    fragment[c].length = fatSectorsPerCluster;
    
    /* Find cluster entry for the current sector */
    
    /* in how manyth sector from start of data area are we? ->this */
    cthis.l = sectorAddress.l - dataStart;
    /* convert from CurrentSectorN to currentClusterN */
    cthis.l /= fatSectorsPerCluster;
    /* this.l is now the current cluster number */
    
    
    /* now let's find the FAT entry for this.l cluster */
    if (!IS_FAT_32){ //FAT16
      entryNumber = cthis.b.b0; /* 256 entries / page in FAT16 table */
      fatSector.l = cthis.l >> 8; /* Div by n of entries/page in FAT16 tbl*/
    }else{ //FAT32
      entryNumber = (cthis.b.b0 & 0x7f); /* 128 entries/page in FAT32 table */
      fatSector.l = cthis.l >> 7; /* Div by n of entries/page in FAT32 tbl*/
    }
    fatSector.l += fatStart;
    /* fatSector.l is now the DISK SECTOR NUMBER CONTAINING THE FAT table */
    /* read this page into memory */
    ReadDiskSector(fatSector.l);    
    
    if (!IS_FAT_32){ //FAT16
      next.l = diskSect.Fat16Table[entryNumber]; /* get next cluster n */
      /* Check for FAT16 end-of-file condition */
      if ((next.b.b1 == 0xff) && (next.b.b0 == 0xff)){
	/* FAT16 End of file */
	next.b.b3 = 0xff; /* return a large value (fat32 compatible) */
      }
    }else{ //FAT32
      next.l = diskSect.Fat32Table[entryNumber]&0x0fffffff;
    }
    /* next.l is the FAT entry (next cluster number) */
    
    
    /*Serial.print("Fragment start: cluster ");
    Serial.print(cthis.l,HEX);
    Serial.print(", sector ");
    Serial.print(fragment[c].start,DEC);
	*/
    while (next.l==(cthis.l+1)){
      //Not a fragment break -- continue to next entry 

      //in this temp.l is used only to allow compiler memory spilling to temp
      temp.l = fragment[c].length;
      temp.l += fatSectorsPerCluster;
      fragment[c].length = temp.l;

      entryNumber++;

      // --- Check for a page break
      if (entryNumber==0){ //entryNumber has rolled over!!!! (8bit char)
	//FAT16 table page border is reached
	fatSector.l++; //Advance to next page;
	entryNumber=0;
	ReadDiskSector(fatSector.l);
      }
      if (IS_FAT_32 && (entryNumber==128)){
	//FAT32 table page border is reached
	fatSector.l++;
	entryNumber=0;
	ReadDiskSector(fatSector.l);
      }
      
      // --- Advance to next cluster
      cthis.l = next.l;
      
      if (!IS_FAT_32){
	//FAT16 get next cluster n
	next.l = diskSect.Fat16Table[entryNumber];       
	if ((next.b.b1==0xff)&&(next.b.b0==0xff)){
	  //FAT16 end-of-file
	  next.b.b3 = 0xff; //mark end-of-file (FAT32 compatible)
	}
      }else{
	//FAT32 get next cluster n
	next.l = diskSect.Fat32Table[entryNumber];
      }
    }//This repeats until there is a discontinuity
    
    /* next.l now has the cluster entry for last cluster in fragment
       it has a high value if eof (see FAT spec) */
	if( fragment[c].length > (fileSize>>9))
    fragment[c].length = (fileSize>>9)+1;//aim to reduce unessary track data--add by Icing, 
    
    /*Serial.print("\r\nSize: ");
    Serial.print(fragment[c].length<<9,DEC);
    Serial.println("sectors.");
	*/
    //EOF test
    if ((next.b.b3&0x08) //Quick test
	&& ((next.l>0x0ffffff0) | !IS_FAT_32)){ //Complete test
      //EOF
      //Serial.print(" <EOF>\r\n");
      sectorAddress.b.b3 = 0xff;
    }else{
      
      //Determine next physical sector for next fragment
      sectorAddress.l = next.l;
      sectorAddress.l *= fatSectorsPerCluster;
      sectorAddress.l += dataStart;
    }
    
    c++; //Advance to next fragment number
    
    if (c==MAX_NUMBER_FRAGMENTS){
      //End of RAM space allocated for fragment table
      //Force end-of-file
      sectorAddress.b.b3=0xff;
    }
    
  }//break or continue to next cluster
  
  return c; //Return number of fragments;
}

/** Calculate next sector from currentSector.
 *  Return sector address with MSB set if end of file. 
 *
 * This is the essence of FAT filesystem: traversing through
 * the file allocation table to scan for cluster chains.
*/

unsigned long GetNextSector(unsigned long currentSector){
  addressType fatSector;
  unsigned char entryNumber;
  
  /* how manyth sector from start of data area -> fatsector */
  fatSector.l = currentSector - dataStart;
  
  /* Check for cluster boundary */
  /* if we're not in cluster boundary then return currentsector + 1 */
  if ((fatSector.b.b0 + 1) % fatSectorsPerCluster){
    return currentSector + 1;
  }
  
  /* is the value negative? then return currentSector + 1 */
  if (fatSector.b.b3 & 0x80) {
    return currentSector + 1;
  }    

  /* The trivial conditions are not met, we actually do need to 
     look up the next sector info from FAT tables in the disk.*/

  /* convert fatSector from CurrentSectorN to currentClusterN */
  fatSector.l /= fatSectorsPerCluster;

  /* calculate which page (fatSector) and entry of FAT table correspond
     to current sector */
   
  if (!IS_FAT_32){ //FAT16
    entryNumber = fatSector.b.b0; /* 256 entries / page in FAT16 table */
    fatSector.l >>= 8; /* Divide by 256 (n of entries/page in FAT32 table */
  }else{ //FAT32
    entryNumber =(fatSector.b.b0 & 0x7f); /* 128 entries/page in FAT32 tbl */
    fatSector.l >>= 7; /* Divide by 128 (n of entries/sector in FAT32 table */
  }
  
  /* Get sector address for the needed page */
  fatSector.l += fatStart;
  
  /* Load (if needed) table page from disk */
  if (sectorAddress.l != fatSector.l){
    sectorAddress.l = fatSector.l; /* (not needed because mmc.c does this) */
    ReadDiskSector(fatSector.l);
    
    Serial.print('n');
    
    
  }
  /* Now there should be correct FAT entry page in memory... */
  
  if (!IS_FAT_32){ //FAT16
    fatSector.l = diskSect.Fat16Table[entryNumber];
    /* Check for FAT16 end-of-file condition */
    if ((fatSector.b.b1 == 0xff) && (fatSector.b.b0 == 0xff)){
      /* End of file */
      fatSector.b.b3 = 0xff; /* return a large value (fat32 compatible) */
      return fatSector.l;
    }
  }else{ //FAT32
    fatSector.l = diskSect.Fat32Table[entryNumber] & 0x0fffffff;
    /* Check for FAT32 end-of-file condition */
    if ((fatSector.b.b3&0x08) //Quick test
	&& (fatSector.l>0x0ffffff0)){ //Complete test
      /* End of file */
      return 0xffffffff; /* return EOF*/
    }
  }
  

  /* now fatSector.l contains the proper next cluster value */

  fatSector.l *= fatSectorsPerCluster;
  /* now fatSector.l contains the next sector value */

  fatSector.l += dataStart;
  /* now fatSector.l contains the proper absolute sector address */

  return fatSector.l;
}





#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
/** Search for next free sector.
 * If freeSector is zero, a new file should be allocated. */    
unsigned char ScanForFreeSector(){

  //ConsoleWrite("\rLooking for free space starting from ");

  if ((freeSector) && (freeSector-dataStart+1)%fatSectorsPerCluster){
    //Still room in current cluster...
    freeSector++;

    //ConsoleWrite("free at sector ");
    //ConsolePutUInt(freeSector);

    return 1;
  }  
  
  if (freeSector){
    //Advance to next sector
    freeSector++;
    //switch from using sectors to using clusters
    PConvertSectorToCluster(&freeSector);
  }else{
    freeSector=3; //starting cluster entry (should be 2?)
  }


  //scan until free entry is found
  //freeSector actually counts clusters here.
  while(GetFatEntry(freeSector)){    
    freeSector++;
  }

  //ConsoleWrite("F");
  //ConsolePutUInt(freeSector);

  
  //while (!KEY_BUTTON);
  //ConsoleWrite("free at cluster ");
  //ConsolePutUInt(freeSector);

  //switch back from using clusters to using sectors.
  PConvertClusterToSector(&freeSector);

  //ConsoleWrite("sector ");
  //ConsolePutUInt(freeSector);

  return 1;
}
#endif




/** Write a cluster chain to FAT for a fragment.

    This takes its input from the fragment[] table. It currently has
    the ability to register only single fragment from fragment[0].
    fragment[1].start should contain the next cluster number
    after this fragment or 0x0fffffff if this is the last fragment.
    
    This function currently also registers a FAT directory entry for
    the file to first free entry slot in the directory structure.
    If no free directory entries are available, it will behave
    unpredictably. This could happen in FAT32.

    OpenFile(0) is a special call to register a directory entry for
    fragment[0].

*/

#if defined(__AVR_ATmega1280__)|| defined(__AVR_ATmega2560__)
void WriteClusterChain(){

   unsigned long currentCluster;    //is now fragment[0].start
   unsigned long fragSize;          //is now fragment[0].length
   unsigned long fatPageSector;
   unsigned char entryNumber; 
  

  // PREPARE

  // Calculate starting cluster number...
  currentCluster = fragment[0].start;
  PConvertSectorToCluster(&currentCluster);
  
  fragSize = fragment[0].length;         //size in sectors

  // Write cluster number and size in bytes to fragment[0]
  // for the function that registers directory entries
  fragment[0].start = currentCluster;
  fragment[0].length *= 512;

  // Locate the relevant page in FAT clusterchain records
  fatPageSector = currentCluster;
  if (!IS_FAT_32){ //FAT16
    entryNumber = fatPageSector & 0xff; 
    fatPageSector >>= 8; 
  }else{ //FAT32
    entryNumber = fatPageSector & 0x7f;
    fatPageSector >>= 7; 
  }
  fatPageSector += fatStart;
     
  Serial.print("\r\nWriting clusterchain from: "); Serial.print(currentCluster,DEC);
  Serial.print("\r\nFragment size (sectors): "); Serial.print(fragSize),DEC;
  Serial.print("\r\nFatPageSector: "); Serial.print(fatPageSector,DEC);
  Serial.print("\r\nEntryNumber: "); Serial.print(entryNumber,DEC);

  
  // WRITE CLUSTER CHAIN TO FIRST FAT

  while (fragSize>fatSectorsPerCluster){ 
    // while more than 1 cluster left...

    // is the FAT clusterchain page changed?
    if (sectorAddress.l != fatPageSector){
      // Yes, have we advanced by 1 sector?
      if (sectorAddress.l == (fatPageSector - 1)){
	//Yes, we must save the edited sector
	WriteDiskSector (sectorAddress.l);
	///\todo update second FAT too	  
      }
      // at least we need to load the new clusterchain page
      sectorAddress.l = fatPageSector;
      ReadDiskSector(sectorAddress.l);
    }//Load/Save FAT sector

    
    // Register Middle Entry
   currentCluster++; //advancce to next cluster
    
    //fragSize-=fatSectorsPerCluster; //we will register one cluster
    //to avoid memory spill in C compiler, the preceding line is written as:
    for (temp.c = fatSectorsPerCluster;temp.c;temp.c--){
      fragSize--;
    }
   
    if (IS_FAT_32){
      diskSect.Fat32Table[entryNumber]=currentCluster;
    }else{
      diskSect.Fat16Table[entryNumber]=(currentCluster&0xffff);
    }      

    // Calculate next entry position in FAT records
    fatPageSector = currentCluster;
    if (!IS_FAT_32){ //FAT16
      entryNumber = fatPageSector & 0xff; 
      fatPageSector >>= 8; 
    }else{ //FAT32
      entryNumber = fatPageSector & 0x7f;
      fatPageSector >>= 7; 
    }
    fatPageSector += fatStart;

  }//while


  // WRITE THE FINAL ENTRY TO FAT CLUSTERCHAIN

  // is the FAT clusterchain page changed?
  if (sectorAddress.l != fatPageSector){
    // Yes, have we advanced by 1 sector?
    if (sectorAddress.l == (fatPageSector - 1)){
      //Yes, we must save the edited sector
      WriteDiskSector (sectorAddress.l);
      ///\todo update second FAT too	  
	}
    // at least we need to load the new clusterchain page
    sectorAddress.l = fatPageSector;
    ReadDiskSector(sectorAddress.l);
  }//Load/Save FAT sector
  

  //Write the Last entry
  if (IS_FAT_32){
    diskSect.Fat32Table[entryNumber]=fragment[1].start;
  }else{
    diskSect.Fat16Table[entryNumber]=(fragment[1].start & 0xffff);
  }    
  WriteDiskSector(sectorAddress.l); //Write last sector
    
  OpenFile(0); //Register FAT entry

}

#endif






