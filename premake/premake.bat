
cd ..

.\premake\premake4.exe --os=windows --file=./master_CharAnim.lua gmake
.\premake\premake4.exe --os=windows --file=./master_CharAnim.lua codeblocks

.\premake\premake5.exe --os=windows --file=./master_CharAnim.lua vs2013
.\premake\premake5.exe --os=windows --file=./master_CharAnim.lua vs2015

pause
