@echo off

if exist bf.exe ( del bf.exe )
gcc -o bf.exe -fpermissive -Wwrite-strings f.cpp prims.cpp loader.cpp opcodes.cpp
if xx%1xx == xxxx goto :done
if exist bf.exe ( bf.exe )

:done