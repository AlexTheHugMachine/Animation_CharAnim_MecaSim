
//! \file tuto5GL_dynamic_cubemap.cpp rendu dans une cubemap en 1 passe : draw instancie + vertex gl_Layer 

#include <stdio.h>

#include "window.h"
#include "vec.h"
#include "mat.h"

#include "program.h"
#include "uniforms.h"

#include "mesh.h"
#include "wavefront.h"

#include "orbiter.h"
#include "image_io.h"


GLuint program;
GLuint program_cubemap;
GLuint program_render_cubemap;

GLuint texture_cubemap;
GLuint depth_cubemap;
GLuint framebuffer;

GLuint vao;
GLuint vao_null;
GLuint vertex_buffer;
GLuint normal_buffer;
int vertex_count;

Orbiter camera;

int init( )
{
    // etape 1 : shaders
    // . dessiner le reflet de la cubemap sur un objet
    program= read_program("tutos/cubemap.glsl");
    program_print_errors(program);
    
    // . dessiner la cubemap a l'infini
    program_cubemap= read_program("tutos/cubemap.glsl");
    program_print_errors(program_cubemap);
    
    // . dessiner dans une cube map
    program_render_cubemap= read_program("tutos/render_cubemap.glsl");
    program_print_errors(program_render_cubemap);
    
    // etape 2 : charger un mesh, (avec des normales), vao + vertex buffer
    Mesh mesh= read_mesh("data/bigguy.obj");
    //~ Mesh mesh= read_mesh("data/cube.obj");
    if(mesh.vertex_count() == 0)
        return -1;      // gros probleme, pas de sommets...

    vertex_count= mesh.vertex_count();
    
    Point pmin, pmax;
    mesh.bounds(pmin, pmax);
    camera.lookat(pmin, pmax);
    
    // vertex format : position + normal,
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);
    
    // vertex buffer
    glGenBuffers(1, &vertex_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, vertex_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.vertex_buffer_size(), mesh.vertex_buffer(), GL_STATIC_DRAW);
    
    // configurer l'attribut position, cf declaration dans le vertex shader : in vec3 position;
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(0);
    
    // normal buffer
    if(!mesh.has_normal())
    {
        printf("[oops]  pas de normales...\n");
        return -1;
    }
    
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.normal_buffer_size(), mesh.normal_buffer(), GL_STATIC_DRAW);
    
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, 0);
    glEnableVertexAttribArray(1);
    
    // nettoyage
    mesh.release();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // etape 3 : textures + framebuffer
    {
        int w= 1024;
        int h= 1024;
        
        // 6 faces couleur
        glGenTextures(1, &texture_cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap);
        for(int i= 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0,
                GL_RGBA, w, h, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        }
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);    
        
        // 6 faces profondeur
        glGenTextures(1, &depth_cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, depth_cubemap);
        for(int i= 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0,
                GL_DEPTH_COMPONENT, w, h, 0,
                GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
        }
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);        
    }

    // framebuffer, attache les 6 faces couleur + profondeur
    glGenFramebuffers(1, &framebuffer);
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, texture_cubemap, 0);
    glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, depth_cubemap, 0);
    
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    

    // filtrage "correct" sur les bords du cube...
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    
    // etape 4 : vao pour dessiner la cubemap a l'infini
    glGenVertexArrays(1, &vao_null);
    glBindVertexArray(vao_null);
    // pas de buffer, c'est le vertex shader qui genere directement les positions des sommets
    
    glUseProgram(0);
    glBindVertexArray(0);
    
    // etat par defaut
    glClearColor(0.2f, 0.2f, 0.2f, 1);
    glClearDepthf(1);

    glDepthFunc(GL_LEQUAL);     // !! attention !! le support de la cube map est dessine exactement sur le plan far
    // et le test "classique" GL_LESS est toujours faux, la cubemap ne sera pas dessinee.
    
    glEnable(GL_DEPTH_TEST);

    glFrontFace(GL_CCW);
    glCullFace(GL_BACK);
    //~ glEnable(GL_CULL_FACE);
    glDisable(GL_CULL_FACE);
    return 0;
}

int quit( )
{
    release_program(program);
    release_program(program_cubemap);
    release_program(program_render_cubemap);
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &vao_null);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &normal_buffer);
    glDeleteTextures(1, &texture_cubemap);
    glDeleteTextures(1, &depth_cubemap);
    glDeleteFramebuffers(1, &framebuffer);
    return 0;
}

int draw( )
{
    // partie 1 : dessiner dans la cubemap dynamique
    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer);
    glViewport(0, 0, 1024, 1024);
    
    // efface les 6 faces couleur + profondeur attachees au framebuffer
    //~ glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    float black[4]= { 0, 0, 0, 0 };
    glClearBufferfv(GL_COLOR, 0, black);
    
    float one= 1;
    glClearBufferfv(GL_DEPTH, 0, &one);
    
    {
        // prepare les 6 matrices view, une par face de la cubemap
        // !! attention a la direction 'up' de lookat... rappel : orientation des textures des cubemaps...
        Transform faces[6];
        faces[0]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(1, 0, 0),  /* up */ Vector(0, -1, 0));   // +X
        faces[1]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(-1, 0, 0), /* up */ Vector(0, -1, 0));   // -X
        
        faces[2]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(0, 1, 0),  /* up */ Vector(0, 0, 1));    // +Y
        faces[3]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(0, -1, 0), /* up */ Vector(0, 0, -1));   // -Y
        
        faces[4]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(0, 0, 1),  /* up */ Vector(0, -1, 0));   // +Z
        faces[5]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(0, 0, -1), /* up */ Vector(0, -1, 0));   // -Z
        
        Transform model= Identity();
        Transform projection= Perspective(45, 1, 0.01, 100);
        
        // dessine 6 fois la scene
        glBindVertexArray(vao);
        glUseProgram(program_render_cubemap);
        glUniformMatrix4fv(glGetUniformLocation(program_render_cubemap, "projectionMatrix"), 1, GL_TRUE, projection.buffer());
        glUniformMatrix4fv(glGetUniformLocation(program_render_cubemap, "modelMatrix"), 1, GL_TRUE, model.buffer());
        glUniformMatrix4fv(glGetUniformLocation(program_render_cubemap, "viewMatrix"), 6, GL_TRUE, faces[0].buffer());
        
        // mais avec un seul draw... qui dessine chaque triangle 6 fois, une fois par face de la cubemap, avec les 6 transformations correspondantes
        glDrawArraysInstanced(GL_TRIANGLES, 0, vertex_count, 6);
    }
    
    // mise a jour des mipmaps des faces de la cubemap
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap);
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    // partie 2 : dessiner avec la cubemap dynamique
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, window_width(), window_height());
    
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    
    // recupere les mouvements de la souris, utilise directement SDL2
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);

    // deplace la camera
    if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
        // tourne autour de l'objet
        camera.rotation(mx, my);

    else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
        // approche / eloigne l'objet
        camera.move(mx);

    else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
        // deplace le point de rotation
        camera.translation((float) mx / (float) window_width(), (float) my / (float) window_height());

    /*  config pipeline :
        vertex array object
        program
        uniforms
        texture
     */
    // recupere le point de vue et la projection de la camera
    Transform model= Identity();
    Transform view= camera.view();
    Transform projection= camera.projection(window_width(), window_height(), 45);

    // compose les matrices pour passer du repere local de l'objet au repere projectif
    Transform mvp= projection * view * model;

    // dessine l'objet avec le reflet de la cubemap
    {
        glBindVertexArray(vao);
        glUseProgram(program);

        program_uniform(program, "mvpMatrix", mvp);
        program_uniform(program, "modelMatrix", model);
        program_uniform(program, "camera_position", Inverse(view)(Point(0, 0, 0)));

        // texture
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap);

        // sampler2D declare par le fragment shader
        GLint location= glGetUniformLocation(program, "texture0");
        glUniform1i(location, 0);
        // ou program_uniform(program, "texture0", texture);

        glDrawArrays(GL_TRIANGLES, 0, vertex_count);
    }
    
    // dessine la cubemap sur les autres pixels...
    {
        glBindVertexArray(vao_null);
        glUseProgram(program_cubemap);
        
        // texture
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture_cubemap);
        
        // sampler2D declare par le fragment shader
        GLint location= glGetUniformLocation(program_cubemap, "texture0");
        glUniform1i(location, 0);
        // ou program_uniform(program, "texture0", texture);
        
        program_uniform(program_cubemap, "vpInvMatrix", Inverse(projection * view));
        program_uniform(program_cubemap, "camera_position", Inverse(view)(Point(0, 0, 0)));
        
        // dessine un triangle qui couvre tous les pixels de l'image
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    
    // nettoyage
    glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
    glUseProgram(0);
    glBindVertexArray(0);

    return 1;
}


int main( int argc, char **argv )
{
    // etape 1 : creer la fenetre
    Window window= create_window(1024, 640);
    if(window == NULL)
        return 1;       // erreur lors de la creation de la fenetre ou de l'init de sdl2

    // etape 2 : creer un contexte opengl pour pouvoir dessiner
    Context context= create_context(window);
    if(context == NULL)
        return 1;       // erreur lors de la creation du contexte opengl

    // verifier que l'extension est disponible
    if(GLEW_ARB_shader_viewport_layer_array)
        printf("ARB_shader_viewport_layer_array supported\n");
    
    else
    {
        printf("[error] ARB_shader_viewport_layer_array NOT supported...\n");
        return 1;
    }
    
    // etape 3 : creation des objets
    if(init() < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }

    // etape 4 : affichage de l'application, tant que la fenetre n'est pas fermee. ou que draw() ne renvoie pas 0
    run(window, draw);

    // etape 5 : nettoyage
    quit();
    release_context(context);
    release_window(window);
    return 0;
}
