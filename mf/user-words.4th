: _t0 27 emit '[' emit ;
: gotoXY ( x y -- )  _t0 (.) ';' emit (.) 'H' emit ;
: cls _t0 ." 2J" 0 dup gotoXY ;

variable (comport) 0 (comport) !
variable (comhandle) 0 (comhandle) !
: comhandle (comhandle) @ ;
: comport (comport) @ ;
: comopen ( n -- ) dup (comport) ! com-open (comhandle) ! ;
: comclose ( -- ) comhandle com-close 0 (comhandle) ! ;
: comread ( -- c ) comhandle com-read ;
: comwrite ( c -- ) comhandle com-write 0= if ." -err-" then ;
: comemit ( -- ) comhandle com-read ?dup if emit then ;
: comall ( -- ) begin comread dup if emit 1 then while ;
: comstr ( a n -- ) over + for i c@ comwrite next 13 comwrite ;
19200 21 comopen comhandle .
