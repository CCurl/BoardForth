: fill s4 R< S a! R> S [O a@+ C! 1-]\ forth ;

: w@ dup c@ swap 1+ c@ 8<< + ;
: w! over 8>> over 1+ c! c! ;

#50 constant #pins

variable dpin-vals
#pins #46 allot
dpin-vals #pins 0 fill
: dp-lastVal dpin-vals + c@ ;
: dp-setVal dpin-vals + c! ;

variable apin-vals
#pins #48 allot
apin-vals #pins WORDS 0 fill
: ap-lastVal WORDS apin-vals + w@ ;
: ap-setVal WORDS apin-vals + w! ;

: block-dump s4 #48 + $400 * a! $400[a@+ C@ , 1-] forth ;
: block-read s4 FR forth ;
: block-load s4 FL forth ;
: load dup block-read if block-load leave then drop ;
