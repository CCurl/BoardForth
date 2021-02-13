: emit $20001 ! ; : . $20002 ! ;
: (here) $30001 ; : (last) $30002 ;
: base $30003 ; : state  $30004 ; : (dsp) $30005 ;
: dict-start 0 ; : dict-end $30006 @ 1- ; 
: pin-port $10000 ; : apin-port $11000 ; 
: com-open-port $28001 ; : com-io-port $28002 ;
: block-open-port $29001 ; : file-io-port $29002 ;
: here (here) @ ; : last (last) @ ; 
: depth (dsp) @ ; : sp! 0 (dsp) ! ;
variable fh 2 fh !
variable fl 0 fl ! 
: marker here fh ! last fl ! ;
: forget fh @ (here) ! fl @ (last) ! ;
: cr #13 emit #10 emit ; : space $20 emit ;
: .. dup . ; : hex $10 base ! ; : decimal #10 base ! ; : binary 2 base ! ;
: .s '(' emit >r >r >r >r .. r> .. r> .. r> .. r> .. space ')' emit ;
: count dup 1+ swap c@ ;
: type begin >r dup c@ emit 1+ r> 1- dup while drop drop ;
: on 1 swap ! ; : off 0 swap ! ;
: ?dup dup if dup then ; : 2dup over over ; : 2drop drop drop ;
: nip swap drop ; : tuck swap over ;
: / /mod nip ; : mod /mod drop ;
: words cr last begin dup 5 + count type space w@ dup while drop ;
: .word dup . dup 2 + w@ . space 5 + count type cr ;
: wordsl cr last begin dup .word w@ dup while drop ;
wordsl
