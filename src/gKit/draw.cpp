
#include "draw.h"
#include "window.h"


void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection, const GLuint texture )
{
    m.draw(model, view, projection, /* use light */ false, Point(), White(), /* use texture */ true, texture, /* use alpha test */ false, 0);
}

void draw( Mesh& m, const Transform& model, const Transform& view, const Transform& projection )
{
    m.draw(model, view, projection, /* use light */ false, Point(), White(), /* use texture */ false, 0, /* use alpha test */ false, 0);
}

void draw( Mesh& m, const Transform& model, const Orbiter& camera, const GLuint texture )
{
    // recupere les transformations
    Transform view= camera.view();
    Transform projection= camera.projection((float) window_width(), (float) window_height(), 45);
    
    // affiche l'objet
    draw(m, model, view, projection, texture);
}

void draw( Mesh& m, const Orbiter& camera, const GLuint texture )
{
    draw(m, Identity(), camera, texture);
}

void draw( Mesh& m, const Orbiter& camera )
{
    draw(m, Identity(), camera, 0);
}

void draw( Mesh& m, const Transform& model, const Orbiter& camera )
{
    draw(m, model, camera, 0);
}

void draw( Mesh& m, const GLuint program, const bool use_position, const bool use_texcoord, const bool use_normal )
{
    if(program == 0)
        return;
    
#ifndef GK_RELEASE
    // verifie que les attributs necessaires a l'execution du shader sont presents dans le mesh...

    // etape 1 : recuperer le nombre d'attributs
    GLint n= 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTES, &n);

    // recuperer la longueur max occuppee par un nom d'attribut
    GLint length_max= 0;
    glGetProgramiv(program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &length_max);
    char *name= new char [length_max];
    
    // etape 2 : recuperer les infos de chaque attribut
    bool has_position= false;
    bool has_texcoord= false;
    bool has_normal= false;
    for(int index= 0; index < n; index++)
    {
        GLint glsl_size;
        GLenum glsl_type;
        glGetActiveAttrib(program, index, length_max, NULL, &glsl_size, &glsl_type, name);
        // et son identifiant
        GLint location= glGetAttribLocation(program, name);
        
        if(location == 0)       // attribut position necessaire a l'execution du shader
        {
            if(!use_position || !m.vertex_buffer_size())
                printf("[oops]  no position '%s' attribute in mesh... can't draw !!\n", name);
            if(glsl_size != 1 || glsl_type != GL_FLOAT_VEC3)
                printf("[oops]  attribute '%s' is not declared as a vec3... can't draw !!\n", name);
        }
        else if(location == 1)  // attribut texcoord necessaire 
        {
            if(!use_texcoord || !m.texcoord_buffer_size())
                printf("[oops]  no texcoord '%s' attribute in mesh... can't draw !!\n", name);
            if(glsl_size != 1 || glsl_type != GL_FLOAT_VEC2)
                printf("[oops]  attribute '%s' is not declared as a vec2... can't draw !!\n", name);
        }
        else if(location == 2)  // attribut normal necessaire
        {
            if(!use_normal || !m.normal_buffer_size())
                printf("[oops]  no normal '%s' attribute in mesh... can't draw !!\n", name);
            if(glsl_size != 1 || glsl_type != GL_FLOAT_VEC3)
                printf("[oops]  attribute '%s' is not declared as a vec3... can't draw !!\n", name);
        }
    }
    delete [] name;
#endif
    
    m.create_buffers(use_position, use_texcoord, use_normal);
    m.draw(program);
}

void DrawParam::draw( Mesh& mesh ) const
{
    mesh.draw(m_model, m_view, m_projection, m_use_light, m_light, m_light_color, m_use_texture, m_texture, m_use_alpha_test, m_alpha_min);
}

void draw( Mesh& m, const DrawParam& param )
{
    param.draw(m);
}
