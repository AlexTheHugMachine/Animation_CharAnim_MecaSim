solution "gKit2light"
	configurations { "debug", "release" }

	platforms { "x64", "x32" }
	
	includedirs { ".", "src/gKit" }
	
	configuration "debug"
		targetdir "bin/debug"
		defines { "DEBUG" }
		flags { "Symbols" }

	configuration "release"
		targetdir "bin/release"
--~ 		defines { "NDEBUG" }
--~ 		defines { "GK_RELEASE" }
		flags { "OptimizeSpeed" }

	configuration "linux"
		buildoptions { "-mtune=native -march=native" }
		buildoptions { "-std=c++11" }
		buildoptions { "-W -Wall -Wextra -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable", "-pipe" }
		buildoptions { "-flto"}
		linkoptions { "-flto"}
		links { "GLEW", "SDL2", "SDL2_image", "GL" }

	configuration { "linux", "debug" }
		buildoptions { "-g"}
		linkoptions { "-g"}
		
	configuration { "linux", "release" }
		buildoptions { "-fopenmp" }
		linkoptions { "-fopenmp" }

	configuration { "windows" }
		defines { "WIN32", "NVWIDGETS_EXPORTS", "_USE_MATH_DEFINES", "_CRT_SECURE_NO_WARNINGS" }
		defines { "NOMINMAX" } -- allow std::min() and std::max() in vc++ :(((

	configuration { "windows", "gmake", "x32" }
		buildoptions { "-std=c++11"}
		includedirs { "extern/mingw/include" }
		libdirs { "extern/mingw/lib" }
		links { "mingw32", "SDL2main", "SDL2", "SDL2_image", "opengl32", "glew32" }

	configuration { "windows", "codeblocks", "x32" }
		buildoptions { "-std=c++11"}
		includedirs { "extern/mingw/include" }
		libdirs { "extern/mingw/lib" }
		links { "mingw32", "SDL2main", "SDL2", "SDL2_image", "opengl32", "glew32" }
		
	configuration { "windows", "vs2013", "x64" }
		includedirs { "extern/visual2013/include" }
		libdirs { "extern/visual2013/lib" }
		platforms { "x64" }
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image" }

		
	configuration { "windows", "vs2015", "x64" }
		includedirs { "extern/visual2015/include" }
		libdirs { "extern/visual2015/lib" }
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image" }
		
	configuration "macosx"
		local frameworks= "-F /Library/Frameworks/"
		buildoptions { "-std=c++11" }
		defines { "GK_MACOS" }
		buildoptions { frameworks }
		linkoptions { frameworks .. " -framework OpenGL -framework SDL2 -framework SDL2_image" }


 -- description des fichiers communs
local gkit_files = { "src/gKit/*.cpp", "src/gKit/*.h" }

 -- description des projets		

    
local projects = {
	"shader_kit"
}

for i, name in ipairs(projects) do
	project(name)
		language "C++"
		kind "ConsoleApp"
		targetdir "bin"
		files ( gkit_files )
		files { "src/" .. name..'.cpp' }
end

 -- description des tutos
local tutos = {
	"tuto1",
	"tuto2",
	"tuto3",
	"tuto4",
	"tuto5",
	"tuto6",
	"tuto7",
	"tuto8",
	
	"tuto_transform",
	
	"tuto2GL",
	"tuto3GL",
	"tuto3GL_reflect",
	"tuto4GL",
	"tuto4GL_normals",
	"tuto5GL",
	"tuto5GL_sampler",
	"tuto5GL_samplers",
	"tuto5GL_multi",
	"tuto6GL",
	"tuto6GL_buffer",
	"tuto_framebuffer",
	
--~ 	"ray_tuto1",
--~ 	"ray_tuto2",
--~ 	"ray_tuto3",
--~ 	
--~ 	"pipeline"
}

for i, name in ipairs(tutos) do
	project(name)
		language "C++"
		kind "ConsoleApp"
		targetdir "bin"
		files ( gkit_files )
		files { "tutos/" .. name..'.cpp' }
end


-- description des tutos openGL avances / M2
local tutosM2 = {
	"tuto_time",
	"tuto_mdi"
}

for i, name in ipairs(tutosM2) do
	project(name)
		language "C++"
		kind "ConsoleApp"
		targetdir "bin"
		files ( gkit_files )
		files { "tutos/M2/" .. name..'.cpp' }
end

project("tp2")
	language "C++"
	kind "ConsoleApp"
	targetdir "bin"
	files ( gkit_files )
	files { "opengl_tp2/tp2.cpp" }


 
project("l2_lifgfx")
    language "C++"
    kind "ConsoleApp"
    targetdir "bin"
    files ( gkit_files )
    files { "src/l2_lifgfx/main.cpp",
			"src/l2_lifgfx/Viewer.cpp", "src/l2_lifgfx/Viewer.h", 
			"src/l2_lifgfx/AnimationCurve.cpp", "src/l2_lifgfx/AnimtionCurve.h" }

newoption {
   trigger     = "corrige",
   value       = "cor",
   description = "Choose a particular sub-list of projects",
   allowed = {
      { "off",	"Pas de corrige" },
      { "on",  	"Avec corrige" },
   }
}

			
if not _OPTIONS["corrige"] then
   _OPTIONS["corrige"] = "off"
end

if _OPTIONS["corrige"] == "on" then
project("l2_lifgfx_corrige")
    language "C++"
    kind "ConsoleApp"
    targetdir "bin"
    files ( gkit_files )
    files { "src/l2_lifgfx/Viewer.cpp", "src/l2_lifgfx/Viewer.h", 
			"src/l2_lifgfx/Viewer_corrige.cpp", "src/l2_lifgfx/Viewer_corrige.h", 
			"src/l2_lifgfx/AnimationCurve.cpp", "src/l2_lifgfx/AnimtionCurve.h" }
end
