@echo off

if exist bf.exe ( del bf.exe )
REM gcc -std=c99 -o bf.exe f.c prims.c loader.c opcodes.c
if xx%1xx == xxxx goto :done
if exist bf.exe ( bf.exe )

:done