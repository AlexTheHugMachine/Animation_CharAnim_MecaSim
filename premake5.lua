workspace "gkit2"

	configurations { "debug", "release" }
	platforms { "x64", "x32" }
	
	location "build"
	debugdir "./"
	
	includedirs { ".", "src/gKit"}
	
	 -- description des fichiers communs
	gkit_dir = path.getabsolute(".")
	gkit_files = { gkit_dir .. "/src/gKit/*.cpp", gkit_dir .. "/src/gKit/*.h" }

	configuration "debug"
		symbols "on"
	
	configuration "release"
		optimize "speed"
		
	configuration "linux"
		removeplatforms { "x32" }
		buildoptions { "-mtune=native -march=native" }
		cppdialect "c++11"
		buildoptions { "-W -Wall -Wextra -Wsign-compare -Wno-unused-parameter -Wno-unused-function -Wno-unused-variable", "-pipe" }
		buildoptions { "-flto"}
		linkoptions { "-flto"}
		links { "GLEW", "SDL2", "SDL2_image", "GL" }

	configuration { "linux", "debug" }
		linkoptions { "-g"}	-- bug : premake ne genere pas l'option "-g" pour le linker

	configuration { "linux", "release" }
		buildoptions { "-fopenmp" }
		linkoptions { "-fopenmp" }

	configuration { "windows" }
		defines { "WIN32", "_USE_MATH_DEFINES", "_CRT_SECURE_NO_WARNINGS" }
		defines { "NOMINMAX" }	-- bug visual studio : autoriser explicitement les fonctions std::min et std::max...
		cppdialect "c++11"
	
	configuration { "windows", "gmake" }
		removeplatforms { "x64" }
		includedirs { "extern/mingw/include" }
		libdirs { "extern/mingw/lib" }
		links { "mingw32", "SDL2main", "SDL2", "SDL2_image", "opengl32", "glew32" }
	
	configuration { "windows", "codeblocks" }
		removeplatforms { "x64" }
		includedirs { "extern/mingw/include" }
		libdirs { "extern/mingw/lib" }
		links { "mingw32", "SDL2main", "SDL2", "SDL2_image", "opengl32", "glew32" }
	
	
	configuration { "windows", "vs2017" }
		removeplatforms { "x32" }
		system "Windows"
		architecture "x64"
		disablewarnings { "4244", "4305" }
		flags { "MultiProcessorCompile", "NoMinimalRebuild" }
	
		includedirs { "extern/visual/include" }
		libdirs { "extern/visual/lib" }
		links { "opengl32", "glew32", "SDL2", "SDL2main", "SDL2_image" }

		
	configuration "macosx"
		frameworks= "-F /Library/Frameworks/"
		buildoptions { "-std=c++11" }
		defines { "GK_MACOS" }
		buildoptions { frameworks }
		linkoptions { frameworks .. " -framework OpenGL -framework SDL2 -framework SDL2_image" }

 -- \todo reprendre la logique d'inclusion		
if no_project then
	do return end
end

		
 -- description des projets
projects = {
	"shader_kit",
	"image_viewer"
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
	"tuto9",
	"tuto9_texture1",
	"tuto9_textures",
	"tuto10",
	
	"tuto_transform",
	"tuto_pad",
	
	"tuto1GL",
	"tuto2GL",
	"tuto2GL_app",
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
	"tuto_storage",
	"tuto_storage_buffer",
	"tuto_storage_texture",
	
	"min_data"
}

for i, name in ipairs(tutos) do
	project(name)
		language "C++"
		kind "ConsoleApp"
		targetdir "bin"
		files ( gkit_files )
		files { gkit_dir .. "/tutos/" .. name..'.cpp' }
end

tutos_m1 = {
	"tp1"
}

for i, name in ipairs(tutos_m1) do
	project(name .. "m1")
		language "C++"
		kind "ConsoleApp"
		targetdir "bin"
		files ( gkit_files )
		files { gkit_dir .. "/m1/" .. name..'.cpp' }
end

--~  -- description des tutos serie 2
--~  tutos2 = {
--~ 	"tuto1",
--~ 	"tuto2"
--~ }

--~ for i, name in ipairs(tutos2) do
--~ 	project(name .. "_serie2")
--~ 		language "C++"
--~ 		kind "ConsoleApp"
--~ 		targetdir "bin"
--~ 		files ( gkit_files )
--~ 		files { gkit_dir .. "/tutos/serie2/" .. name..'.cpp' }
--~ end


project("mesh_viewer")
	language "C++"
	kind "ConsoleApp"
	targetdir "bin"
	files ( gkit_files )
	files { gkit_dir .. "/tutos/mesh_viewer.cpp"}
	files { gkit_dir .. "/tutos/mesh_buffer.cpp"}
	files { gkit_dir .. "/tutos/mesh_buffer.h"}
	files { gkit_dir .. "/tutos/mesh_data.cpp"}
	files { gkit_dir .. "/tutos/mesh_data.h"}
	files { gkit_dir .. "/tutos/material_data.cpp"}
	files { gkit_dir .. "/tutos/material_data.h"}


-- description des tutos openGL avances / M2
tutosM2 = {
	"tuto_time",
	"tuto_mdi",
	"tuto_is",
	"tuto_raytrace_fragment"
}

for i, name in ipairs(tutosM2) do
	project(name)
		language "C++"
		kind "ConsoleApp"
		targetdir "bin"
		files ( gkit_files )
		files { gkit_dir .. "/tutos/M2/" .. name..'.cpp' }
end
