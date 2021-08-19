 : auto-run-last last >body dict a! ;
 : auto-run-off 0 dict a! ; 
 : k 1000 * ; : mil k k ; 
 : bm tick swap begin 1- while- elapsed ; 
 : bm2 >r tick 0 r> for next elapsed ; 
 variable (led) 13 (led) ! 
 : led (led) @ ; 
 : led-on 1 led dp! ; 
 : led-off 0 led dp! ; 
 : blink led-on dup ms led-off dup ms ; 
 : blinks 0 swap for blink next ; 
 variable (button)  6 (button) ! 
 : button (button) @ ;  
 : button-val button dp@ ; 
 : button->led button-val if led-on else led-off then ; 
 variable (pot)  3 (pot) !  : pot (pot) @ ; 
 : pot-val pot ap@ ; 
 variable pot-lastVal  
 variable sensitivity  4 sensitivity ! 
 : pot-changed? pot-lastVal @ - abs sensitivity @ > ; 
 : .pot dup pot-lastVal ! . cr ; 
 : .pot? pot-val dup pot-changed? if .pot else drop then ; 
 : init led output-pin pot input-pin button input-pin ; 
 : go button->led .pot? ;