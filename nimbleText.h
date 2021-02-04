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
-------------------------------------------------------
$each
#define <%($0 + '               ').substring(0,12)%><%Number($rownum) < 10 ? ' ' : ''%>$rownum     // $1
$once

$each
const PROGMEM char string_$rownum[] = "$1";
$once

const char *const keyWords[] PROGMEM = {
$each
    string_$rownum),
$once    0 };

$each
        case $0: 
          // N = N+T; t1 = T; T = N; N = t1; pop(); push(t1);
          break;

$once
    }
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
