# BoardForth
A Forth VM for development boards and the PC, written in C.

BoardForth supports automatically running a user-defined word during the loop() phase of the arduino interface.

    - Of course, that 'word' can be as complex as needed.
    - This is done by setting the beginning of DICT to the XT of the desired word.
    - Similarly, setting DICT to 0 will disable automatic running.
    - The easiest way to do this is to use the 'auto-run-last' and 'auto-run-off' words:
        - e.g. ... : auto-run-last LAST >BODY dict A! ;
        - e.g. ... : auto-run-off 0 dict A! ;
    - The only rule is that the data and return stacks need to end up with the same depth when done.

See the file 'reference.txt' for details about the words and primitives in BoardForth.

To build BoardForth under Windows:

	- Build the bf/bf.sln project using Visual Studio (the Community version is fine)

To build BoardForth for a given development board:

    - Open the Arduino software IDE.
    - Load file bf/bf.ino.
    - Select the proper COM port and target board.
    - Change the following #defines in defs.h to appropriate values for the target board (I use these for the XIAO).
        - #define DICT_SZ (24*1024)
        - #define STK_SZ 32
        - #define TIB_SZ 0x0080
    - To include HID device support, uncomment out the #define __GAMEPAD__ line
        - Note that this requires that the HID-Project is installed (use Sketch->Library to install it)
    - Set the #define theSerial appropriately. Some boards use Serial, others use SerialUSB.
    - Press the [Verify] button (<ctrl>-R).
    
To deploy the BoardForth to the development board:

    - Follow the steps from above, but use the [Upload] button (<ctrl>-U) instead.

Notes:

    - This is not an ANSI standard Forth.
    - This is a work in progress. I welcome collaboration.
    - Words are not case sensitive. DICT, Dict, and dict all refer to the same word.
    - The data and return stack sizes are defaulted to 32 CELLS (32-bit). That can be easily changed in board.h
    - To add an opcode (primitive):
        - Add a 'X("NEWOP", NEWOP, code) \' to the BASE_OPCODES list in defs.h.
        - If the code is more involved, define a function and call that.
    - The development process I use for development boards is as follows:
        - Deploy BoardForth as above.
        - Open up Serial Monitor or PuTTY on that COM port.
        - Interactively code as usual, including defining new words as appropriate.
        - When I know what I want to keep, I update SOURCE_USER with any new code.
        - If there is anything from the interactive session I want to use, I copy it from the session.
        - Re-deploy the project. Now the new functionality is now "built-in".

To control a LED connected to pin 4, you can do this:

    - : HIGH 1 swap pin! ;
    - : LOW 0 swap pin! ;
    - variable (led) 4 (led) !
    - : led (led) @ ; 
    - led output
    - : led-on led HIGH ;     \ NOTE: some boards turn on the internal LED when LOW
    - : led-off led LOW ;
    - led-on                  \ the LED turns on
    - led-off                 \ the LED turns off

The standard 'blink' example program:

    - #13 (led) ! led output
    - variable (waitTime) 250 (waitTime) !
    - : delay (waitTime) @ ms ;
    - : blink led-on delay led-off delay ;
    - auto-run-last                    \ the LED starts blining with a 500 ms interval
    - 100 (speed) !                    \ the LED speeds up
    - 500 (speed) !                    \ the LED slows down
    - auto-run-off                     \ the LED stops blining

To read a switch connected to pin 36, you can do this:

    - : sw 36 ; sw input
    - sw pin@ .
	- Note: if you use 'sw pullup', the pin value is 1 when open, and 0 when shorted to ground

To read a potentiometer connected to pin 8, you can do this:

    - : pot 8 ; pot input
    - pot apin@ .

To turn on the LED depending on the value of the above switch:

    - : sw->led sw pin@ led pin! ;     \ this will turn the LED on or off depending on the switch at pin 36
    - auto-run-on                      \ now when you flip the switch, the LED turns on or off
    - 0 auto-run                       \ the LED no longer changes when the switch is flipped
