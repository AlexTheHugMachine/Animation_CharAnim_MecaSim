
//! \file app.cpp

#include <cstdio>
#include <cstring>

#include "app.h"
#include "glcore.h"

#ifdef __EMSCRIPTEN__
#include "emscripten.h"
#endif

App::App( const int width, const int height, const int major, const int minor, const int samples )
    : m_window(nullptr), m_context(nullptr)
{
    m_window= create_window(width, height, major, minor, samples);
    m_context= create_context(m_window);
}

App::~App( )
{
    if(m_context)
        release_context(m_context);
    if(m_window)
        release_window(m_window);
}

#ifdef __EMSCRIPTEN__
void App::loop_iteration(void* instance) {
  App* app = (App*) instance ;
  if(!events(app->m_window)) {
    emscripten_cancel_main_loop() ;
  }
  if(app->update(global_time(), delta_time()) < 0) {
    emscripten_cancel_main_loop() ;
  }

  if(app->prerender() < 0) {
    emscripten_cancel_main_loop() ;
  }

  if(app->render() < 1) {
    emscripten_cancel_main_loop() ;
  }

  if(app->postrender() < 0) {
    emscripten_cancel_main_loop() ;
  }
  SDL_GL_SwapWindow(app->m_window);
}
#endif

int App::run( )
{
    if(init() < 0)
        return -1;
    
    // configure openGL
    glViewport(0, 0, window_width(), window_height());

#ifdef __EMSCRIPTEN__
    emscripten_set_main_loop_arg(App::loop_iteration, this, 0, 1) ;
#else
    // gestion des evenements
    while(events(m_window))
    {
        if(prerender() < 0)
            break;
        
        if(render() < 1)
            break;
        
        if(postrender() < 0)
            break;
        
        // presenter le resultat
        SDL_GL_SwapWindow(m_window);
        
        // force openGL a finir d'executer toutes les commandes, 
        // cf https://www.khronos.org/opengl/wiki/Swap_Interval#GPU_vs_CPU_synchronization
        // devrait limiter la consommation sur portable
        glFinish();
    }
    
    if(quit() < 0)
        return -1;
#endif
    return 0;
}
