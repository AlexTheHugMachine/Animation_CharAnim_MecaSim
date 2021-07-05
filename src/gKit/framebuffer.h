
//! \file framebuffer.h

#ifndef _FRAMEBUFFER_H
#define _FRAMEBUFFER_H

#include "texture.h"


/*! stockage sorties d'un fragment shader. 
    exemple complet d'utilisation dans tuto_shadows.cpp 
    
\code
Framebuffer framebuffer;
    
init( ) : 
    framebuffer.create(1024, 1024);
    framebuffer.clear_color( White() );
    framebuffer.clear_depth( 1 );
    
render( ) :
    // etape 1 : dessiner des objets dans le framebuffer, avec un shader qui stocke 1 ou plusieurs valeurs par pixel
    glUseProgram( program1 );
    framebuffer.bind( program1, store_color, store_depth, store_position, store_texcoord, store_normal, store_material );
    
    // draw
    { ... }
    
    // etape 2 : retour au framebuffer de la fenetre
    framebuffer.unbind( window_width(), window_height() );
    
    // etape 3 : utiliser les valeurs stockees dans le framebuffer, comme des textures... avec un autre shader
    glUseProgram( program2 );
    
    framebuffer.use_color_texture( program2, "sampler2D declare dans le shader", unit );
    
    // draw
    { ... }
\endcode
*/

struct Framebuffer
{
    Framebuffer( ) = default;
    
    //! creation du framebuffer
    GLuint create( const int width, const int height );
    //! destruction.
    void release( );
    
    //! renvoie la largeur du framebuffer.
    int width( ) const;
    //! renvoie la hauteur du framebuffer.
    int height( ) const;
    
///@{
    //! @name configuration des valeurs par defaut.
    void clear_color( const Color& value ); 
    void clear_depth( const float value ); 
    void clear_position( const Point& value ); 
    void clear_texcoord( const vec2& value ); 
    void clear_normal( const Vector& value ); 
    void clear_material( const unsigned value ); 
///@}

    //! selection du framebuffer, stocker les sorties du fragment shader.
    void bind( const GLuint program, const bool store_color, const bool store_depth, const bool store_position, const bool store_texcoord, const bool store_normal, const bool store_material );
    //! selection du framebuffer par defaut associe a la fenetre.
    void unbind( const int width, const int height );
    
///@{
    //! @name utilisation des resultats. meme utilisation que program_use_texture().
    void use_color_texture( const GLuint program, const char *uniform, const int unit, const GLuint sampler= 0 ); 
    void use_depth_texture( const GLuint program, const char *uniform, const int unit, const GLuint sampler= 0 ); 
    void use_position_texture( const GLuint program, const char *uniform, const int unit, const GLuint sampler= 0 ); 
    void use_texcoord_texture( const GLuint program, const char *uniform, const int unit, const GLuint sampler= 0 ); 
    void use_normal_texture( const GLuint program, const char *uniform, const int unit, const GLuint sampler= 0 ); 
    void use_material_texture( const GLuint program, const char *uniform, const int unit, const GLuint sampler= 0 ); 
///@}
    
///@{
    //! @name visualisation des textures.
    void blit_depth( const int dstX0, const int dstY0, const int dstX1, const int dstY1 );
    void blit_color( const int dstX0, const int dstY0, const int dstX1, const int dstY1 );
    void blit_position( const int dstX0, const int dstY0, const int dstX1, const int dstY1 );
    void blit_texcoord( const int dstX0, const int dstY0, const int dstX1, const int dstY1 );
    void blit_normal( const int dstX0, const int dstY0, const int dstX1, const int dstY1 );
    //~ void blit_material( const int dstX0, const int dstY0, const int dstX1, const int dstY1 );
///@}
    
protected:
    bool status( );
    
    std::vector<GLenum> m_draw_buffers;
    std::vector<GLuint> m_color_textures;
    
    std::vector<unsigned[4]> m_clear_colors;
    std::vector<int> m_color_units;
    float m_clear_depth;
    int m_depth_unit;
    
    GLuint m_depth_texture;
    GLuint m_fbo;
    int m_width;
    int m_height;
};

// utilitaires.
//! creation de textures pour stocker des donnees (autres qu'une couleur).
GLuint make_depth_texture( const int unit, const int width, const int height, const GLenum texel_type= GL_DEPTH_COMPONENT );
//! creation de textures pour stocker des donnees (autres qu'une couleur).
GLuint make_uint_texture( const int unit, const int width, const int height, const GLenum texel_type= GL_R32UI );
//! creation de textures pour stocker des donnees (autres qu'une couleur).
GLuint make_float_texture( const int unit, const int width, const int height, const GLenum texel_type= GL_R32F );
//! creation de textures pour stocker des donnees (autres qu'une couleur).
GLuint make_vec2_texture( const int unit, const int width, const int height, const GLenum texel_type= GL_RG32F );
//! creation de textures pour stocker des donnees (autres qu'une couleur).
GLuint make_vec3_texture( const int unit, const int width, const int height, const GLenum texel_type= GL_RGB32F );
//! creation de textures pour stocker des donnees (autres qu'une couleur).
GLuint make_vec4_texture( const int unit, const int width, const int height, const GLenum texel_type= GL_RGBA32F );

#endif
