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
***********************************

music shield firmware v1.14
change log:
Add the code like "||defined(__AVR_ATmega2560__)" behind the code "#if defined(__AVR_ATmega1280__)" in all files of the previous demo code "music_v1_13".
Compatiable with Arduino MEGA2560.

2011-8-10 17:00:00 Piggy