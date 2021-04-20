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
