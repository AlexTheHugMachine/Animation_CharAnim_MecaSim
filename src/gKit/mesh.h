
#ifndef _MESH_H
#define _MESH_H

#include <vector>

#include "glcore.h"

#include "vec.h"
#include "mat.h"
#include "color.h"


//! \addtogroup objet3D utilitaires pour manipuler des objets 3d
///@{

//! \file 
//! representation d'un objet

//! representation d'un objet / maillage.
struct Mesh
{
    std::vector<vec3> positions;
    std::vector<vec2> texcoords;
    std::vector<vec3> normals;
    std::vector<vec3> colors;
    
    std::vector<unsigned int> indices;
    
    Color color;
    
    GLenum primitives;
    GLuint vao;
    GLuint program;
};

//! construit un objet / maillage vide compose de primitives de type primitives. a detruire avec release_mesh( ).
//! \param primitives : GL_TRIANGLES, GL_TRIANGLE_STRIP, GL_LINES, etc.
Mesh create_mesh( const GLenum primitives );
//! detruit un objet.
void release_mesh( Mesh& m );

//! definit les coordonnees de texture du prochain sommet.
void vertex_texcoord( Mesh& m, const float u, const float v );
//! definit les coordonnees de texture du prochain sommet.
void vertex_texcoord( Mesh& m, const vec2& uv );
//! definit la normale du prochain sommet.
void vertex_normal( Mesh& m, const vec3& normal );
//! definit la normale du prochain sommet.
void vertex_normal( Mesh& m, const Vector& normal );
//! definit la couleur du prochain sommet.
void vertex_color( Mesh& m, const vec3& color );
//! definit la couleur du prochain sommet.
void vertex_color( Mesh& m, const Color& color );

//! insere un sommet et ses attributs dans l'objet. renvoie l'indice du sommet.
unsigned int push_vertex( Mesh& m, const vec3& position );
//! insere un sommet et ses attirbuts dans l'objet. renvoie l'indice du sommet.
unsigned int push_vertex( Mesh& m, const Point& position );
//! insere un sommet et ses attirbuts dans l'objet. renvoie l'indice du sommet.
unsigned int push_vertex( Mesh& m, const float x, const float y, const float z );

/*! insere un sommet et ses attributs dans l'objet. utilitaire, equivalent a:
\code
vertex_texcoord(m, u, v);
vertex_normal(m, normal);
push_vertex(m, position);
\endcode
*/
unsigned int push_vertex( Mesh& m, const vec3& position, const float u, const float v, const vec3& normal );
//! insere un sommet et ses attributs dans l'objet. utilitaire.
unsigned int push_vertex( Mesh& m, const Point& position, const float u, const float v, const Vector& normal );

/*! insere un sommet et ses attributs dans l'objet. utilitaire, equivalent a:
\code
vertex_texcoord(m, u, v);
push_vertex(m, position);
\endcode
*/
unsigned int push_vertex( Mesh& m, const vec3& position, const float u, const float v );
//! insere un sommet et ses attributs dans l'objet. utilitaire.
unsigned int push_vertex( Mesh& m, const Point& position, const float u, const float v );

/*! insere un sommet et ses attributs dans l'objet. utilitaire, equivalent a:
\code
vertex_normal(m, normal);
push_vertex(m, position);
\endcode
*/
unsigned int push_vertex( Mesh& m, const vec3& position, const vec3& normal );
//! insere un sommet et ses attributs dans l'objet. utilitaire.
unsigned int push_vertex( Mesh& m, const Point& position, const Vector& normal );

/*! insere un triangle dans l'objet, a, b, c sont les indices des sommets deja inseres dans l'objet. ne fonctionne pas avec les strips et les fans.
\code
Mesh m= create_mesh(GL_TRIANGLES);
unsigned int a= push_vertex(m, Point(ax, ay, az));
unsigned int b= push_vertex(m, Point(bx, by, bz));
unsigned int c= push_vertex(m, Point(cx, cy, cz));
push_triangle(m, a, b, c);
\endcode
*/
void push_triangle( Mesh& m, const unsigned int a, const unsigned int b, const unsigned int c );

/*! insere un triangle dans l'objet, a, b, c sont les indices des sommets deja inseres dans l'objet, en comptant en partant du dernier. ne fonctionne pas avec les strips et les fans.
\code
Mesh m= create_mesh(GL_TRIANGLES);
push_vertex(m, Point(ax, ay, az));
push_vertex(m, Point(bx, by, bz));
push_vertex(m, Point(cx, cy, cz));
push_triangle_last(m, -3, -2, -1);
\endcode
*/
void push_triangle_last( Mesh& m, const int a, const int b, const int c );

//! demarrage un nouveau strip. a utiliser avec un objet composes de GL_TRIANGLE_STRIP, doit aussi fonctionner avec GL_TRIANGLE_FAN, GL_LINE_STRIP, GL_LINE_LOOP, etc.
void restart_strip( Mesh& m );

//! renvoie min et max les coordonnees des extremites des positions des sommets de l'objet.
void mesh_bounds( const Mesh& m, Point& pmin, Point& pmax );

//! construit les buffers et le vertex array object necessaires pour dessiner l'objet avec openGL. utilitaire. detruit par release_mesh( ).\n
//! exemple, cf make_mesh_program( )
GLuint make_mesh_vertex_format( Mesh& m, const bool use_texcoord= true, const bool use_normal= true, const bool use_color= true );

//! construit le shader program configure pour dessiner l'objet avec openGL. utilitaire. detruit par release_mesh( ).\n
/*! exemple :
\code
Mesh mesh= { ... };
Orbiter camera= { ... };

mesh.vao= make_mesh_vertex_format(mesh, false, false, false);
mesh.program= make_mesh_program(mesh, false, false, false);

// dessine uniquement la geometrie... sans couleurs, sans normales, sans textures
draw(mesh, camera);
\endcode
 */

//! \param use_texcoord force l'utilisation des coordonnees de texture
//! \param use_normal force l'utilisation des normales
//! \param use_color force l'utilisation des couleurs 
GLuint make_mesh_program( Mesh& m, const bool use_texcoord= true, const bool use_normal= true, const bool use_color= true );


//! dessine l'objet avec les transformations model, vue et projection.
void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection );
//! dessine l'objet avec les transformations model, vue et projection.\n
//! applique une texture a la surface de l'objet. ne fonctionne que si les coordonnees de textures sont fournies avec tous les sommets de l'objet. 
void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection, GLuint texture );

///@}
#endif
