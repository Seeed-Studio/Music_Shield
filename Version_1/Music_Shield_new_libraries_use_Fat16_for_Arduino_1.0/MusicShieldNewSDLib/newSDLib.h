//  File newSDLib.h
//  Headers for using FAT16 lib
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

#ifndef NEWSDLIB_H
#define NEWSDLIB_H

extern SdCard card;
extern Fat16 file;

// store error strings in flash to save RAM
#define error(s) error_P(PSTR(s))

int initialSDCard();
int openFile(char *fileName);
int openFile(uint16_t index);
int readFile(byte *buffer, int len);
void error_P(const char* str) ;
#endif

