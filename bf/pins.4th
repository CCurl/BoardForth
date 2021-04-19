: w@ dup c@ swap 1+ c@ 256 * + ;
: w! over 256 / over 1+ c! c! ;
: fill s4 v!Sf![v@f@+C!1-] forth ;
50 constant #pins
variable dpin-vals
#pins CELL - allot
dpin-vals #pins 0 fill
: dp-lastVal dpin-vals + c@ ;
: dp-setVal dpin-vals + c! ;

variable apin-vals
#pins WORDS 1- 1- allot
apin-vals #pins WORDS 0 fill
: ap-lastVal WORDS apin-vals + w@ ;
: ap-setVal WORDS apin-vals + w! ;
