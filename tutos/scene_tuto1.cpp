
#include <math.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include "window.h"

#include "mesh.h"
#include "wavefront.h"
#include "texture.h"

#include "orbiter.h"

#include "draw.h"


Orbiter camera;

struct Object
{
    Object *parent;
    Mesh *mesh;
    Transform model;
    GLuint texture;
};

Object *create_object( Mesh *mesh, const Transform model, Object *parent= NULL )
{
    Object *object= new Object;
    object->mesh= mesh;
    object->model= model;
    object->parent= parent;
    object->texture= 0;
    return object;
}

Transform model_transform( Object *object )
{
    if(object->parent == NULL)
        return object->model;
    
    return object->model * model_transform(object->parent);
}

Transform model_transform( Object *object, const Transform& local )
{
    if(object->parent == NULL)
        return object->model;
    
    return local * object->model * model_transform(object->parent);
}

std::vector<Object *> scene;


void mesh_normalize( Mesh& mesh )
{
    // calcule le centre de gravite
    Vector g= make_vector(mesh.positions[0]);
    for(unsigned int i= 1; i < (unsigned int) mesh.positions.size(); i++)
        g= g + make_vector(mesh.positions[i]);
    g= g / (float) mesh.positions.size();
    
    // calcule la distance max 
    float dmax= length(make_vector(mesh.positions[0]) - g);
    for(unsigned int i= 1; i < (unsigned int) mesh.positions.size(); i++)
        dmax= std::max(dmax, length(make_vector(mesh.positions[i]) - g));
    
    // normalise les distances au centre de gravite
    for(unsigned int i= 0; i < (unsigned int) mesh.positions.size(); i++)
    {
        Vector d= make_vector(mesh.positions[i]) - g;
        mesh.positions[i]= make_vec3(d / dmax);
    }
}

int init( )
{
    // charge un cube
    Mesh *mesh= new Mesh();
    *mesh= read_mesh("data/cube.obj");
    mesh_normalize(*mesh);
    
    Mesh *mesh2= new Mesh();
    *mesh2= read_mesh("data/bigguy.obj");
    mesh_normalize(*mesh2);
    
    GLuint texture= read_texture(0, "data/debug2x2red.png");

    // cree plusieurs copies du cube placees au hasard sur une grille
    srand(time(0));
    
    int grid_width= 32;
    int grid_height= 32;
    for(unsigned int i= 0; i < 200; i++)
    {
        float x= (float) rand() / (float) RAND_MAX * (grid_width -1) - grid_width / 2;
        float z= (float) rand() / (float) RAND_MAX * (grid_height -1) - grid_height / 2;
        float y= (float) rand() / (float) RAND_MAX - 0.5;
        y*= (float) rand() / (float) RAND_MAX * 10;
        float a= (float) rand() / (float) RAND_MAX; 
        Transform model= make_translation( make_vector(x, y, z) ) * make_rotationY(a * 180);
        
        Object *parent= create_object(mesh, model, NULL);
        parent->texture= texture;
        scene.push_back(parent);
        
        Transform local= make_translation(make_vector(0, 1.3, 0));
        //~ Transform local= make_identity();
        Object *object= create_object(mesh2, local, parent);
        object->texture= 0;
        scene.push_back(object);
    }
    
    camera= make_orbiter_lookat( make_point(0, 0, 0), 32 );
    
    // etat openGL par defaut
    glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
    
    // etape 3 : configuration du pipeline.
    glClearDepth(1.f);                          // profondeur par defaut
    glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
    glEnable(GL_DEPTH_TEST);                    // activer le ztest
    
    return 0;   // ras, pas d'erreur
}

int draw( )
{
    // etape 2 : dessiner l'objet avec opengl
    
    // on commence par effacer la fenetre avant de dessiner quelquechose
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    // on efface aussi le zbuffer
    
    // recupere les mouvements de la souris, utilise directement SDL2
    int mx, my;
    unsigned int mb= SDL_GetRelativeMouseState(&mx, &my);
    
    // deplace la camera
    if(mb & SDL_BUTTON(1))              // le bouton gauche est enfonce
        // tourne autour de l'objet
        orbiter_rotation(camera, mx, my);
    
    else if(mb & SDL_BUTTON(3))         // le bouton droit est enfonce
        // approche / eloigne l'objet
        orbiter_move(camera, mx);
    
    else if(mb & SDL_BUTTON(2))         // le bouton du milieu est enfonce
        // deplace le point de rotation
        orbiter_translation(camera, (float) mx / (float) window_width(), (float) my / (float) window_height()); 
    
    float time= SDL_GetTicks();
    for(unsigned int i= 0; i < scene.size(); i++)
    {
        // mouvement global 
        float x= cosf(time / 100 + (float) i / (float) scene.size() - 0.5 ) * 0.5;
        //~ float y= cosf( time / 100 * M_PI / 2 );
        float y= 0;
        float z= sinf(time / 1000 + (float) i / (float) scene.size() - 0.5) * 0.25;
        //~ Transform model= scene[i]->model * make_translation( make_vector(x, y, z) );
        
        //~ Transform model= scene[i]->model;
        //~ Transform model= model_transform(scene[i]);
        
        // mouvement local
        Transform local= make_translation( make_vector(0, 
            cosf(time / (100 - (float) i / (float) scene.size() * 80)    // change de frequence 
                + (float) i / (float) scene.size() * M_PI / 2) * 0.25, 0) );    // change de phase

        // transformation complete
        Transform model= model_transform(scene[i], local);// * make_translation( make_vector(x, y, z) );
        
        //~ printf("draw %d, texture %u\n", i, scene[i]->texture);
        draw(*scene[i]->mesh, 
            model, 
            orbiter_view_transform(camera), 
            orbiter_projection_transform(camera, window_width(), window_height(), 45),
            scene[i]->texture); 
    }
    
    return 1;    
}

int quit( )
{
    
    return 0;   // ras, pas d'erreur
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
