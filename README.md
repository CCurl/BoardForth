# BoardForth
A token-threaded implementation of a Machine-Forth like VM for development boards, written in C.

It supports automatically running a user-defined word during the loop() phase of the arduino interface.

    - Of course, that 'word' can be as complex as needed.
    - This is done by setting address [0:1] to the (16-bit) XT of the desired word.
    - Similarly, setting address [0:1] to 0 will disable automatic running.
    - The easiest way to do this is to use the 'auto-run-last' and 'auto-run-off' words:
        - e.g. ... : auto-run-last LAST ->XT 0 W! ;
        - e.g. ... : auto-run-off 0 0 W! ;
    - The only rule is that the stacks need to end up with the same depth when done.

To build this VM for a given board:

    - open the Arduino software IDE
    - load file vm/vm.ino
    - select the proper COM port and target board
    - Change the #define in vm.h to an appropriate value or add a new section for the target board
        - The header currently has entries for:
            - LEONARDO (2.5k SRAM, 1.5k dicitionary - pretty much worthless)
            - ATMEGA_2560 (8k SRAM, 6k dictionary)
            - REDBOARD_TURBO (32K SRAM, 28k dictionary)
            - XIAO (32K SRAM, 28k dictionary)
    - press the [Verify] button
    
Notes:

    - This is not an ANSI standard Forth.
    - However, an ANSI standard forth can be built using it if so desired.
    - The data and return stacks are set to 15 CELLS (32-bit). That can be easily changed in vm.h
    - Addresses are 16-bit values, allowing for up to 64k of dictionary space. That is an awful lot of code.
    - To add a primitive:
        - Add a #define to the list in vm.h
        - Update keywords.ino
        - Put a case into the switch statement in vm.ino.
        - There is a nimbleText script to help with this. See NimbleText.h
    - The development process I use is as follows:
        - Deploy the VM as above.
        - Open up Serial Monitor.
        - Interactively code as usual, including defining new words as appropriate.
        - When I know what I want to keep, I update the FLASH(nnn) section in XXX.ino.
        - If i have anything in the interactive log I want to use, I copy and paste it.
        - Rebuild and upload the project to the board, and the new functinality is now "built-in".

To deploy the VM to the development board:

    - follow the steps from above, but use the [Upload] button instead

To turn on the LED at pin 13, do this:

    - : pin! $10000 + ! ;
    - 1 13 pin! \ this turns on the LED
    - 0 13 pin! \ this turns off the LED

To read the LED at pin 13, do this:

    - : pin@ $10000 + ! ;
    - 13 pin@ .

To turn on the LED depending on whether another pin is HIGH or LOW:

    - : main 36 pin@ 13 pin! ;    \ this turns the LED on or off depending on pin 36
    - auto-run-last               \ now you can switch pin 36 on and off and the LED changes
    - auto-run-off                \ the LED is no longer changed when pin 36 changes

More notes:

    - This is a work in progress
    - I welcome collaboration
