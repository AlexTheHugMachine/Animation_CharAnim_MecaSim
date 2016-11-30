
cd ..
.\premake\premake4.exe --os=windows --file=l2_lifgfx.lua gmake
.\premake\premake4.exe --os=windows --file=l2_lifgfx.lua codeblocks

.\premake\premake5.exe --os=windows --file=l2_lifgfx.lua vs2013
.\premake\premake5.exe --os=windows --file=l2_lifgfx.lua vs2015

pause
