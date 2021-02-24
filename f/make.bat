@echo off

if exist f.exe ( del f.exe )
gcc -o f.exe -fpermissive -Wwrite-strings f.cpp prims.cpp loader.cpp opcodes.cpp
if xx%1xx == xxxx goto :done
if exist f.exe ( f.exe )

:done