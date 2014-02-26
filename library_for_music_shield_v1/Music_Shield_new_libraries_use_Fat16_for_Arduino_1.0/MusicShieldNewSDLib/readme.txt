Copyright (c) 2010 Seedstudio.  All right reserved.

music shield v1.0

Initial version.
Features:
1. support ATMega328,1280
2. support FAT16 MicroSD card
3. support playing Ogg, mp3,wma,MIDI.
4. support recording as Ogg
5. Some MircroSD card may not support.

2010-6-11 9:45:56 Icing
***********************************

music shield firmware v1.13
changing log:	
1. Add the setup of I2S function of vs1053
2. Remove some debug info 
3. Add the initialization for the buttons of internal pull high	
4. Change SPI speed to cpu/4 when playing music, and in other circumstance to cpu/16. As in cpu/64,recording is not working well.

2010-8-31 15:14:20 Icing
*************************************
music shield new sd library 
1.Base on adrduino library Fat16
2.Play a song named test.mp3 in the SD card
3.Keys include Pause/Play,volume up and down are available
4.Compatible with Seeeduino v2.21(328P),Seeeduino Mega and Arduino Mega2560

2011-9-15 9:30:00 Piggy
**************************************
Arduino 1.0 library for Music shield

2012-3-2 17:00:00 Frankie
**************************************


