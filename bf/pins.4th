: fill rV r! rN r! rF r! rN r@ begin rV r@ rF r@+ c! rN r@- while ;
: BYTES ; : WORDS 2 * ; : CELLS 4 * ;
: w@ dup c@ swap 1+ c@ 256 * + ;
: w! over 256 / over 1+ c! c! ;
variable dpin-vals
48 BYTES allot
variable apin-vals
49 WORDS allot
: dp-lastVal BYTES dpin-vals + c@ ;
: dp-setVal dpin-vals + c! ;
: ap-lastVal WORDS apin-vals w@ ;
: ap-setVal apin-vals + w! ;
: fill rV r! rN r! rF r! rN r@ begin rV r@ rF r@+ c! rN r@- while ;
: BYTES ; : WORDS 2 * ; : CELLS 4 * ;
: w@ dup c@ swap 1+ c@ 256 * + ;
: w! over 256 / over 1+ c! c! ;
variable dpin-vals
48 BYTES allot
variable apin-vals
49 WORDS allot
: dp-lastVal BYTES dpin-vals + c@ ;
: dp-setVal dpin-vals + c! ;
: ap-lastVal WORDS apin-vals w@ ;
: ap-setVal apin-vals + w! ;
.ic
apin-vals .
apin-vals
rN r@ .
apin-vals 52 words
WORDS
0
fill
variable dpin-vals
48 BYTES allot
variable apin-vals
49 WORDS allot
: BYTES ; : WORDS 2 * ; : CELLS 4 * ;
: w@ dup c@ swap 1+ c@ 256 * + ;
: w! over 256 / over 1+ c! c! ;
variable dpin-vals
48 BYTES allot
variable apin-vals
49 WORDS allot
: dp-lastVal BYTES dpin-vals + c@ ;
: dp-setVal dpin-vals + c! ;
: ap-lastVal WORDS apin-vals w@ ;
: ap-setVal apin-vals + w! ;
.ic
dpin-vals
52 BYTES
0
rV r! rN r! rF r!
