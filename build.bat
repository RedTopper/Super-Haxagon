@echo off
IF "%~1"=="bgm" GOTO bgm
IF "%~1"=="BGM" GOTO bgm

:default
md romfstemp
move romfs\music romfstemp
make
move romfstemp\music romfs
goto end

:bgm
make
goto end

:end
cp Super-Haxagon.cia /g/ownCloud/3DS/
echo copied to owncloud