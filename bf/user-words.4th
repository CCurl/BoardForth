
: _t1     s" 1 2 3 + + ."             comline ;
: _t2 _t1 s" 4 5 6 + + ."             comline ;
: _t1 _t2 s" : test123 234 567 + . ;" comline ;
: _t2 _t1 s" test"                    comline ;
: _t9 _t2 ;
: load-board comhandle if _t9 else ." board port not open" then ;
marker

19200 21 comopen ." com-handle:" comhandle .
