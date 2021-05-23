: _t0 27 emit '[' emit ;
: gotoXY ( x y -- )  _t0 (.) ';' emit (.) 'H' emit ;
: cls _t0 ." 2J" 0 dup gotoXY ;
: .ch dup #32 < if drop '.' then dup $7e > if drop '.' then emit ;
: dump-ch low->high for i c@ .ch next ;

variable (comport) 0 (comport) !
variable (comhandle) 0 (comhandle) !
: comhandle (comhandle) @ ;
: comport (comport) @ ;
: comopen ( n -- ) dup (comport) ! com-open (comhandle) ! ;
: comclose ( -- ) comhandle com-close 0 (comhandle) ! ;
: comread ( -- c ) comhandle com-read ;
: comwrite ( c -- ) comhandle com-write 0= if ." -err-" then ;
: comall ( -- ) begin comread dup if emit 1 then while ;
: comstring ( a n -- ) over + for i c@ comwrite next ;
: comcr ( -- ) 13 comwrite ;
: comline ( cs -- ) count comstring comcr comall ;
: comlinez ( zs -- ) zcount comstring comcr ;
marker

: _t1     s" 1 2 3 + + ."             comline ;
: _t2 _t1 s" 4 5 6 + + ."             comline ;
: _t1 _t2 s" : test123 234 567 + . ;" comline ;
: _t2 _t1 s" test"                    comline ;
: _t9 _t2 ;
: load-board comhandle if _t9 else ." board port not open" then ;
marker

19200 21 comopen ." com-handle:" comhandle .
