
#include <cstddef>
#include <cstdio>
#include <cstdlib>
#include <ctime>

#include <vector>
#include <chrono>

#include "window.h"

#include "vec.h"
#include "program.h"
#include "uniforms.h"


GLuint program= 0;
GLuint input_buffer= 0;
GLuint tmp_buffer= 0;

std::vector<int> input;


const int logNmax= 24;

int init( )
{
    srand(time(nullptr));
        
    int o= rand() % 10000;
    for(int i= 0; i < (1<<logNmax); i++)
        input.push_back( o + rand() % 10000);
        //~ input.push_back( N - 1 - i );
    
    glGenBuffers(1, &input_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, input_buffer);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * input.size(), input.data(), GL_STREAM_READ);
    
    glGenBuffers(1, &tmp_buffer);
    glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 1, tmp_buffer);
    //~ glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * input.size(), nullptr, GL_STREAM_READ);
    glBufferData(GL_SHADER_STORAGE_BUFFER, sizeof(int) * input.size(), input.data(), GL_STREAM_READ);
    
    //~ program= read_program("tutos/min_data.glsl");
    program= read_program("tutos/min_atomic.glsl");
    program_print_errors(program);
    
    return 0;
}

int quit( )
{
    release_program(program);
    glDeleteBuffers(1, &input_buffer);
    glDeleteBuffers(1, &tmp_buffer);
    return 0;
}


int main( int argc, char **argv )
{
    // etape 1 : creer la fenetre
    Window window= create_window(1024, 640);
    if(window == NULL)
        return 1;

    // etape 2 : creer un contexte opengl pour pouvoir dessiner
    Context context= create_context(window, 4,3);       // openGL version 4.3
    if(context == NULL)
        return 1;

    // etape 3 : creation des objets
    if(init() < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }

    // verification 
    // taille des groupes
    GLint threads_max= 0;
    glGetIntegerv(GL_MAX_COMPUTE_WORK_GROUP_INVOCATIONS, &threads_max);
    printf("threads max %d\n", threads_max);
    
    // dimensions des groupes
    GLint size_max[3]= { };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 0, &size_max[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 1, &size_max[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_SIZE, 2, &size_max[2]);
    printf("size max %d %d %d\n", size_max[0], size_max[1], size_max[2]);
    
    // nombre de groupes
    GLint groups_max[3]= { };
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 0, &groups_max[0]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 1, &groups_max[1]);
    glGetIntegeri_v(GL_MAX_COMPUTE_WORK_GROUP_COUNT, 2, &groups_max[2]);
    printf("groups max %d %d %d\n", groups_max[0], groups_max[1], groups_max[2]);
    printf("\n");
    
    //~ FILE *out= fopen("min_data.txt", "wt");
    //~ FILE *out= fopen("min_atomic.txt", "wt");
    FILE *out= fopen("min_atomic_group.txt", "wt");
    for(int logN= 1; logN < logNmax; logN++)
    {
        int N= (1<<logN);
        
        // run
        glUseProgram(program);
        
        // mesure le temps d'execution du dispatch 
        // utilise std::chrono pour mesurer le temps cpu 
        std::chrono::high_resolution_clock::time_point start= std::chrono::high_resolution_clock::now();

        program_uniform(program, "N", (unsigned int) N);
        //~ program_uniform(program, "n", (unsigned int) 0);
        glDispatchCompute((N+1024) / 1024, 1, 1);
        
        //~ program_uniform(program, "N", (unsigned int) 0);
        //~ for(int n= N/2; n > 1; n= n/2)
        //~ {
            //~ program_uniform(program, "n", (unsigned int) n);

            //~ glMemoryBarrier(GL_SHADER_STORAGE_BARRIER_BIT);
            //~ glDispatchCompute((n+1024) / 1024, 1, 1);
        //~ }
        
        // recupere les resultats tmp
        //~ glMemoryBarrier(GL_BUFFER_UPDATE_BARRIER_BIT);
        glMemoryBarrier(GL_ALL_BARRIER_BITS);
        
        std::chrono::high_resolution_clock::time_point stop= std::chrono::high_resolution_clock::now();
        // conversion des mesures en duree...
        long long int gpu_time= std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
        
        std::vector<int> tmp(input.size(), 0);
        glBindBuffer(GL_SHADER_STORAGE_BUFFER, tmp_buffer);
        glGetBufferSubData(GL_SHADER_STORAGE_BUFFER, 0, sizeof(int) * tmp.size(), tmp.data());
        
        //~ for(int i= 0; i < (int) tmp.size(); i++)
            //~ printf("%d ", tmp[i]);
        printf("gpu min %d\n", tmp[0]);
        printf(" %02dms %03dus\n", (int) (gpu_time / 1000000), (int) ((gpu_time / 1000) % 1000));
        
        {
            // verification
            std::chrono::high_resolution_clock::time_point start= std::chrono::high_resolution_clock::now();
            
            int m= input[0];
            for(int i= 1; i < N; i++)
                    m= std::min(m, input[i]);
            
            std::chrono::high_resolution_clock::time_point stop= std::chrono::high_resolution_clock::now();
            // conversion des mesures en duree...
            long long int cpu_time= std::chrono::duration_cast<std::chrono::nanoseconds>(stop - start).count();
            
            printf("cpu min %d\n", m);
            printf(" %02dms %03dus\n", (int) (cpu_time / 1000000), (int) ((cpu_time / 1000) % 1000));
            
            fprintf(out, "%d %f %f\n", N, gpu_time / 1000000.f, cpu_time / 1000000.f);
            
            printf("\n");
        }
    }
    fclose(out);

    // etape 5 : nettoyage
    quit();
    release_context(context);
    release_window(window);
    return 0;
}

