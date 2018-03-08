
#include <cstdio>

#include "app_time.h"
#include "texture.h"


AppTime::AppTime( const int width, const int height, const int major, const int minor, const int samples ) 
    : App(width, height, major, minor, samples)
{
    // desactive vsync pour les mesures de temps
    SDL_GL_SetSwapInterval(0);
    printf("[Apptime] vsync OFF...\n");
    
#ifndef __EMSCRIPTEN__
    // requete pour mesurer le temps gpu
    glGenQueries(1, &m_time_query);
#endif
    
    // affichage du temps  dans la fenetre
    m_console= create_text();
    
    // configure openGL
    glViewport(0, 0, window_width(), window_height());
    
    // remarque : utiliser std::chrono si la precision n'est pas suffisante
    while(events(m_window))
    {
    #if 0
        if(laptop_mode() && last_event_count() == 0)
        {
            SDL_Delay(16);
            continue;
        }
    #endif
        
        if(update(global_time(), delta_time()) < 0)
            break;
        
#ifndef __EMSCRIPTEN__
        // mesure le temps d'execution du draw pour le gpu
        glBeginQuery(GL_TIME_ELAPSED, m_time_query);
#endif
        
        // mesure le temps d'execution du draw pour le cpu
        // utilise std::chrono pour mesurer le temps cpu 
        std::chrono::high_resolution_clock::time_point cpu_start= std::chrono::high_resolution_clock::now();
        
        int code= render();
       
        std::chrono::high_resolution_clock::time_point cpu_stop= std::chrono::high_resolution_clock::now();
        // conversion des mesures en duree...
        int cpu_time= std::chrono::duration_cast<std::chrono::nanoseconds>(cpu_stop - cpu_start).count();
        
#ifndef __EMSCRIPTEN__
        glEndQuery(GL_TIME_ELAPSED);
#endif
        
        // force openGL a finir d'executer toutes les commandes, cf App::run()
        glFinish();
        
        if(code< 1)
            break;
        
        // attendre le resultat de la requete
        GLint64 gpu_time= 0;
#ifndef __EMSCRIPTEN__
        glGetQueryObjecti64v(m_time_query, GL_QUERY_RESULT, &gpu_time);
#endif
        
        // afficher le texte
        clear(m_console);        
        printf(m_console, 0, 1, "cpu  %02dms %03dus", (int) (cpu_time / 1000000), (int) ((cpu_time / 1000) % 1000));
        printf(m_console, 0, 2, "gpu  %02dms %03dus", (int) (gpu_time / 1000000), (int) ((gpu_time / 1000) % 1000));        
        
        draw(m_console, window_width(), window_height());
        
        if(key_state('s'))
        {
            clear_key_state('s');
            
            static int calls= 1;
            screenshot("gkit2app", calls++);
        }
        
        // presenter le resultat
        SDL_GL_SwapWindow(m_window);
    }
    
    if(quit() < 0)
        return -1;
    
#ifndef __EMSCRIPTEN__
    glDeleteQueries(1, &m_time_query);
#endif
    release_text(m_console);    
    
    return 0;    
}
