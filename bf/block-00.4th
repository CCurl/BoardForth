: CELL 4 ; : CELLS 4 * ;
: WORD 2 ; : WORDS 2 * ;
: tib #8 @ ; 
: >in #12 ;
: (h) #16 ; : here (h) @ ;
: (l) #20 ; : last (l) @ ;
: base #24 ;
: state #28 ;
: (dstk) #32 ;
: (rstk) #36 ;
: (dsp) #40 ; : dsp (dsp) @ ;
: ?dup dup if dup then ;
: min over over < if drop else nip then ;
: max over over > if drop else nip then ;
: hex $10 base ! ;
: decimal #10 base ! ;
: binary 2 base ! ;
: allot here + (h) ! ;
: fill s4 R<Sa!R>S[Oa@+C!1-]\ forth ;
: w@ dup c@ swap 1+ c@ 8<< + ;
: w! over 8>> over 1+ c! c! ;
s4 "here-1" forth