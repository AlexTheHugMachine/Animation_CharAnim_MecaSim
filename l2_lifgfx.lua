
no_project = true
dofile "./premake4.lua"

l2_gfx_dir = path.getabsolute(".")

l2_lifgfx_files = {	l2_gfx_dir .. "/src/l2_lifgfx/*", l2_gfx_dir .. "/src/l2_lifgfx/pacman_core/*" }
	
project("l2_lifgfx")
    language "C++"
    kind "ConsoleApp"
    targetdir ( l2_gfx_dir .. "/bin" )
    includedirs { l2_gfx_dir .. "/src/l2_lifgfx/" }
    files ( gkit_files )
    files ( l2_lifgfx_files )
