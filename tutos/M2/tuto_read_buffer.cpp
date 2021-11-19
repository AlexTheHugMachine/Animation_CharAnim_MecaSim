
//! \file tuto_read_buffer.cpp relire un buffer privee

#include <vector>

#include "app.h"
#include "program.h"

/*
    cree 2 buffers dans la memoire privee du gpu, soit avec glBufferData(GL_STATIC) comme d'habitude,
    soit avec glBufferStorage() introduit par opengl 4.4
    
    pour relire les resultats qui se trouvent dans un buffer prive du gpu, il faut d'abord copier les valeurs dans un buffer accessible par l'application,
    puis relire enfin les valeurs dans l'application, soit en utilisant glGetBufferSubData() soit en utilisant glMapBuffer()
    
    les differentes solutions sont dispo pour tester, il suffit de definir (ou pas) USE_BUFFER_STORAGE et USE_MAP.
    la configuration choisie est affichee par init().
    
    bilan de l'operation : c'est beaucoup plus simple et direct a faire avec glBufferStorage(), les flags de glBufferData() sont bien penibles a choisir correctement
    remarque : un mauvais choix impactera sans doute les geforce plus que les radeon
 */


//~ #define USE_BUFFER_STORAGE
//~ #define USE_MAP


struct ReadBuffer : public App
{
    ReadBuffer( ) : App(1280, 768, 4,3) {}
    
    int init( )
    {
        m_program= read_program("tutos/M2/read_buffer.glsl");
        program_print_errors(m_program);
        
        std::vector<int> data(1024);

        glGenBuffers(1, &m_gpu_buffer1);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_gpu_buffer1);
    #ifdef USE_BUFFER_STORAGE
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(int) * data.size(), data.data(), 0);
    #else
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * data.size(), data.data(), GL_STATIC_COPY);
    #endif
        
        glGenBuffers(1, &m_gpu_buffer2);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_gpu_buffer2);
    #ifdef USE_BUFFER_STORAGE
        glBufferStorage(GL_SHADER_STORAGE_BUFFER, sizeof(int) * data.size(), nullptr, 0);
    #else
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * data.size(), nullptr, GL_STATIC_COPY);
    #endif
        
        glGenBuffers(1, &m_read_buffer);
        glBindBuffer(GL_COPY_READ_BUFFER, m_read_buffer);
    #ifdef USE_BUFFER_STORAGE
        #ifdef USE_MAP
            glBufferStorage(GL_COPY_READ_BUFFER, sizeof(int) * data.size(), nullptr, GL_CLIENT_STORAGE_BIT | GL_MAP_READ_BIT);
        #else
            glBufferStorage(GL_COPY_READ_BUFFER, sizeof(int) * data.size(), nullptr, GL_CLIENT_STORAGE_BIT);
        #endif
    #else
        glBufferData(GL_COPY_READ_BUFFER, sizeof(int) * data.size(), nullptr, GL_DYNAMIC_READ);
    #endif
    
    #ifdef USE_BUFFER_STORAGE
        printf("!! use buffer storage\n");
    #else
        printf("!! use buffer data\n");
    #endif
    
        return 0;
    }
    
    int quit( )
    {
        release_program(m_program);
        glDeleteBuffers(1, &m_gpu_buffer1);
        glDeleteBuffers(1, &m_gpu_buffer2);
        glDeleteBuffers(1, &m_read_buffer);
        
        return 0;
    }
    
    int render( )
    {
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_gpu_buffer1);
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, m_gpu_buffer2);
        
        glUseProgram(m_program);
        glDispatchCompute(4, 1, 1);
        
        glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);

        glBindBuffer(GL_COPY_READ_BUFFER, m_read_buffer);
        glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_COPY_READ_BUFFER, 0, 0, sizeof(int)*1024);
        
    #ifdef USE_MAP
        printf("!! use map\n");
        
        int *tmp= (int *) glMapBuffer(GL_COPY_READ_BUFFER, GL_READ_ONLY);
        {
            for(unsigned i= 0; i < 1024; i++)
                printf("%d ", tmp[i]);
            printf("\n");
        }
        glUnmapBuffer(GL_COPY_READ_BUFFER);

    #else
        printf("!! use get buffer\n");
        
        std::vector<int> tmp(1024);
        glGetBufferSubData(GL_COPY_READ_BUFFER, 0, sizeof(int) * tmp.size(), tmp.data());
        for(unsigned i= 0; i < tmp.size(); i++)
            printf("%d ", tmp[i]);
        printf("\n");
    #endif
        
        return 0;
    }
    
    GLuint m_gpu_buffer1;
    GLuint m_gpu_buffer2;
    GLuint m_read_buffer;
    GLuint m_program;
};

int main( )
{
    ReadBuffer app;
    app.run();
    
    return 0;
}
