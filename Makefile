
SRCS = tutos/tuto5.cpp \
	src/gKit/window.cpp src/gKit/vec.cpp src/gKit/program.cpp src/gKit/mesh.cpp src/gKit/image.cpp src/gKit/color.cpp \
	src/gKit/wavefront.cpp src/gKit/texture.cpp src/gKit/orbiter.cpp src/gKit/mat.cpp src/gKit/buffer.cpp src/gKit/draw.cpp \
	src/gKit/text.cpp src/gKit/widgets.cpp

FINAL_TARGET = gKit2light

ifeq ($(OS),Windows_NT)
	LIBS = 	-Lextern \
		-Lextern/SDL2_mingw/SDL2-2.0.3/i686-w64-mingw32/lib \
		-Lextern/SDL2_mingw/SDL2_image-2.0.0/i686-w64-mingw32/lib \
		-Lextern/glew-1.10.0_mingw/lib \
		-lmingw32 -lSDL2main -lSDL2.dll -lSDL2_image.dll -lglew32 -lopengl32
	INCLUDELIBS_DIR = -Iextern/SDL2_mingw/SDL2-2.0.3/include \
		-Iextern/SDL2_mingw/SDL2-2.0.3/include/SDL2 \
		-Iextern/SDL2_mingw/SDL2_image-2.0.0/i686-w64-mingw32/include \
		-Iextern/glew-1.10.0_mingw/include \
		-Isrc/viewer \
		-Isrc/gKit
else
	LIBS = -lSDL2 -lSDL2_image -lGLEW -lGL
	INCLUDELIBS_DIR = -I /usr/include/SDL2 -I src/gKit -I src/viewer
endif

CC		= g++
LD 		= g++
LDFLAGS  	=
CPPFLAGS 	= -D_USE_MATH_DEFINES  -Wall -g #-O2   # pour optimiser
INCLUDE_DIR	= -I src $(INCLUDELIBS_DIR)
SRC_DIR 	= src
BIN_DIR 	= bin
OBJ_DIR 	= bin
OBJ_DIRS 	= $(OBJ_DIR)/viewer $(OBJ_DIR)/src/gKit $(OBJ_DIR)/tutos



default: make_dir $(BIN_DIR)/$(FINAL_TARGET)

make_dir:
	mkdir -p $(OBJ_DIRS)

$(BIN_DIR)/$(FINAL_TARGET): $(SRCS:%.cpp=$(OBJ_DIR)/%.o)
	$(LD) $+ -o $@ $(LDFLAGS) $(LIB_DIR) $(LIBS)

$(OBJ_DIR)/%.o: %.cpp
	$(CC) -c $(CPPFLAGS) $(DEFINE) $(INCLUDE_DIR) $< -o $@

clean:
	rm -rf $(OBJ_DIRS) $(BIN_DIR)/$(FINAL_TARGET)
