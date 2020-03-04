
//! \file tuto5GL_cubemap.cpp dessiner une cubemap a l'infini.

#include <stdio.h>

#include "window.h"
#include "vec.h"
#include "mat.h"

#include "program.h"
#include "uniforms.h"
#include "texture.h"

#include "mesh.h"
#include "wavefront.h"

#include "orbiter.h"
#include "image_io.h"


GLuint program;
GLuint program_cubemap;

GLuint texture;

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
    program= read_program("tutos/tuto5GL_cubemap.glsl");
    program_print_errors(program);
    
    // . dessiner la cubemap a l'infini
    program_cubemap= read_program("tutos/cubemap.glsl");
    program_print_errors(program_cubemap);
    
    // etape 2 : charger un mesh, (avec des normales), vao + vertex buffer
    //~ Mesh mesh= read_mesh("data/bigguy.obj");
    Mesh mesh= read_mesh("data/cube.obj");
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
    GLint position= glGetAttribLocation(program, "position");
    if(position >= 0)
    {
        glVertexAttribPointer(position, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(position);
    }
    
    // normal buffer
    if(!mesh.normal_buffer_size())
    {
        printf("[oops]  pas de normales...\n");
        return -1;
    }
    
    glGenBuffers(1, &normal_buffer);
    glBindBuffer(GL_ARRAY_BUFFER, normal_buffer);
    glBufferData(GL_ARRAY_BUFFER, mesh.normal_buffer_size(), mesh.normal_buffer(), GL_STATIC_DRAW);

    GLint normal= glGetAttribLocation(program, "normal");
    if(normal >= 0)
    {
        glVertexAttribPointer(normal, 3, GL_FLOAT, GL_FALSE, 0, 0);
        glEnableVertexAttribArray(normal);
    }
    
    // nettoyage
    mesh.release();
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);

    // etape 3 : texture
#if 0
    ImageData tmp= read_image_data("data/debug2x2red.png");
    //~ ImageData image= flipX(flipY(tmp));
    ImageData image= flipY(tmp);
    
    // solution 1, utiliser une seule texture *carree* et la copier sur les 6 faces de la cubemap
    int size= image.width;
    
    GLenum data_format;
    GLenum data_type= GL_UNSIGNED_BYTE;
    if(image.channels == 3)
        data_format= GL_RGB;
    else // par defaut
        data_format= GL_RGBA;

    // creer la texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    
    // creer les 6 faces 
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.data());
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_X, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.data());
    
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Y, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.data());
        
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Y, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.data());
        
    glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_Z, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.data());
        
    glTexImage2D(GL_TEXTURE_CUBE_MAP_NEGATIVE_Z, 0,
        GL_RGBA, size, size, 0,
        data_format, data_type, image.data());
#endif

#if 0
    // les 6 faces sur une bande
    ImageData image= read_image_data("tutos/cubemap_debug.png");
    
    // les 6 images sont regroupees sur une bande horizontale.
    int size= image.width / 6;
    
    GLenum data_format;
    GLenum data_type= GL_UNSIGNED_BYTE;
    if(image.channels == 3)
        data_format= GL_RGB;
    else // par defaut
        data_format= GL_RGBA;
    
    // creer la texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    // creer les 6 faces
    // chaque face de la cubemap est un bloc [image.width/6 x image.height] dans l'image originale 
    int faces[]= { 0, 1, 2, 3, 4, 5 };
    
    // largeur totale de l'image
    for(int i= 0; i < 6; i++)
    {
        // extrait la face 
        ImageData face= flipX(flipY(copy(image, faces[i]*size, 0, size, size)));
        //~ ImageData face= copy(image, faces[i]*size, 0, size, size);
        
        // transferer les pixels
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0,
            GL_RGBA, size, size, 0,
            data_format, data_type, face.data());
    }
#endif
    
#if 1
// http://paulbourke.net/miscellaneous/cubemaps/

    // les 6 faces sur une croix
    ImageData image= read_image_data("canyon2.jpg");
    //~ ImageData image= read_image_data("html/cubemap_debug_cross.png");
    
    int w= image.width / 4;
    int h= image.height / 3;
    assert(w == h);
    
    GLenum data_format;
    GLenum data_type= GL_UNSIGNED_BYTE;
    if(image.channels == 3)
        data_format= GL_RGB;
    else // par defaut
        data_format= GL_RGBA;
    
    // creer la texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

    // creer les 6 faces
    // chaque face de la cubemap est un rectangle [image.width/4 x image.height/3] dans l'image originale 
    struct { int x, y; } faces[]= {
        {0, 1}, // X+
        {2, 1}, // X-
        {1, 2}, // Y+
        {1, 0}, // Y- 
        {1, 1}, // Z+
        {3, 1}, // Z-
    };
    
    for(int i= 0; i < 6; i++)
    {
        ImageData face= flipX(flipY(copy(image, faces[i].x*w, faces[i].y*h, w, h)));
        //~ ImageData face= copy(image, faces[i].x*w, faces[i].y*h, w, h);
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0,
            GL_RGBA, w, h, 0,
            data_format, data_type, face.data());
    }
#endif

#if 0
    // 6 images
    const char *filenames[]= {
        "data/cubemap/cubemap_opensea/opensea_posx.png",
        "data/cubemap/cubemap_opensea/opensea_negx.png",
        "data/cubemap/cubemap_opensea/opensea_posy.png",
        "data/cubemap/cubemap_opensea/opensea_negy.png",
        "data/cubemap/cubemap_opensea/opensea_posz.png",
        "data/cubemap/cubemap_opensea/opensea_negz.png"
        //~ "data/cubemap/skybox/left.jpg",
        //~ "data/cubemap/skybox/right.jpg",
        //~ "data/cubemap/skybox/top.jpg",
        //~ "data/cubemap/skybox/bottom.jpg",
        //~ "data/cubemap/skybox/back.jpg",
        //~ "data/cubemap/skybox/front.jpg",
    };
    
    // creer la texture
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
    
    for(int i= 0; i < 6; i++)
    {
        ImageData tmp= read_image_data(filenames[i]);
        //~ ImageData image= flipX(flipY(tmp));
        ImageData image= flipY(tmp);    // les faces haut/bas sont retouchees a la main...
        
        GLenum data_format;
        GLenum data_type= GL_UNSIGNED_BYTE;
        if(image.channels == 3)
            data_format= GL_RGB;
        else // par defaut
            data_format= GL_RGBA;
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0,
            GL_RGBA, image.width, image.height, 0,
            data_format, data_type, image.data());
    }
#endif
    
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    // filtrage "correct" sur les bords du cube...
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //~ glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
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
    glDeleteVertexArrays(1, &vao);
    glDeleteVertexArrays(1, &vao_null);
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &normal_buffer);
    glDeleteTextures(1, &texture);
    return 0;
}

int draw( )
{
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

#if 1
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
#endif

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
        program_uniform(program, "mMatrix", model);
        program_uniform(program, "camera_position", Inverse(view)(Point(0, 0, 0)));

        // texture
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);

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
        glBindTexture(GL_TEXTURE_CUBE_MAP, texture);
        
        // sampler2D declare par le fragment shader
        GLint location= glGetUniformLocation(program_cubemap, "texture0");
        glUniform1i(location, 0);
        // ou program_uniform(program, "texture0", texture);
        
        program_uniform(program_cubemap, "vpInvMatrix", Inverse(projection * view));
        program_uniform(program_cubemap, "camera_position", Inverse(view)(Point(0, 0, 0)));
        
        // dessine un triangle qui couvre tous les pixels de l'image
        glDrawArrays(GL_TRIANGLES, 0, 3);
    }
    
    if(key_state('s'))
    {
        clear_key_state('s');
        static int calls= 0;
        screenshot("cubemaps", ++calls);
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
