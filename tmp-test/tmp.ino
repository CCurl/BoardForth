#include <Arduino.h>

LittleFS_Program myFS;

typedef struct LF_STR {
    File the_file;
    int is_inuse;
} LF_FILES_T;

#define NUM_FILES 16
LF_FILES_T files[NUM_FILES];

typedef int CELL;

CELL doFileOpen(const char* fn, const char *mode) {
    int fileNum = -1;
    for (int i = 0; i < NUM_FILES; i++) {
        if (!files[i].is_inuse) {
            files = i;
            break;
        }
    }
    if (0 <= fileNum) {
        int openMode = FILE_READ;
        if (mode[0] == 'w') { openMmode = FILE_WRITE; }
        files[fileNum] = myFS.open(fn, openMode);
        if (files[fileNum].something) {
            files[fp].is_inuse = 1;
        }
    }
    return fileNum;
}

void doFileClose(CELL fp) {
    if ((0 <= fp) && (fp < NUM_FILES)) {
        files[fp].file.close();
        // files[fp].is_inuse = 0;
    }
}

CELL doFileRead(CELL fp, char *buf, int sz) {
    if ((0 <= fp) && (fp < NUM_FILES)) {
        CELL num = files[fp].read(buf, sz);
        return num;
    }
    return 0;
}

CELL doFileWrite(CELL fp, char *buf, int sz) {
    if ((0 <= fp) && (fp < NUM_FILES)) {
        CELL num = files[fp].write(buf, sz);
        return num;
    }
    return 0;
}

void setup() {
    Serial.begin(19200);
    while (!Serial) {
        ; // wait for serial port to connect. Needed for native USB port only
    }
    myFS.begin(1024*1024);
    for (int i = 0; i < NUM_FILES; i++) {
        files[i].is_inuse = 0;
    }
}

int doFileReadTest(CELL num) {
    char buf[64];
    sprintf(buf, "file-%03d.4th", num);
    SerialUSB.printf("(R) trying to open %s ... ", buf);
    CELL fp = doFileOpen(buf, "rb");
    if (0 <= fp) {
        SerialUSB.print("file opened!\n");
        doFileClose(fp);
        return 1;
    }
    SerialUSB.print("file NOT opened!\n");
    return 0;
}

int doFileWriteTest(CELL num) {
    char buf[64];
    sprintf(buf, "file-%03d.4th", num);
    SerialUSB.printf("(W) trying to open %s ... ", buf);
    CELL fp = doFileOpen(buf, "wb");
    if (0 <= fp) {
        SerialUSB.print("file opened!\n");
        for (int i = 0; i < 50; i++) {
            sprintf(buf, "line #%d:%d%c%c", num, i, 13, 10);
            doFileWrite(fp, buf, strlen(buf));
        }
        doFileClose(fp);
        SerialUSB.print("file written\n");
        return 1;
    }
    SerialUSB.print("file NOT opened!\n");
    return 0;
}

CELL num = 0;
void loop() {
    while (mySerial.available()) {
        int c = mySerial.read();
        if (c == 'w') {
            doFileWriteTest(++num);
        }
        if (c == 'r') {
            for (int i = 0; i < 100; i++) {
                if (doFileReadTest(i) == 0) break;
            }
        }
    }
}
