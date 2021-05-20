variable (port) 0 (port) !
: port (port) @ ;
: comopen com-open (port) ! ;
: comread port com-read ;
: comemit port com-read ?dup if emit then ;
: comwrite port com-write 0= if ." -err-" then ;
: comclose port com-close 0 (port) ! ;
: comall begin comread dup if emit 1 then while ;
: comstr over + for i c@ comwrite next 13 comwrite ;
19200 21 comopen port .
