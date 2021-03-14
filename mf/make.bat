@echo off

if exist mf.exe ( del mf.exe )
gcc -o mf.exe forth.cpp main.cpp
if xx%1xx == xxxx goto :done
if exist mf.exe ( mf.exe )

:done