@echo off
IF "%~1"=="bgm" GOTO bgm
IF "%~1"=="BGM" GOTO bgm

:default
md romfstemp
move romfs\music romfstemp
move resource\cia-NO-BGM.rsf resource\cia.rsf
make
move resource\cia.rsf resource\cia-NO-BGM.rsf
move romfstemp\music romfs
cp Super-Haxagon.cia Super-Haxagon-NO-BGM.cia
IF "%~1"=="both" GOTO bgm
goto end

:bgm
move resource\cia-BGM.rsf resource\cia.rsf
make
move resource\cia.rsf resource\cia-BGM.rsf
cp Super-Haxagon.cia Super-Haxagon-BGM.cia
goto end

:end
cp Super-Haxagon.cia /g/ownCloud/3DS/
echo copied to owncloud