/*
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
*/


typedef unsigned char BYTE;
int pop() { return 0; }
 
void runProgram(BYTE *PC) {
    BYTE IR = *(PC++);
    int N, T;
    while (1) {
    }
}
