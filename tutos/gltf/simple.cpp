
#include "gltf.h"

#include "program.h"
#include "uniforms.h"
#include "orbiter.h"
#include "draw.h"

#include "app_camera.h"        // classe Application a deriver

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
    
    glLineWidth(2);
    
    return grid;
}

struct GLTF : public AppCamera
{
    GLTF( ) : AppCamera(1024,640, 3,3) {}
    
    int init( )
    {
        const char *filename= "data/robot.gltf";
        
        m_mesh= read_gltf_mesh( filename );
        if(m_mesh.triangle_count() == 0)
            return -1;
        
        size_t size= m_mesh.vertex_buffer_size() + m_mesh.normal_buffer_size() + m_mesh.index_buffer_size();
        printf("%dKB\n", int(size / 1024));
        
        m_lights= read_gltf_lights( filename );
        printf("%d lights\n", int(m_lights.size()));
        
        m_draw_light= Mesh(GL_LINES);
        {
            m_draw_light.color(Yellow());
            for(unsigned i= 0; i < m_lights.size(); i++)
            {
                Point light= m_lights[i].position;
                m_draw_light.vertex(light.x, 0, light.z);
                m_draw_light.vertex(light.x, light.y, light.z);
            }
        }
        
        m_cameras= read_gltf_cameras( filename );
        if(m_cameras.size() > 0)
        {
            m_camera= m_cameras[0];
            printf("fov %f\n", m_camera.fov);
            printf("aspect %f\n", m_camera.aspect);
            printf("znear %f\n", m_camera.znear);
            printf("zfar  %f\n", m_camera.zfar);
        }
        printf("%d camera\n", int(m_cameras.size()));
        
        m_repere= make_grid(20);
        
        Point pmin, pmax;
        m_repere.bounds(pmin, pmax);
        
        // parametrer la camera de l'application, renvoyee par la fonction camera()
        camera().lookat(pmin, pmax);
        
        // etat openGL par defaut
        glClearColor(0.2f, 0.2f, 0.2f, 1.f);        // couleur par defaut de la fenetre
        
        glClearDepth(1.f);                          // profondeur par defaut
        glDepthFunc(GL_LESS);                       // ztest, conserver l'intersection la plus proche de la camera
        glEnable(GL_DEPTH_TEST);                    // activer le ztest
        
        return 0;
    }
    
    
    int quit( ) 
    {
        return 0;
    }
    
    int render( )
    {
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
        
        Transform view= camera().view();
        Transform projection= camera().projection();
        
        if(key_state(' '))
        {
            view= m_camera.view;
            projection= m_camera.projection;
        }
        
        draw(m_repere, Identity(), view, projection);
        
        if(m_lights.size() > 0)
        {
            draw(m_draw_light, Identity(), view, projection);
            
            //
            DrawParam params;
            params.view(view);
            params.projection(projection);
            params.model(Identity());
            params.light( m_lights[0].position, m_lights[0].emission );
            
            draw(m_mesh, params);
        }
        else
        {
            draw(m_mesh, Identity(), view, projection);
        }
        
        return 1;
    }
    
    Mesh m_mesh;
    Mesh m_repere;
    Mesh m_draw_light;
    GLTFCamera m_camera;
    
    std::vector<GLTFCamera> m_cameras;
    std::vector<GLTFLight> m_lights;
};

int main( int argc, char **argv )
{
    GLTF app;
    app.run();
    
    return 0;
}
