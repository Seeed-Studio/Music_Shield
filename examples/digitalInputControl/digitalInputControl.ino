/*
    digitalInputControl.ino
    Example sketch for MusicShield 2.0

    Copyright (c) 2012 seeed technology inc.
    Website    : www.seeed.cc
    Author     : Jack Shao (jacky.shaoxg@gmail.com)
    Create Time: Mar 2014
    Change Log :

    The MIT License (MIT)

    Permission is hereby granted, free of charge, to any person obtaining a copy
    of this software and associated documentation files (the "Software"), to deal
    in the Software without restriction, including without limitation the rights
    to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
    copies of the Software, and to permit persons to whom the Software is
    furnished to do so, subject to the following conditions:

    The above copyright notice and this permission notice shall be included in
    all copies or substantial portions of the Software.

    THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
    IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
    FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
    AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
    LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
    OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
    THE SOFTWARE.
*/


#include <SD.h>
#include <SPI.h>
#include <arduino.h>
#include <MusicPlayer.h>

void setup(void) {
    Serial.begin(9600);
    player.keyDisable(); //keys disable first;
    player.digitalControlEnable();
    player.begin();
    //player.attachDigitOperation(2,playNextSong,HIGH);
    //player.attachDigitOperation(2,playPause,HIGH);
    player.attachDigitOperation(2, volumeUp, HIGH); //the button connected to pin2,and HIGH leval is valid
    player.setPlayMode(PM_NORMAL_PLAY);
    player.scanAndPlayAll(); //If the current playlist is empty,it will add all the songs in the root directory to the playlist.
}
void loop() {
    player.play();
}
void playNextSong() { //User-defined function
    player.opNextSong();
}
void playPause() { //User-defined function
    static boolean flag_on = 1;
    if (flag_on) {
        player.opPlay();
    } else {
        player.opPause();
    }
    flag_on = !flag_on;
}
void volumeUp() { //User-defined function
    player.opVolumeUp();
}
