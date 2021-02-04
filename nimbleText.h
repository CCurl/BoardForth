// NOTE: do not #include this file

The NimbleText input
-------------------------------------------------------
NOOP nop
CLIT cliteral
WLIT wliteral
LIT literal
CFETCH c@
WFETCH w@
FETCH @
CSTORE c!
WSTORE w!
STORE !
CALL call
RET ret
JMP jmp
JMPZ jmpz
JMPNZ jmpnz
ONEMINUS 1-
ONEPLUS 1+
DUP dup
SWAP swap
DROP drop
OVER over
ADD +
SUB -
MULT *
DIV /
LSHIFT <<
RSHIFT >>
AND and
OR or
XOR xor
NOT not
DTOR >r
RFETCH r@
RTOD r>
WDTFEED wdtfeed
-------------------------------------------------------
END of NimbleText input

For the opcodes
------------------------------------------------------
$each+
#define <%($0 + '               ').substring(0,13)%><%(Number($h0)+$rownum) < 10 ? ' ' : ''%><%Number($h0)+$rownum%>     // $1
$once

$each+
const PROGMEM char string_<%Number($h0)+$rownum%>[] = "$1";
$once

$each+
    string_<%Number($h0)+$rownum%>,

$once

$each+
        case $0: 
          // N = N+T; t1 = T; T = N; N = t1; pop(); push(t1);
          break;


-------------------------------------------------------
END of the NimbleText script

For the built-in words
-------------------------------------------------------
$each
const PROGMEM char string_<%Number(1000) + Number($rownum)%>[] = "$0";
$once

const char *const builtInWords[] PROGMEM = {
$each
    string_<%Number(1000) + Number($rownum)%>,
$once
    0 };
-------------------------------------------------------
END of the NimbleText script

#define TOCOM        38     // >com
#define FROMCOM      39     // com>

const PROGMEM char string_38[] = ">com";
const PROGMEM char string_39[] = "com>";

    string_38,
    string_39,

        case TOCOM: 
          // N = N+T; t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case FROMCOM: 
          // N = N+T; t1 = T; T = N; N = t1; pop(); push(t1);
          break;

