
//! tutorial : exemple minimaliste opengl 3 core profile


#include <viewer.h>
#include <cstdio>

using namespace std;


int init(Viewer& v)
{
    v.camera= make_orbiter_lookat( make_vec3(0.5, 0.5, 0.5), 1 );

    // etat openGL par defaut
    glClearColor(0.2, 0.2, 0.2, 1);
    glClearDepth(1.f);

    glCullFace(GL_BACK);
    glFrontFace(GL_CCW);
    glEnable(GL_CULL_FACE);

    glDepthFunc(GL_LESS);
    glEnable(GL_DEPTH_TEST);

    // charger une texture
    v.texture= read_texture(0, "data/debug2x2red.png");
    if(v.texture == 0) return -1;


#if 1
    // charge un fichier obj
    v.cube= read_obj("data/bigguy.obj");
    v.cube_vao= make_mesh_vertex_format(v.cube);

    // genere le shader permettant d'afficher le mesh
    v.program = make_mesh_program(v.cube);
    if(v.program == 0) return -1;

#else
#if 0
    // creer les buffers : decrire un cube indexe.
    float positions[][3] = {
        {0, 0, 0}, {1, 0, 0}, {1, 1, 0}, {0, 1, 0},
        {0, 0, 1}, {1, 0, 1}, {1, 1, 1}, {0, 1, 1}
    };

    float texcoords[][2] = {
        {0, 0}, {1, 0}, {1, 1}, {0, 1},
        {0, 0}, {1, 0}, {1, 1}, {0, 1}
    };

    unsigned int indices[] = {
        0, 1, 2,
        0, 2, 3,
        4, 5, 6,
        4, 6, 7
    };

    // creer un vertex array / vertex format
    cube_vao= make_vertex_format();

    // creer le vertex buffer, attribut position == 0
    vertex_buffer= make_vertex_buffer(cube_vao, 0,  3, GL_FLOAT, sizeof(positions), positions);

    // creer le texcoord buffer, attribut texcoord == 1
    texcoord_buffer= make_vertex_buffer(cube_vao, 1,  2, GL_FLOAT, sizeof(texcoords), texcoords);

    // creer l'index buffer
    index_buffer= make_index_buffer(cube_vao, sizeof(indices), indices);

#else
    cube= make_mesh(GL_TRIANGLES);
    // glBegin(GL_TRIANGLES)
    {
        // triangle 1
        mesh_texcoord(cube, make_vec2(0, 0));
        unsigned int a= mesh_push_vertex(cube, make_vec3(0, 0, 0));
        // glTexCoord(0, 0);
        // glVertex(0, 0, 0);
        // comme gl, mesh_push_vertex( ) termine la description des attributs et emet le sommet
        // et un sommet a obligatoirement une position...

        mesh_texcoord(cube, make_vec2(1, 0));
        unsigned int b= mesh_push_vertex(cube, make_vec3(1, 0, 0));

        mesh_texcoord(cube, make_vec2(1, 1));
        unsigned int c= mesh_push_vertex(cube, make_vec3(1, 1, 0));

        //~ mesh_push_triangle(cube, a, b, c);
        mesh_push_triangle_last(cube, -3, -2, -1);

        // triangle 1
        //~ mesh_texcoord(cube, make_vec2(0, 0));
        //~ mesh_push_vertex(cube, make_vec3(0, 0, 0));

        //~ mesh_texcoord(cube, make_vec2(1, 1));
        //~ mesh_push_vertex(cube, make_vec3(1, 1, 0));

        mesh_texcoord(cube, make_vec2(0, 1));
        unsigned int d= mesh_push_vertex(cube, make_vec3(0, 1, 0));

        //~ mesh_push_triangle(cube, a, c, d);
        mesh_push_triangle_last(cube, -4, -2, -1);
    }
    // glEnd();

    // construit les buffers necessaires et configure un vao, position==0, texcoord==1, normal==2, color==3
    cube_vao= make_mesh_vertex_format(cube);

    // genere le shader permettant d'afficher le mesh
    program= make_mesh_program(cube);
    if(program == 0)
        return -1;
#endif
#endif


    // nettoyage
    glUseProgram(0);
    glBindTexture(GL_TEXTURE_2D, 0);
    glBindVertexArray(0);
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);


    return 0;
}

int quit(Viewer& v)
{
    // detruit les objets openGL
#if 0
    glDeleteBuffers(1, &vertex_buffer);
    glDeleteBuffers(1, &texcoord_buffer);
    glDeleteBuffers(1, &index_buffer);
    glDeleteVertexArrays(1, &cube_vao);
#else
    release_vertex_format(v.cube_vao);
#endif
    glDeleteTextures(1, &v.texture);
    glDeleteProgram(v.program);
    return 0;
}



int draw(Viewer& v)
{
    // effacer l'image
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    /* configuration minimale du pipeline :
        glBindVertexArray() // association du contenu des buffers aux attributs declares dans le vertex shader
        glUseProgram()  // indique quel shader program utiliser pour dessiner
        { ... } // parametrer les uniforms des shaders

        glDraw()    // execution du pipeline
     */
    glBindVertexArray(v.cube_vao);
    glUseProgram(v.program);

    // affecter les uniforms
    int x, y;
    unsigned int button= SDL_GetRelativeMouseState(&x, &y);

    if(button & SDL_BUTTON(1))
        orbiter_rotation(v.camera, x, y);      // orbit
    else if(button & SDL_BUTTON(2))
        orbiter_translation(v.camera, (float) x / (float) window_width(), (float) y / (float) window_height()); // pan
    else if(button & SDL_BUTTON(3))
        orbiter_move(v.camera, x);           // dolly

    // initialiser les transformations
    mat4 model= make_identity();

    mat4 view= orbiter_view_matrix(v.camera);
    mat4 projection= orbiter_projection_matrix(v.camera, window_width(), window_height(), 45);

    mat4 mv= view * model;
    mat4 normal= make_normal_matrix(mv);
    mat4 mvp= projection * view * model;

    program_set_mat4(v.program, "mvpMatrix", mvp);
    program_set_mat4(v.program, "mvMatrix", mv);
    program_set_mat4(v.program, "normalMatrix", normal);

    // utiliser une texture, elle ne sera visible que le mesh a des texcoords...
    program_use_texture(v.program, "diffuse_color", 0, v.texture);

    // draw
    glDrawArrays(v.cube.primitives, 0, v.cube.count);
    //~ glDrawElements(cube.primitives, cube.count, GL_UNSIGNED_INT, 0);

    // cacher tout l'affichage dans mesh_draw( cube, model, view, projection ) ?? et la texture ??
    // cacher tout l'affichage dans mesh_draw_texture( cube, model, view, projection, /* unit */ 0, texture ) ??

    return 1;
}




int main( int argc, char **argv )
{
    window w= create_window(1024, 768);
    if(w == NULL) return 1;

    context c= create_context(w);        // cree un contexte opengl 3.3, par defaut
    if(c == NULL) return 1;

    Viewer v;
    // creation des objets opengl
    if(init(v) < 0)
    {
        printf("[error] init failed.\n");
        return 1;
    }

    // affichage de l'application
    run(w);

    // nettoyage
    quit(v);
    release_context(c);
    release_window(w);
    return 0;
}
