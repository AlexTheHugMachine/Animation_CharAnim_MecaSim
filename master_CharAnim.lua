
no_project = true
dofile "./premake4.lua"

-- master_CharAnim
gfx_masterCharAnim_dir = path.getabsolute(".")

master_CharAnim_files = {	gfx_masterCharAnim_dir .. "/src/master_CharAnim/*.cpp", 
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/*.h"
	}
	
project("master_CharAnim")
    language "C++"
    kind "ConsoleApp"
    targetdir ( gfx_masterCharAnim_dir .. "/bin" )
	includedirs { gfx_masterCharAnim_dir .. "/src/master_CharAnim/" }
    files ( gkit_files )
    files ( master_CharAnim_files )
