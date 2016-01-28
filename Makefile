
SRCS =	 	viewer/mini_gl3core.cpp \
			gKit/window.cpp     gKit/vec.cpp      gKit/program.cpp  gKit/mesh.cpp  gKit/image.cpp \
			gKit/wavefront.cpp  gKit/texture.cpp  gKit/orbiter.cpp  gKit/mat.cpp   gKit/buffer.cpp \


FINAL_TARGET = gKit2light

ifeq ($(OS),Windows_NT)
	LIBS = 	-Lextern \
			-Lextern/SDL2_mingw/SDL2-2.0.3/i686-w64-mingw32/lib \
			-Lextern/SDL2_mingw/SDL2_ttf-2.0.12/i686-w64-mingw32/lib \
			-Lextern/SDL2_mingw/SDL2_image-2.0.0/i686-w64-mingw32/lib \
			-Lextern/glew-1.10.0_mingw/lib \
			-lmingw32 -lSDL2main -lSDL2.dll -lSDL2_ttf.dll -lSDL2_image.dll -lglew32 -lopengl32
	INCLUDELIBS_DIR = 	-Iextern/SDL2_mingw/SDL2-2.0.3/include \
						-Iextern/SDL2_mingw/SDL2-2.0.3/include/SDL2 \
						-Iextern/SDL2_mingw/SDL2_ttf-2.0.12/i686-w64-mingw32/include/SDL2 \
						-Iextern/SDL2_mingw/SDL2_image-2.0.0/i686-w64-mingw32/include \
						-Iextern/glew-1.10.0_mingw/include \
						-Isrc/viewer \
						-Isrc/gKit
else
	LIBS = -lSDL2 -lSDL2_ttf -lSDL2_image -lGLEW -lGL
	INCLUDELIBS_DIR = -I/usr/include/SDL2 -Isrc/gKit -I/src/viewer
endif

CC					= g++
LD 					= g++
LDFLAGS  			=
CPPFLAGS 			= -D_USE_MATH_DEFINES  -Wall -pedantic -ansi -ggdb   -fpermissive #-O2   # pour optimiser
INCLUDE_DIR	 		= -Isrc $(INCLUDELIBS_DIR)
OBJ_DIR 			= bin
SRC_DIR 			= src
BIN_DIR 			= bin



default: $(BIN_DIR)/viewer $(BIN_DIR)/gKit $(BIN_DIR)/$(FINAL_TARGET)

$(BIN_DIR)/viewer:
	mkdir -p $(BIN_DIR)/viewer

$(BIN_DIR)/gKit:
	mkdir -p $(BIN_DIR)/gKit

$(BIN_DIR)/$(FINAL_TARGET): $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
	$(LD) $+ -o $@ $(LDFLAGS) $(LIB_DIR) $(LIBS)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	$(CC) -c $(CPPFLAGS) $(DEFINE) $(INCLUDE_DIR) $< -o $@


clean:
	find $(OBJ_DIR) -name "*.o" -exec rm {} \;  ; rm $(BIN_DIR)/$(FINAL_TARGET)*
