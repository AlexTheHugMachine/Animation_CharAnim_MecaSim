
no_project = true
dofile "./premake4.lua"

-- master_CharAnim
gfx_masterCharAnim_dir = path.getabsolute(".")

master_CharAnim_files = {	gfx_masterCharAnim_dir .. "/src/master_CharAnim/CharAnimViewer.cpp", 
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/CharAnimViewer.h", 
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/BVH.cpp", 
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/BVH.h",
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/BVHAxis.h",
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/BVHChannel.cpp", 
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/BVHChannel.h",
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/BVHJoint.cpp", 
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/BVHJoint.h",
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/Viewer.cpp", 
							gfx_masterCharAnim_dir .. "/src/master_CharAnim/Viewer.h"
	}
project("master_CharAnim")
    language "C++"
    kind "ConsoleApp"
    targetdir ( gfx_masterCharAnim_dir .. "/bin" )
	includedirs { gfx_masterCharAnim_dir .. "/src/master_CharAnim/" }
    files ( gkit_files )
    files ( master_CharAnim_files )
	files { gfx_masterCharAnim_dir .. "/src/master_CharAnim/main.cpp" }
