
#ifndef _GK_GL3CORE_H

#ifdef __APPLE__
    #include <OpenGL/gl3.h>
    #ifndef  CGL_VERSION_1_3
    #error openGL core profile not supported
    #endif
#else
    // windows et linux
    #define GLEW_NO_GLU
    #include "GL/glew.h"
#endif

#endif
