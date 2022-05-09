
#include <cstdio>

#include "app_time.h"
#include "texture.h"


AppTime::AppTime( const int width, const int height, const int major, const int minor, const int samples ) 
    : App(width, height, major, minor, samples)
{
    // desactive vsync pour les mesures de temps
    SDL_GL_SetSwapInterval(0);
    printf("[Apptime] vsync OFF...\n");
    
    // requete pour mesurer le temps gpu
    glGenQueries(1, &m_time_query);
    
    // affichage du temps  dans la fenetre
    m_console= create_text();
}

AppTime::~AppTime( )
{
    glDeleteQueries(1, &m_time_query);
    release_text(m_console);
}

int AppTime::prerender( )
{ 
    // mesure le temps d'execution du draw pour le gpu
    glBeginQuery(GL_TIME_ELAPSED, m_time_query);
    
    // mesure le temps d'execution du draw pour le cpu
    // utilise std::chrono pour mesurer le temps cpu 
    m_cpu_start= std::chrono::high_resolution_clock::now();
    
    return update(global_time(), delta_time());
}

int AppTime::postrender( )
{
    m_cpu_stop= std::chrono::high_resolution_clock::now();
    // conversion des mesures en duree...
    int cpu_time= std::chrono::duration_cast<std::chrono::microseconds>(m_cpu_stop - m_cpu_start).count(); 
    
    glEndQuery(GL_TIME_ELAPSED);
    
    // attendre le resultat de la requete
    GLint64 gpu_time= 0;
    glGetQueryObjecti64v(m_time_query, GL_QUERY_RESULT, &gpu_time);
    
    // afficher le texte
    clear(m_console);        
    printf(m_console, 0, 1, "cpu  %02dms %03dus", cpu_time / 1000, cpu_time % 1000);
    printf(m_console, 0, 2, "gpu  %02dms %03dus", int(gpu_time / 1000000), int((gpu_time / 1000) % 1000));
    
    // affiche le temps dans le terminal 
    //~ printf("cpu  %02dms %03dus    ", cpu_time / 1000, cpu_time % 1000);
    //~ printf("gpu  %02dms %03dus\n", int(gpu_time / 1000000), int((gpu_time / 1000) % 1000));
    
    draw(m_console, window_width(), window_height());
    
    return 0;
}
