: cr #13 emit #10 emit ; : bl $20 ; : space bl emit ;
: ?dup if- dup then ; : 2dup over over ; : 2drop drop drop ; 
: nip swap drop ; : tuck swap over ;
: hex $10 base ! ; : decimal #10 base ! ; : binary 2 base ! ;
: ok space 'o' emit 'k' emit '.' emit space .s cr ;
: bm 's' emit begin 1- while- drop 'e' emit ;
: main 'm' emit  100 1000 1000 * * bm ok ; hex ok
'L' emit (last) dup . @ . ok
'H' emit (here) dup . @ . ok
1 2 3 + + . >in    dup . @ . ok
main
