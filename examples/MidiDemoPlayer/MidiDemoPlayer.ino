#include <SD.h>
#include <SPI.h>
#include <MusicPlayer.h>
void setup() {
    // put your setup code here, to run once:
    SERIAL.begin(9600);
    player.beginInMidiFmt();
}

void loop() {
    // put your main code here, to run repeatedly:
    player.midiDemoPlayer();
    delay(500);
}
