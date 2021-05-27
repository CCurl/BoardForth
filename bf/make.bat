@echo off

if exist bf.exe ( del bf.exe )
gcc -Ofast -o bf.exe forth.cpp
if xx%1xx == xxxx goto :done
if exist bf.exe ( bf.exe )

:done