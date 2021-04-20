: k 1000 * ; : mil k dup * ;
: elapsed tick swap - dup 1000 / . 1000 mod . ;
: bm tick swap begin 1- while elapsed ;

#50 constant #pins

variable dpin-vals
#pins allot
dpin-vals #pins 0 fill
: dp-lastVal dpin-vals + c@ ;
: dp-setVal dpin-vals + c! ;

variable apin-vals
#pins WORDS allot
apin-vals #pins WORDS 0 fill
: ap-lastVal WORDS apin-vals + w@ ;
: ap-setVal WORDS apin-vals + w! ;
