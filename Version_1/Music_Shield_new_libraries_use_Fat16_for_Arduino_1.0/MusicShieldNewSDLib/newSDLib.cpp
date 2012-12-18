//  File newSDLib.cpp
//  Functions for using FAT16 lib
//  For more details about the product please check http://www.seeedstudio.com/depot/

//  Copyright (c) 2010 seeed technology inc.
//  Author: Icing Chang
//  Version: 1.0
//  Time: Oct 21, 2010
//  Changing records:
//    
//  This library is free software; you can redistribute it and/or
//  modify it under the terms of the GNU Lesser General Public
//  License as published by the Free Software Foundation; either
//  version 2.1 of the License, or (at your option) any later version.
//
//  This library is distributed in the hope that it will be useful,
//  but WITHOUT ANY WARRANTY; without even the implied warranty of
//  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
//  Lesser General Public License for more details.
//
//  You should have received a copy of the GNU Lesser General Public
//  License along with this library; if not, write to the Free Software
//  Foundation, Inc., 51 Franklin St, Fifth Floor, Boston, MA  02110-1301  USA

#include <Fat16.h>
#include <Fat16Util.h>
#include "newSDLib.h"
#include "vs10xx.h"
#include "config.h"

SdCard card;
Fat16 file;

void error_P(const char* str) {
  PgmPrint("error: ");
  SerialPrintln_P(str);
  if (card.errorCode) {
    PgmPrint("SD error: ");
    Serial.print(card.errorCode, HEX);
  }
  //while(1);
}

int initialSDCard()
{
  //SPI speed: 0 - F_CPU/2, 1 - F_CPU/4
  if (!card.init(0,SD_CS_PIN))
  {
    error("card.init failed");
    return 0;
  }
  // initialize a FAT16 volume
  if (!Fat16::init(&card))
  {
    error("Fat16::init");
    return 0;
  }
  return 1;
}

int openFile(char *fileName)
{

  if (file.open(fileName, O_READ)) {
    Serial.write(fileName);
    Serial.println(" opened");
    return 1;
  }
  else{
    error("open file failed");
    return 0;
  }
}

int openFile(uint16_t index)
{

  if (file.open(index, O_READ)) {
    Serial.print(index);
    Serial.println(" opened");
    return 1;
  }
  else{
    error("open file failed");
    return 0;
  }
}

int readFile(byte *buffer, int len)
{
  int readLen = 0;
  readLen = file.read(buffer,len);
  return readLen;
}




