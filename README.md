# Stop-Watch
Stop Watch system that is implemented using c language and ATMEGA-32.
Description:-
• This project is based on AVR ATmega32 with frequency 1Mhz. 
• Timer1 CTC mode configuration is used to count the Stop Watch time. (1 TIMER1_COMPA interrupt = 1 second) 
• Six Common Anode 7-segments is connected using multiplexed technique.
• One 7447 decoder is used for all the six 7-segments controlling the enable/disable for each one using round robin technique.
• Reset button is connected to External Interrupt INT0 using internal pull-up resistor. 
• External Interrupt INT0 is configured with falling edge. • Pause button is connected to External Interrupt INT1 using external pull-down resistor.
• External Interrupt INT1 is configured with raising edge. • Resume button is connected to External Interrupt INT2 using internal pull-up resistor. 
• External Interrupt INT2 is configured with falling edge.
