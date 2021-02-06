// : (here) $30001 ; : (last) $30002 ;
// 2 (here) ! 0 (last) !
: emit $20001 ! ; : . $20002 ! ;
: (here) $30001 ; : (last) $30002 ;
: base $30003 ; : state  $30004 ; : (dsp) $30005 ;
: dict-start 0 ; : dict-end $30006 @ 1- ; 
: pin-port $10000 ; : apin-port $11000 ; 
: here (here) @ ; : last (last) @ ; 
: depth (dsp) @ ; : clearstack 0 (dsp) ! ;
variable m-here 2 m-here !
variable m-last 0 m-last ! 
: marker here m-here ! last m-last ! ;
: forget m-here @ (here) ! m-last @ (last) ! ;
: cr #13 emit #10 emit ; : space $20 emit ;
: .. dup . ; : hex $10 base ! ; : decimal #10 base ! ;
: .s '(' emit >r >r >r >r .. r> .. r> .. r> .. r> .. space ')' emit ;
: count dup 1+ swap c@ ;
: type begin >r dup c@ emit 1+ r> 1- dup while drop drop ;
: on 1 swap ! ; : off 0 swap ! ;
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
: pin? pin@ . ; : apin? apin@ . ;
: led! 13 pin! ; : pin>led pin@ led! ;
// other stuff
: k 1000 * ; : mil k k ;
: free dict-end here - ; : used here dict-start - ;
: .char dup c@ . 1+ ; : .char4 .char .char .char .char ; 
: dump begin >r dup .char 1+ r> 1- dup while drop drop ;
: dump-ft over - dump ;
: dump-16 dup . ':' emit space .char4 .char4 .char4 .char4 ;
: dump-dict here . last . 0 begin cr dump-16 here over > while drop ;
: auto-run 0 w! ; : auto-run-last last 1+ 1+ w@ auto-run ;
variable running : ?running running @ ;
: run running on ; : stop running off ;
: bm cr 's' emit begin wdtfeed 1- dup while drop cr 'e' emit ;
: do-loop ?running if 36 pin>led 0 apin? cr then ;
