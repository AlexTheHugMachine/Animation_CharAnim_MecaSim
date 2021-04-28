
//! \file tuto_dynamic_cubemap.cpp rendu dans une cubemap

#include <memory>

#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"
#include "program.h"
#include "uniforms.h"
#include "draw.h"

#include "app_camera.h"        // classe Application a deriver


//! charge une image, decoupe les 6 faces et renvoie une texture cubemap.
GLuint read_cubemap( const int unit, const char *filename,  const GLenum texel_type = GL_RGBA )
{
    // les 6 faces sur une croix
    ImageData image= read_image_data(filename);
    if(image.pixels.empty()) 
        return 0;
    
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
    GLuint texture= 0;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
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
        
        glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0,
            texel_type, w, h, 0,
            data_format, data_type, face.data());
    }
    
    // parametres de filtrage
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    
    glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);    
    
    glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
    
    // filtrage "correct" sur les bords du cube...
    glEnable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    //~ glDisable(GL_TEXTURE_CUBE_MAP_SEAMLESS);
    
    printf("  cubemap faces %dx%d\n", w, h);
    
    return texture;
}


// utilitaire. creation d'une grille / repere.
Mesh make_grid( const int n= 10 )
{
    Mesh grid= Mesh(GL_LINES);
    
    // grille
    grid.color(White());
    for(int x= 0; x < n; x++)
    {
        float px= float(x) - float(n)/2 + .5f;
        grid.vertex(Point(px, 0, - float(n)/2 + .5f)); 
        grid.vertex(Point(px, 0, float(n)/2 - .5f));
    }

    for(int z= 0; z < n; z++)
    {
        float pz= float(z) - float(n)/2 + .5f;
        grid.vertex(Point(- float(n)/2 + .5f, 0, pz)); 
        grid.vertex(Point(float(n)/2 - .5f, 0, pz)); 
    }
    
    // axes XYZ
    grid.color(Red());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(1, .1, 0));
    
    grid.color(Green());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(0, 1, 0));
    
    grid.color(Blue());
    grid.vertex(Point(0, .1, 0));
    grid.vertex(Point(0, .1, 1));
    
    return grid;
}


class TP : public AppCamera
{
public:
    // constructeur : donner les dimensions de l'image, et eventuellement la version d'openGL.
    TP( ) : AppCamera(1024, 640) {}
    
    void init_dynamic_cubemap( const int w, const int h )
    {
        // 6 faces couleur
        glGenTextures(1, &m_texture_cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_cubemap);
        for(int i= 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0,
                GL_RGBA, w, h, 0,
                GL_RGBA, GL_UNSIGNED_BYTE, nullptr);
        }
        
        // parametres de filtrage
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);    
        glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);    
        //+ mipmaps
        glGenerateMipmap(GL_TEXTURE_CUBE_MAP);    
        
        // 6 faces profondeur
        glGenTextures(1, &m_depth_cubemap);
        glBindTexture(GL_TEXTURE_CUBE_MAP, m_depth_cubemap);
        for(int i= 0; i < 6; i++)
        {
            glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X +i, 0,
                GL_DEPTH_COMPONENT, w, h, 0,
                GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
        }
        
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);        

        // framebuffer, attache les 6 faces couleur + profondeur
        glGenFramebuffers(1, &m_framebuffer);
        glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, m_texture_cubemap, 0);
        glFramebufferTexture(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, m_depth_cubemap, 0);
        
        glBindFramebuffer(GL_FRAMEBUFFER, 0);
    }
    
    // creation des objets de l'application
    int init( )
    {
        m_object= read_mesh("data/bigguy.obj");
        m_cube= read_mesh("data/cube.obj");
        m_ground= make_grid(20);
        
        m_program_render= read_program("tutos/render_cubemap.glsl");
        program_print_errors(m_program_render);
        m_program_draw= read_program("tutos/draw_cubemap.glsl");
        program_print_errors(m_program_draw);
        m_program= read_program("tutos/cubemap.glsl");
        program_print_errors(m_program);
        
        // init camera
        Point pmin, pmax;
        m_object.bounds(pmin, pmax);
        camera().lookat(pmin*2, pmax*2);
        
        // cubemap dynamique / framebuffer
        init_dynamic_cubemap(1024, 1024);
        
        // etat openGL par defaut
        glGenVertexArrays(1, &m_vao);               // un vao sans attributs, pour dessiner la cubemap
        
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LEQUAL);                     // !! ztest, conserver l'intersection la plus proche de la camera !!
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        glLineWidth(1.5);
        
        return 0;   // ras, pas d'erreur
    }
   
    
    struct Object
    {
        Transform model;
        Color color;
    };
    
    void scene( std::vector<Object>& objects )
    {
        objects.resize(100);
        
        // place quelques cubes dans la scene
        float offset= global_time() / 200;
        for(int i= 0; i < 100; i++)
        {
            float x= (i % 10 - 4.5) * 4;
            float z= (i / 10 - 4.5) * 4;
            float y= std::cos(x * z + offset);
            
            objects[i].color= Color(White() * std::abs(y));
            objects[i].model= Translation(x, y, z) * RotationY(y*180);
        }
    }
    
    // dessiner une nouvelle image
    int render( )
    {
        Transform view= camera().view();
        Transform projection= camera().projection();
        Transform viewport= camera().viewport();
        Transform mvp= projection * view;
        
        // "animer" les cubes du "decor"... 
        std::vector<Object> objects;
        scene(objects);
        
        // partie 1 : dessiner le decor dans la cubemap dynamique
        {
            // ... dans le framebuffer
            glBindFramebuffer(GL_FRAMEBUFFER, m_framebuffer);
            glViewport(0, 0, 1024, 1024);       // dimension des faces de la cubemap
            
            // efface les 6 faces couleur + profondeur attachees au framebuffer
            float black[4]= { 0.6, 0.6, 0.6, 1 };
            glClearBufferfv(GL_COLOR, 0, black);
            
            float one= 1;
            glClearBufferfv(GL_DEPTH, 0, &one);
        
            // prepare les 6 matrices view, une par face de la cubemap
            // !! attention a la direction 'up' de lookat... rappel : orientation des textures des cubemaps !!
            Transform faces[6];
            faces[0]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(1, 0, 0),  /* up */ Vector(0, -1, 0));   // +X
            faces[1]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(-1, 0, 0), /* up */ Vector(0, -1, 0));   // -X
            
            faces[2]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(0, 1, 0),  /* up */ Vector(0, 0, 1));    // +Y
            faces[3]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(0, -1, 0), /* up */ Vector(0, 0, -1));   // -Y
            
            faces[4]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(0, 0, 1),  /* up */ Vector(0, -1, 0));   // +Z
            faces[5]= Lookat(/* from */ Point(0, 0, 0), /* to */ Point(0, 0, -1), /* up */ Vector(0, -1, 0));   // -Z
            
            // dessine chaque objet du decor
            GLuint vao= m_cube.create_buffers( /* texcoord */ false, /* normal */ true, /* color */ false, /* material */ false);
            glBindVertexArray(vao);
            
            glUseProgram(m_program_render);
            int location_mvp= glGetUniformLocation(m_program_render, "mvpMatrix");
            int location_model= glGetUniformLocation(m_program_render, "modelMatrix");
            
            for(int i= 0; i < int(objects.size()); i++)
            {
                Transform model= objects[i].model;
                Transform projection= Perspective(45, 1, 0.01, 100);
                
                // recalcule les 6 transformations, model est different pour chaque objet 
                Transform mvp[6];
                for(int i= 0; i < 6; i++)
                    mvp[i]= projection * faces[i] * model;
                
                // go !!
                glUniformMatrix4fv(location_mvp, 6, GL_TRUE, mvp[0].buffer());
                glUniformMatrix4fv(location_model, 1, GL_TRUE, model.buffer());
                glDrawArraysInstanced(GL_TRIANGLES, 0, m_object.vertex_count(), 6);
            }
            
            // mise a jour des mipmaps des faces de la cubemap
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_cubemap);
            glGenerateMipmap(GL_TEXTURE_CUBE_MAP);
        }
        
        // partie 2 : dessiner avec la cubemap dynamique...
        {
            // ... dans le framebuffer de la fenetre de l'application
            glBindFramebuffer(GL_FRAMEBUFFER, 0);
            glViewport(0, 0, window_width(), window_height());
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            
            Transform viewInv= Inverse(view);
            Point camera_position= viewInv(Point(0, 0, 0));  // coordonnees de la camera, dans le repere camera... c'est l'origine
            
            // affiche l'objet principal, utilise la cubemap pour calculer les reflets
            Transform model= Identity();
            
            glUseProgram(m_program);
            program_uniform(m_program, "mvpMatrix", mvp * model);
            program_uniform(m_program, "modelMatrix", model);
            program_uniform(m_program, "camera_position", camera_position);
            
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_cubemap);
            program_uniform(m_program, "texture0", int(0));
            
            // dessine l'objet, les attributs position et normale sont necessaires a l'execution du shader.
            m_object.draw(m_program, /* position */ true, /* texcoord */ false, /* normal */ true, /* color */ false, /* material */ false );
            
            // et le reste
            draw(m_ground, Identity(), view, projection);
            for(int i= 0; i < int(objects.size()); i++)
                draw(m_cube, objects[i].model, view, projection);
            
            // etape 2 : affiche la cube map 
            // inverse de la transformation repere monde vers repere image
            Transform inv= Inverse(viewport * projection * view);
            
            glUseProgram(m_program_draw);
            glBindVertexArray(m_vao);
            program_uniform(m_program_draw, "invMatrix", inv);
            program_uniform(m_program_draw, "camera_position", camera_position);
            
            glBindTexture(GL_TEXTURE_CUBE_MAP, m_texture_cubemap);
            program_uniform(m_program_draw, "texture0", int(0));
            
            glDrawArrays(GL_TRIANGLES, 0, 3);
        }
        
        // nettoyage
        glBindTexture(GL_TEXTURE_CUBE_MAP, 0);
        glUseProgram(0);
        glBindVertexArray(0);
        
        if(key_state('r'))
        {
            clear_key_state('r');
            
            reload_program(m_program_render, "tutos/render_cubemap.glsl");
            program_print_errors(m_program_render);
            reload_program(m_program_draw, "tutos/draw_cubemap.glsl");
            program_print_errors(m_program_draw);
            reload_program(m_program, "tutos/cubemap.glsl");
            program_print_errors(m_program);
        }
        
        if(key_state('s'))
        {
            clear_key_state('s');
            static int calls= 0;
            screenshot("cubemap_brdf", ++calls);
            printf("screenshot %d\n", calls);
        }
        
        return 1;
    }
    
    // destruction des objets de l'application
    int quit( )
    {
        m_object.release();
        m_cube.release();
        m_ground.release();
        
        release_program(m_program_render);
        release_program(m_program_draw);
        release_program(m_program);
        
        glDeleteVertexArrays(1, &m_vao);
        
        glDeleteTextures(1, &m_texture_cubemap);
        glDeleteTextures(1, &m_depth_cubemap);
        glDeleteFramebuffers(1, &m_framebuffer);
        return 0;
    }
    
protected:
    Mesh m_object;
    Mesh m_cube;
    Mesh m_ground;
    
    GLuint m_program_render;
    GLuint m_program_draw;
    GLuint m_program;
    GLuint m_vao;
    GLuint m_texture_cubemap;
    GLuint m_depth_cubemap;
    GLuint m_framebuffer;
};


int main( int argc, char **argv )
{
    TP tp;
    tp.run();
    
    return 0;
}
