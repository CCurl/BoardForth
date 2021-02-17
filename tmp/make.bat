@echo off

if exist f.exe ( del f.exe )
gcc -o f.exe f.c prims.c
if xx%1xx == xxxx goto :done
if exist f.exe ( f.exe )

:done