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
EMIT emit
DOT .
-------------------------------------------------------
END of NimbleText input

The NimbleText script
-------------------------------------------------------
$each
#define <%($0 + '               ').substring(0,12)%><%Number($rownum) < 10 ? ' ' : ''%>$rownum     // $1
$once

$each
FLASH(<%Number(10000) + Number($rownum)%>) = "$1";
$once

const char *const keyWords[] PROGMEM = {
$each
    string_<%Number(10000) + Number($rownum)%>,
$once    0 };

$each
        case $0: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

$once
    }
-------------------------------------------------------
END of the NimbleText script

The NimbleText output
-------------------------------------------------------
#define NOOP          0     // nop
#define CLIT          1     // cliteral
#define WLIT          2     // wliteral
#define LIT           3     // literal
#define CFETCH        4     // c@
#define WFETCH        5     // w@
#define FETCH         6     // @
#define CSTORE        7     // c!
#define WSTORE        8     // w!
#define STORE         9     // !
#define CALL         10     // call
#define RET          11     // ;
#define JMP          12     // jmp
#define JMPZ         13     // jmpz
#define JMPNZ        14     // jmpnz
#define ONEMINUS     15     // 1-
#define ONEPLUS      16     // 1+
#define DUP          17     // dup
#define SWAP         18     // swap
#define DROP         19     // drop
#define OVER         20     // over
#define ADD          21     // +
#define SUB          22     // -
#define MULT         23     // *
#define DIV          24     // /
#define LSHIFT       25     // <<
#define RSHIFT       26     // >>
#define AND          27     // and
#define OR           28     // or
#define XOR          29     // xor
#define NOT          30     // not
#define EMIT         31     // emit

FLASH(10000) = "nop";
FLASH(10001) = "cliteral";
FLASH(10002) = "wliteral";
FLASH(10003) = "literal";
FLASH(10004) = "c@";
FLASH(10005) = "w@";
FLASH(10006) = "@";
FLASH(10007) = "c!";
FLASH(10008) = "w!";
FLASH(10009) = "!";
FLASH(10010) = "call";
FLASH(10011) = "ret";
FLASH(10012) = "jmp";
FLASH(10013) = "jmpz";
FLASH(10014) = "jmpnz";
FLASH(10015) = "1-";
FLASH(10016) = "1+";
FLASH(10017) = "dup";
FLASH(10018) = "swap";
FLASH(10019) = "drop";
FLASH(10020) = "over";
FLASH(10021) = "+";
FLASH(10022) = "-";
FLASH(10023) = "*";
FLASH(10024) = "/";
FLASH(10025) = "<<";
FLASH(10026) = ">>";
FLASH(10027) = "and";
FLASH(10028) = "or";
FLASH(10029) = "xor";
FLASH(10030) = "not";
FLASH(10031) = "emit";

const char *const keyWords[] PROGMEM = {
    string_10000,
    string_10001,
    string_10002,
    string_10003,
    string_10004,
    string_10005,
    string_10006,
    string_10007,
    string_10008,
    string_10009,
    string_10010,
    string_10011,
    string_10012,
    string_10013,
    string_10014,
    string_10015,
    string_10016,
    string_10017,
    string_10018,
    string_10019,
    string_10020,
    string_10021,
    string_10022,
    string_10023,
    string_10024,
    string_10025,
    string_10026,
    string_10027,
    string_10028,
    string_10029,
    string_10030,
    string_10031,
    0 };

        case NOOP: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case CLIT: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case WLIT: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case LIT: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case CFETCH: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case WFETCH: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case FETCH: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case CSTORE: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case WSTORE: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case STORE: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case CALL: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case RET: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case JMP: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case JMPZ: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case JMPNZ: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case ONEMINUS: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case ONEPLUS: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case DUP: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case SWAP: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case DROP: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case OVER: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case ADD: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case SUB: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case MULT: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case DIV: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case LSHIFT: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case RSHIFT: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case AND: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case OR: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case XOR: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case NOT: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

        case EMIT: 
          t1 = T; T = N; N = t1; pop(); push(t1);
          break;

    }
