
no_project = true
dofile "./premake4.lua"

-- master_CharAnim
gfx_masterMecaSim_dir = path.getabsolute(".")

master_CharAnim_files = {	gfx_masterMecaSim_dir .. "/src/master_MecaSim/*.cpp", 
							gfx_masterMecaSim_dir .. "/src/master_MecaSim/*.h"
	}
	
project("master_MecaSim")
    language "C++"
    kind "ConsoleApp"
    targetdir ( gfx_masterMecaSim_dir .. "/bin" )
	includedirs { gfx_masterMecaSim_dir .. "/src/master_MecaSim/" }
    files ( gkit_files )
    files ( master_CharAnim_files )
