solution "gKit2light"
	configurations { "debug", "release" }

	platforms { "x64", "x32" }
	
	includedirs { ".", "src/gKit" }
	
	gkit_dir = path.getabsolute(".")
	
	
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
		
	configuration { "windows", "vs2013" }
		if _PREMAKE_VERSION >="5.0" then
			system "Windows"
			architecture "x64"
		end
		includedirs { "extern/visual2013/include" }
		libdirs { "extern/visual2013/lib" }
		platforms { "x64" }
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image" }

		
	configuration { "windows", "vs2015" }
		if _PREMAKE_VERSION >="5.0" then
			system "Windows"
			architecture "x64"
		end
		includedirs { "extern/visual2015/include" }
		libdirs { "extern/visual2015/lib" }
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image" }
		
	configuration "macosx"
		frameworks= "-F /Library/Frameworks/"
		buildoptions { "-std=c++11" }
		defines { "GK_MACOS" }
		buildoptions { frameworks }
		linkoptions { frameworks .. " -framework OpenGL -framework SDL2 -framework SDL2_image" }


 -- description des fichiers communs
gkit_files = { gkit_dir .. "/src/gKit/*.cpp", gkit_dir .. "/src/gKit/*.h" }

 -- description des projets		

    
projects = {
	"shader_kit"
}

for i, name in ipairs(projects) do
	project(name)
		language "C++"
		kind "ConsoleApp"
		targetdir "bin"
		files ( gkit_files )
		files { gkit_dir .. "/src/" .. name..'.cpp' }
end

 -- description des tutos
tutos = {
	"tuto1",
	"tuto2",
	"tuto3",
	"tuto4",
	"tuto5",
	"tuto6",
	"tuto7",
	"tuto8",
	
	"tuto_transform",
	"tuto_pad",
	
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
		files { gkit_dir .. "/tutos/" .. name..'.cpp' }
end


-- description des tutos openGL avances / M2
tutosM2 = {
	"tuto_time",
	"tuto_mdi"
}

for i, name in ipairs(tutosM2) do
	project(name)
		language "C++"
		kind "ConsoleApp"
		targetdir "bin"
		files ( gkit_files )
		files { gkit_dir .. "/tutos/M2/" .. name..'.cpp' }
end

project("tp2")
	language "C++"
	kind "ConsoleApp"
	targetdir "bin"
	files ( gkit_files )
	files { gkit_dir .. "/opengl_tp2/tp2.cpp" }

-- L2_lifgfx



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
    files ( gkit_files )
    files ( master_CharAnim_files )
	files { gfx_masterCharAnim_dir .. "/src/master_CharAnim/main.cpp" }
