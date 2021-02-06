: emit $20001 ! ; : . $20002 ! ;
: (here) $30001 ; : (last) $30002 ;
: base $30003 ; : state  $30004 ; : (dsp) $30005 ;
: dict-start 0 ; : dict-end $30006 @ 1- ; 
: pin-port $10000 ; : apin-port $11000 ; 
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
// pins ...
: pin-base 0 ; : pin# pin-base + ;
: pin! pin# pin-port + ! ; 
: pin@ pin# pin-port + @ ;
: apin-base 54 ; : apin# apin-base + ;
: apin! apin# apin-port + ! ; 
: apin@ apin# apin-port + @ ;
: led! 13 pin! ; : pin>led pin@ led! ;

variable #neg
variable #len
: ++  dup @  1+ swap ! ;
: negate 0 swap - ;
: <# #neg off #len off dup 0 < if negate 1 then 0 swap ;        \ ( u1 -- 0 u2 )
: # base @ /mod swap '0' + dup '9' > if 7 + then #len ++ swap ; \ ( u1 -- c u2 )
: #s begin # dup while ;                                        \ ( u1 -- u2 )
: #> ;
: #p- drop #neg @ if '-' emit then ;
: #p #p- begin emit dup while drop ;      \ ( 0 ... n 0 -- )
: (.) <# #s #> #p ;
: . (.) space ;
: .2 <# # # #> #p ;
: .3 <# # # # #> #p ;
: .4 <# # # # # #> #p ;
: hex.      base @ swap hex (.) base ! ; 
: hex.2     base @ swap hex .2 base ! ; 
: hex.4     base @ swap hex .4 base ! ; 
: decimal.  base @ swap decimal (.) base ! ;
: decimal.3 base @ swap decimal .3  base ! ;
: binary.   base @ swap binary  (.) base ! ;

: .byte dup c@ hex.2 space 1+ ; : .bytes begin >r .byte r> 1- dup while drop ;
: .bytes-ft over - .bytes ;
: .addr dup hex.4 ':' emit space ;
: .dict here hex.4 space last hex.4
    0 begin cr .addr 8 .bytes space 8 .bytes here over > while drop ;

// other stuff
: k 1000 * ; : mil k k ;
: free dict-end here - ; : used here dict-start - ;
: auto-run 0 w! ; : auto-run-last last 1+ 1+ w@ auto-run ;
variable running : ?running running @ ;
: run running on ; : stop running off ;
: bm cr 's' emit begin wdtfeed 1- dup while drop cr 'e' emit ;
: do-loop ?running if 36 pin>led 0 apin@ . cr then ;
