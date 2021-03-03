# BoardForth
A token-threaded implementation of a Machine-Forth like VM for development boards, written in C.

It supports automatically running a user-defined word during the loop() phase of the arduino interface.

    - Of course, that 'word' can be as complex as needed.
    - This is done by setting address [0:1] to the (16-bit) BODY (XT) of the desired word.
    - Similarly, setting address [0:1] to 0 will disable automatic running.
    - The easiest way to do this is to use the 'auto-run-last' and 'auto-run-off' words:
        - e.g. ... : auto-run-last LAST >BODY 0 A! ;
        - e.g. ... : auto-run-off 0 0 A! ;
    - The only rule is that the stacks need to end up with the same depth when done.

To build this VM for a given board:

    - Open the Arduino software IDE.
    - Load file bf/bf.ino.
    - Select the proper COM port and target board.
    - Change the following #defines in defs.h to appropriate values for the target board.
        - #define DICT_SZ (24*1024)
        - #define STK_SZ 32
        - #define TIB_SZ 0x0080
        - #define ALLOC_SZ 32
    - Press the [Verify] button (<ctrl>-R).
    
To deploy the VM to the development board:

    - Follow the steps from above, but use the [Upload] button (<ctrl>_U) instead.

Notes:

    - This is a work in progress. I welcome collaboration.
    - This is not an ANSI standard Forth.
    - However, one could build an ANSI standard Forth using it if so desired.
    - The data and return stack sizes are defaulted to 32 CELLS (32-bit). That can be easily changed in board.h
    - Addresses are 16-bit values, allowing for up to 64k of dictionary space. That is an awful lot of code.
    - To add an opcode (primitive):
        - Add a '#define XXX nnn // kkk' to the list in defs.h.
        - Create a function 'void fXXX() { ... }'  (usually in prims.cpp).
        - Put that function 'fXXX' into the big array of function pointers statement in prims.cpp.
        - Add an entry to the list of 'if ((strcmp_FP(w, PSTR("kkk") == 0) return OP_XXX;' lines in opcodes.cpp.
        - There is a nimbleText script to generate stubs for this. See nt-nt.txt
    - The development process I use is as follows:
        - Deploy the VM as above.
        - Open up Serial Monitor.
        - Interactively code as usual, including defining new words as appropriate.
        - When I know what I want to keep, I update the LoadUserWords() with any new code.
        - If there is anything from the interactive session I want to use, I copy it from the SerialMonitor.
        - Rebuild and upload the project to the board, and the new functionality is now "built-in".

To control a LED connected to pin A4, you can do this:

    - : led 4 ; led output    \ name the LED at pin A4 and makes it an OUTPUT pin
    - : led-on 1 led dp! ;
    - : led-off 0 led dp! ;
    - led-on                  \ the LED turns on
    - led-off                 \ the LED turns off

The standard 'blink' example program:

    - : i-led #13 ; i-led output       \ the internal LED is usually pin #13
    - variable speed 500 speed !
    - : blink 1 i-led dp! speed @ ms 0 i-led dp! speed @ ms ;
    - auto-run-last                    \ the LED starts blining with a 500 ms interval
    - auto-run-off                     \ the LED stops blining

To read a switch connected to pin D36, do this:

    - : sw 36 ; sw input
    - sw dp@ .

To read a potentiometer connected to pin A8, do this:

    - : pot 8 ;
    - pot input
    - pot ap@ .

To turn on the LED depending on the value of the above switch:

    - : sw->led sw dp@ led dp! ;       \ this will turn the LED on or off depending on the switch at pin 36
    - last >body auto-run              \ now when you flip the switch, the LED turns on or off
    - 0 auto-run                       \ the LED no longer changes when the switch is flipped
