$each
#define $1 $rownum // $4
$once 
void runProgram() {
    BYTE IR = *(PC++);
    while (1) {
        switch (IR) {
$each
            case $1:     // $4 (#$rownum)
                N += T; pop();
                break;
$once 
        }
    }
}


#define OP_NOOP 0 // noop
#define OP_CLIT 1 // cliteral
#define OP_WLIT 2 // wliteral
#define OP_LIT 3 // literal
#define OP_CFETCH 4 // c@
#define OP_WFETCH 5 // w@
#define OP_AFETCH 6 // a@
#define OP_FETCH 7 // @
#define OP_CSTORE 8 // c!
#define OP_WSTORE 9 // w!
#define OP_ASTORE 10 // a!
#define OP_STORE 11 // !
#define OP_CCOMMA 12 // c,
#define OP_WCOMMA 13 // w,
#define OP_COMMA 14 // ,
#define OP_ACOMMA 15 // a,
#define OP_CALL 16 // call
#define OP_RET 17 // exit
#define OP_JMP 18 // -n-
#define OP_JMPZ 19 // -n-
#define OP_JMPNZ 20 // -n-
#define OP_ONEMINUS 21 // 1-
#define OP_ONEPLUS 22 // 1+
#define OP_DUP 23 // dup
#define OP_SWAP 24 // swap
#define OP_DROP 25 // drop
#define OP_OVER 26 // over
#define OP_ADD 27 // +
#define OP_SUB 28 // -
#define OP_MULT 29 // *
#define OP_SLMOD 30 // /mod
#define OP_LSHIFT 31 // <<
#define OP_RSHIFT 32 // >>
#define OP_AND 33 // and
#define OP_OR 34 // or
#define OP_XOR 35 // xor
#define OP_NOT 36 // not
#define OP_DTOR 37 // >r
#define OP_RFETCH 38 // r@
#define OP_RTOD 39 // r>
#define OP_EMIT 40 // emit
#define OP_TYPE 41 // type
#define OP_DOTS 42 // .s
#define OP_DOTQUOTE 43 // .\"
#define OP_PAREN 44 // (
#define OP_WDTFEED 45 // wdtfeed
#define OP_BREAK 46 // brk
#define OP_CMOVE 47 // cmove
#define OP_CMOVE2 48 // cmove>
#define OP_FILL 49 // fill
#define OP_OPENBLOCK 50 // open-block
#define OP_FILECLOSE 51 // file-close
#define OP_FILEREAD 52 // file-read
#define OP_LOAD 53 // load
#define OP_THRU 54 // thru
#define OP_DO 55 // do
#define OP_LOOP 56 // loop
#define OP_LOOPP 57 // loop+
#define OP_UNUSED7 58 // -n-
#define OP_PARSEWORD 59 // parse-word
#define OP_PARSELINE 60 // parse-line
#define OP_GETXT 61 // >body
#define OP_ALIGN2 62 // align2
#define OP_ALIGN4 63 // align4
#define OP_CREATE 64 // create
#define OP_FIND 65 // find
#define OP_NEXTWORD 66 // next-word
#define OP_ISNUMBER 67 // number?
#define OP_NJMPZ 68 // -n-
#define OP_NJMPNZ 69 // -n-
#define OP_LESS 70 // <
#define OP_EQUALS 71 // =
#define OP_GREATER 72 // >
#define OP_I 73 // i
#define OP_J 74 // j
#define OP_INPUTPIN 75 // input
#define OP_OUTPUTPIN 76 // output
#define OP_DELAY 77 // ms
#define OP_DELAY 78 // ms
#define OP_APINSTORE 79 // 
#define OP_DPINSTORE 80 // dp!
#define OP_APINFETCH 81 // ap@
#define OP_DPINFETCH 82 // dp@
#define OP_MWFETCH 83 // mw@
#define OP_MCSTORE 84 // mc!
#define OP_NUM2STR 85 // num>str
#define OP_COM 86 // com
#define OP_BYE 87 // bye
 
void runProgram() {
    BYTE IR = *(PC++);
    while (1) {
        switch (IR) {
            case OP_NOOP:     // noop (#0)
                N += T; pop();
                break;
            case OP_CLIT:     // cliteral (#1)
                N += T; pop();
                break;
            case OP_WLIT:     // wliteral (#2)
                N += T; pop();
                break;
            case OP_LIT:     // literal (#3)
                N += T; pop();
                break;
            case OP_CFETCH:     // c@ (#4)
                N += T; pop();
                break;
            case OP_WFETCH:     // w@ (#5)
                N += T; pop();
                break;
            case OP_AFETCH:     // a@ (#6)
                N += T; pop();
                break;
            case OP_FETCH:     // @ (#7)
                N += T; pop();
                break;
            case OP_CSTORE:     // c! (#8)
                N += T; pop();
                break;
            case OP_WSTORE:     // w! (#9)
                N += T; pop();
                break;
            case OP_ASTORE:     // a! (#10)
                N += T; pop();
                break;
            case OP_STORE:     // ! (#11)
                N += T; pop();
                break;
            case OP_CCOMMA:     // c, (#12)
                N += T; pop();
                break;
            case OP_WCOMMA:     // w, (#13)
                N += T; pop();
                break;
            case OP_COMMA:     // , (#14)
                N += T; pop();
                break;
            case OP_ACOMMA:     // a, (#15)
                N += T; pop();
                break;
            case OP_CALL:     // call (#16)
                N += T; pop();
                break;
            case OP_RET:     // exit (#17)
                N += T; pop();
                break;
            case OP_JMP:     // -n- (#18)
                N += T; pop();
                break;
            case OP_JMPZ:     // -n- (#19)
                N += T; pop();
                break;
            case OP_JMPNZ:     // -n- (#20)
                N += T; pop();
                break;
            case OP_ONEMINUS:     // 1- (#21)
                N += T; pop();
                break;
            case OP_ONEPLUS:     // 1+ (#22)
                N += T; pop();
                break;
            case OP_DUP:     // dup (#23)
                N += T; pop();
                break;
            case OP_SWAP:     // swap (#24)
                N += T; pop();
                break;
            case OP_DROP:     // drop (#25)
                N += T; pop();
                break;
            case OP_OVER:     // over (#26)
                N += T; pop();
                break;
            case OP_ADD:     // + (#27)
                N += T; pop();
                break;
            case OP_SUB:     // - (#28)
                N += T; pop();
                break;
            case OP_MULT:     // * (#29)
                N += T; pop();
                break;
            case OP_SLMOD:     // /mod (#30)
                N += T; pop();
                break;
            case OP_LSHIFT:     // << (#31)
                N += T; pop();
                break;
            case OP_RSHIFT:     // >> (#32)
                N += T; pop();
                break;
            case OP_AND:     // and (#33)
                N += T; pop();
                break;
            case OP_OR:     // or (#34)
                N += T; pop();
                break;
            case OP_XOR:     // xor (#35)
                N += T; pop();
                break;
            case OP_NOT:     // not (#36)
                N += T; pop();
                break;
            case OP_DTOR:     // >r (#37)
                N += T; pop();
                break;
            case OP_RFETCH:     // r@ (#38)
                N += T; pop();
                break;
            case OP_RTOD:     // r> (#39)
                N += T; pop();
                break;
            case OP_EMIT:     // emit (#40)
                N += T; pop();
                break;
            case OP_TYPE:     // type (#41)
                N += T; pop();
                break;
            case OP_DOTS:     // .s (#42)
                N += T; pop();
                break;
            case OP_DOTQUOTE:     // .\" (#43)
                N += T; pop();
                break;
            case OP_PAREN:     // ( (#44)
                N += T; pop();
                break;
            case OP_WDTFEED:     // wdtfeed (#45)
                N += T; pop();
                break;
            case OP_BREAK:     // brk (#46)
                N += T; pop();
                break;
            case OP_CMOVE:     // cmove (#47)
                N += T; pop();
                break;
            case OP_CMOVE2:     // cmove> (#48)
                N += T; pop();
                break;
            case OP_FILL:     // fill (#49)
                N += T; pop();
                break;
            case OP_OPENBLOCK:     // open-block (#50)
                N += T; pop();
                break;
            case OP_FILECLOSE:     // file-close (#51)
                N += T; pop();
                break;
            case OP_FILEREAD:     // file-read (#52)
                N += T; pop();
                break;
            case OP_LOAD:     // load (#53)
                N += T; pop();
                break;
            case OP_THRU:     // thru (#54)
                N += T; pop();
                break;
            case OP_DO:     // do (#55)
                N += T; pop();
                break;
            case OP_LOOP:     // loop (#56)
                N += T; pop();
                break;
            case OP_LOOPP:     // loop+ (#57)
                N += T; pop();
                break;
            case OP_UNUSED7:     // -n- (#58)
                N += T; pop();
                break;
            case OP_PARSEWORD:     // parse-word (#59)
                N += T; pop();
                break;
            case OP_PARSELINE:     // parse-line (#60)
                N += T; pop();
                break;
            case OP_GETXT:     // >body (#61)
                N += T; pop();
                break;
            case OP_ALIGN2:     // align2 (#62)
                N += T; pop();
                break;
            case OP_ALIGN4:     // align4 (#63)
                N += T; pop();
                break;
            case OP_CREATE:     // create (#64)
                N += T; pop();
                break;
            case OP_FIND:     // find (#65)
                N += T; pop();
                break;
            case OP_NEXTWORD:     // next-word (#66)
                N += T; pop();
                break;
            case OP_ISNUMBER:     // number? (#67)
                N += T; pop();
                break;
            case OP_NJMPZ:     // -n- (#68)
                N += T; pop();
                break;
            case OP_NJMPNZ:     // -n- (#69)
                N += T; pop();
                break;
            case OP_LESS:     // < (#70)
                N += T; pop();
                break;
            case OP_EQUALS:     // = (#71)
                N += T; pop();
                break;
            case OP_GREATER:     // > (#72)
                N += T; pop();
                break;
            case OP_I:     // i (#73)
                N += T; pop();
                break;
            case OP_J:     // j (#74)
                N += T; pop();
                break;
            case OP_INPUTPIN:     // input (#75)
                N += T; pop();
                break;
            case OP_OUTPUTPIN:     // output (#76)
                N += T; pop();
                break;
            case OP_DELAY:     // ms (#77)
                N += T; pop();
                break;
            case OP_DELAY:     // ms (#78)
                N += T; pop();
                break;
            case OP_APINSTORE:     //  (#79)
                N += T; pop();
                break;
            case OP_DPINSTORE:     // dp! (#80)
                N += T; pop();
                break;
            case OP_APINFETCH:     // ap@ (#81)
                N += T; pop();
                break;
            case OP_DPINFETCH:     // dp@ (#82)
                N += T; pop();
                break;
            case OP_MWFETCH:     // mw@ (#83)
                N += T; pop();
                break;
            case OP_MCSTORE:     // mc! (#84)
                N += T; pop();
                break;
            case OP_NUM2STR:     // num>str (#85)
                N += T; pop();
                break;
            case OP_COM:     // com (#86)
                N += T; pop();
                break;
            case OP_BYE:     // bye (#87)
                N += T; pop();
                break;
 
        }
    }
}
