#include "defs.h"


/* -- NimbleText script:
$once
void loadSystem() {
$each
    parseLine(F("$row"));
$once
}
*/


// vvvvv - NimbleText generated - vvvvv
void loadSystem() {
    parseLine(F(": ?dup if- dup then ; : 2dup over over ; : 2drop drop drop ;"));
    parseLine(F(": hex ! base ! ; : decimal #10 base ! ; : binary 2 base ! ;"));
    parseLine(F(": cr #13 emit #10 emit ; : bl $20 ; : space bl emit ;"));
    parseLine(F(": ?dup if- dup then ; : 2dup over over ; : 2drop drop drop ; "));
    parseLine(F(": hex ! base ! ; : decimal #10 base ! ; : binary 2 base ! ;"));
    parseLine(F(": ok space 'o' emit 'k' emit '.' emit space .s cr ;"));
    parseLine(F(": bm 's' emit begin 1- while- drop 'e' emit ;"));
    parseLine(F(": main 'm' emit  100 1000 1000 * * bm ok ; hex ok"));
    parseLine(F("'L' emit (last) dup . @ . ok"));
    parseLine(F("'H' emit (here) dup . @ . ok"));
    parseLine(F("1 2 3 + + . >in    dup . @ . ok"));
    parseLine(F("main"));
}
// ^^^^^ - NimbleText generated - ^^^^^
