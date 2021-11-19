
#include <chrono>

#include "app.h"

#include "color.h"
#include "image.h"
#include "image_io.h"

#include "program.h"
#include "uniforms.h"

struct Histogram : public App
{
    Histogram( const char *filename ) : App(1024, 640, 4,3)
    {
        m_image= read_image_data(filename);
    }
    
    int init( )
    {
        if(m_image.pixels.size() == 0)
            // pas d'image
            return -1;
        
        // cree la texture pour stocker l'image
        GLenum data_type= GL_UNSIGNED_BYTE;
        GLenum data_format= GL_RGBA;
        if(m_image.channels == 1)
            data_format= GL_RED;
        else if(m_image.channels == 2)
            data_format= GL_RG;
        else if(m_image.channels == 3)
            data_format= GL_RGB;
        //~ else
            //~ data_format= GL_RGBA;
        
        glGenTextures(1, &m_texture);
        glBindTexture(GL_TEXTURE_2D, m_texture);
        glTexImage2D(GL_TEXTURE_2D, 0,
            GL_RGBA8, m_image.width, m_image.height, 0,
            data_format, data_type, m_image.data());
        
        // pas la peine de construire les mipmaps, le shader ne va ecrire que le mipmap 0
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0);
        
        // cree le buffer pour stocker l'histogramme
        glGenBuffers(1, &m_histogram);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, m_histogram);
        glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * 16, nullptr, GL_STATIC_COPY);
        
        m_program= read_program("tutos/M2/histogram2.glsl");
        program_print_errors(m_program);
        
        glGetProgramiv(m_program, GL_COMPUTE_WORK_GROUP_SIZE, m_threads);
        
        // mesure temps gpu
        glGenQueries(1, &m_time_query);
        
        // histogramme cpu pour comparer...
        {
            auto start= std::chrono::high_resolution_clock::now();
            
            int histogram[16]= { 0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0,  0, 0, 0, 0 };
            for(int y= 0; y < m_image.height; y++)
            for(int x= 0; x < m_image.width; x++)
            {
                size_t offset= m_image.offset(x, y);
                int r= m_image.pixels[offset];
                int g= m_image.pixels[offset+1];
                int b= m_image.pixels[offset+2];
                int bin= 15 * (r+g+b) / (255*3);
                histogram[bin]++;
            }
            
            auto stop= std::chrono::high_resolution_clock::now();
            int cpu_time= std::chrono::duration_cast<std::chrono::microseconds>(stop - start).count();
            
            for(int i= 0; i < 16; i++)
                printf("bin %d: %d pixels, %d%%\n", i, histogram[i], 100*histogram[i] / (m_image.width * m_image.height));
            
            printf("cpu %dus\n", cpu_time);
        }
        
        return 1;
    }
    
    int quit( ) 
    {
        release_program(m_program);
        glDeleteBuffers(1, &m_histogram);
        glDeleteTextures(1, &m_texture);
        glDeleteQueries(1, &m_time_query);
        return 0;
    }
    
    int render( )
    {
        glViewport(0, 0, window_width(), window_height());
        glClear(GL_COLOR_BUFFER_BIT);
        
        glUseProgram(m_program);
        
        // storage buffer 0
        glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, m_histogram);
        // remet a zero l'histogramme
        glClearBufferData(GL_SHADER_STORAGE_BUFFER, GL_R32UI, GL_RED_INTEGER, GL_UNSIGNED_INT, 0);
        
        // image texture 0, ecriture seule, mipmap 0 + format rgba8 classique
        glBindImageTexture(0, m_texture, 0, GL_FALSE, 0, GL_READ_ONLY, GL_RGBA8);
        // configurer le shader
        program_uniform(m_program, "image", 0);

        int nx= m_image.width / m_threads[0];
        int ny= m_image.height / m_threads[1];
        // on suppose que les dimensions de l'image sont multiples de 8...
        // sinon calculer correctement le nombre de groupes pour x et y. 
        
        glBeginQuery(GL_TIME_ELAPSED, m_time_query);
        {
            // go !!
            //~ for(int i= 0; i < 100; i++)     // eventuellement recommencer plein de fois pour stabiliser les frequences du gpu...
            glDispatchCompute(nx, ny, 1);
            
            // attendre le resultat
            glMemoryBarrier(GL_ALL_BARRIER_BITS);
        }
        glEndQuery(GL_TIME_ELAPSED);
        
        // relire le buffer resultat
        int histogram[16];
        {
            // creer un buffer temporaire pour le transfert depuis le buffer resultat
            GLuint buffer= 0;
            glGenBuffers(1, &buffer);
            glBindBuffer(GL_COPY_READ_BUFFER, buffer);
            glBufferData(GL_COPY_READ_BUFFER, sizeof(int) * 16, nullptr, GL_DYNAMIC_READ);
            
            // copie les resultats
            glCopyBufferSubData(GL_SHADER_STORAGE_BUFFER, GL_COPY_READ_BUFFER, 0, 0, sizeof(int) * 16);
            
            // recupere les resultats depuis le buffer intermediaire
            glGetBufferSubData(GL_COPY_READ_BUFFER, 0, sizeof(int) * 16, histogram);
            
            // detruit le buffer intermediaire
            glDeleteBuffers(1, &buffer);
        }
        
        for(int i= 0; i < 16; i++)
            printf("bin %d: %d pixels, %d%%\n", i, histogram[i], 100*histogram[i] / (m_image.width * m_image.height));
        
        printf("\n%dx%d groups, %d threads\n", nx, ny, nx*ny*m_threads[0]*m_threads[1]);
        
        // attendre le resultat de la requete
        GLint64 gpu_time= 0;
        glGetQueryObjecti64v(m_time_query, GL_QUERY_RESULT, &gpu_time);
        //~ gpu_time/= 100;
        printf("gpu  %02dms %03dus\n\n", int(gpu_time / 1000000), int((gpu_time / 1000) % 1000));    
        
        return 0;       // une seule fois
        //~ return 1;   // recommencer jusqu'a la fermeture de la fenetre...
    }
    
    ImageData m_image;
    GLuint m_program;
    GLuint m_time_query;
    GLuint m_histogram;
    GLuint m_texture;
    GLint m_threads[3];
};


    
int main( int argc, char **argv )
{
    const char *filename= "data/papillon.png";
    if(argc > 1)
        filename= argv[1];
    
    Histogram app(filename);
    app.run();
    
    return 0;
}
