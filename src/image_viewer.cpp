
#include <cfloat>

#include "app.h"
#include "widgets.h"

#include "image.h"
#include "image_io.h"
#include "image_hdr.h"

#include "program.h"
#include "uniforms.h"
#include "texture.h"


struct ImageViewer : public App
{
    ImageViewer( const char *filename ) : App(1024, 640)
    {
        m_image= Image::error();
        if(is_hdr_image(filename))
            m_image= read_image_hdr(filename);
        else
            m_image= read_image(filename);
        
        // change le titre de la fenetre
        SDL_SetWindowTitle(m_window, filename);
    }
    
    int init( )
    {
        if(m_image == Image::error())
            return -1;
        
        // redminsionne la fenetre
        SDL_SetWindowSize(m_window, m_image.width(), m_image.height());
        
        m_texture= make_texture(0, m_image);
        
        glGenVertexArrays(1, &m_vao);
        glBindVertexArray(m_vao);
        
        m_program= read_program("data/shaders/tonemap.glsl");
        program_print_errors(m_program);
        
        // 
        m_red= 1;
        m_green= 1;
        m_blue= 1;
        m_alpha= 1;
        m_gray= 0;
        m_compression= 22;
        m_saturation= 1;
        
        //
        int bins[100] = {};
        float ymin= FLT_MAX;
        float ymax= 0.f;
        for(int y= 0; y < m_image.height(); y++)
        for(int x= 0; x < m_image.width(); x++)
        {
            Color color= m_image(x, y);
            float y= color.r + color.g + color.b;
            if(y < ymin) ymin= y;
            if(y > ymax) ymax= y;
        }
        
        for(int y= 0; y < m_image.height(); y++)
        for(int x= 0; x < m_image.width(); x++)
        {
            Color color= m_image(x, y);
            float y= color.r + color.g + color.b;
            int b= (y - ymin) * 100.f / (ymax - ymin);
            if(b >= 99) b= 99;
            if(b < 0) b= 0;
            bins[b]++;
        }

        printf("[%f..%f]\n", ymin, ymax);
        for(int i= 0; i < 100; i++)
            //~ printf("%02d%% ", (int) ((float) bins[i] * 100.f / (m_image.width() * m_image.height())));
            printf("%f ", ((float) bins[i] * 100.f / (m_image.width() * m_image.height())));
        printf("\n");
        
        float qbins= 0;
        for(int i= 0; i < 100; i++)
        {
            if(qbins > .75f)
            {
                m_saturation= ymin + (float) i / 100.f * (ymax - ymin);
                break;
            }
            
            qbins= qbins + (float) bins[i] / (m_image.width() * m_image.height());
        }
        
        //
        m_widgets= create_widgets();
        
        // etat openGL par defaut
        glUseProgram(0);
        glBindVertexArray(0);
        glBindTexture(GL_TEXTURE_2D, 0);
        
        glDisable(GL_DEPTH_TEST);
        glDisable(GL_CULL_FACE);
        return 0;
    }
    
    int quit( )
    {
        glDeleteVertexArrays(1, &m_vao);
        glDeleteTextures(1, &m_texture);
        
        release_program(m_program);
        release_widgets(m_widgets);
        return 0;
    }
    
    int render( )
    {
        // effacer l'image
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        if(key_state('r'))
        {
            clear_key_state('r');
            reload_program(m_program, "data/shaders/tonemap.glsl");
            program_print_errors(m_program);
        }
        
        glBindVertexArray(m_vao);
        glUseProgram(m_program);
        
        program_use_texture(m_program, "image", 0, m_texture, 0);
        program_uniform(m_program, "channels", vec4(m_red, m_green, m_blue, m_alpha));
        program_uniform(m_program, "gray", (float) m_gray);
        program_uniform(m_program, "compression", (float) m_compression / 10.f);
        program_uniform(m_program, "saturation", m_saturation);
        
        // dessine 1 triangle plein ecran
        glDrawArrays(GL_TRIANGLES, 0, 3);
        
        begin(m_widgets);
            value(m_widgets, "compression", m_compression, 1, 100, 1);
            value(m_widgets, "saturation", m_saturation, 0.f, 1000.f, 1.f);
        
        begin_line(m_widgets);
            button(m_widgets, "R", m_red);
            button(m_widgets, "G", m_green);
            button(m_widgets, "B", m_blue);
            button(m_widgets, "A", m_alpha);
            button(m_widgets, "gray", m_gray);
        end(m_widgets);
        
        draw(m_widgets, window_width(), window_height());
        return 1;
    }
    
protected:
    Image m_image;
    
    Widgets m_widgets;
    
    GLuint m_texture;
    GLuint m_program;
    GLuint m_vao;
    int m_red, m_green, m_blue, m_alpha, m_gray;
    int m_compression;
    float m_saturation;
};


int main( int argc, char **argv )
{
    if(argc == 1)
    {
        printf("usage: %s image.[bmp|png|jpg|tga|hdr]\n", argv[0]);
        return 0;
    }
    
    ImageViewer app(argv[1]);
    app.run();
    
    return 0;
}
