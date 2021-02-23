: ok SPACE 'o' EMIT 'k' EMIT '.' EMIT SPACE .S CR ;
: bm 'S' EMIT BEGIN 1- WHILE- DROP 'E' EMIT ;
: main 'M' EMIT  100 1000 1000 * * BM ok ; HEX ok
1 2 3 + + . >IN DUP . @ . ok
main WORDS
bye