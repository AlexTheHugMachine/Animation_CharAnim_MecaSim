
#include <ctype.h>
#include <stdio.h>

#include "wavefront.h"


mesh read_obj( const char *filename )
{
    mesh data= make_mesh(GL_TRIANGLES);
    
    FILE *in= fopen(filename, "rt");
    if(in == NULL)
    {
        printf("loading obj '%s'... failed.\n", filename);
        return data;
    }
    
    printf("loading obj '%s'...\n", filename);
    
    std::vector<vec3> positions;
    std::vector<vec2> texcoords;
    std::vector<vec3> normals;
    
    char line_buffer[1024];
    bool error= true;
    for(;;)
    {
        // charge une ligne du fichier
        if(fgets(line_buffer, sizeof(line_buffer), in) == NULL)
        {
            error= false;       // fin du fichier, pas d'erreur detectee
            break;
        }
        
        // force la fin de la ligne, au cas ou
        line_buffer[sizeof(line_buffer) -1]= 0;
        
        // saute les espaces en debut de ligne
        char *line= line_buffer;
        while(*line && isspace(*line))
            line++;
        
        if(line[0] == 'v')
        {
            if(line[1] == ' ')          // position x y z
            {
                float x, y, z;
                if(sscanf(line, "v %f %f %f", &x, &y, &z) != 3)
                    break;
                positions.push_back( make_vec3(x, y, z) );
            }
            else if(line[1] == 'n')     // normal x y z
            {
                float x, y, z;
                if(sscanf(line, "vn %f %f %f", &x, &y, &z) != 3)
                    break;
                normals.push_back( make_vec3(x, y, z) );
            }
            else if(line[1] == 't')     // texcoord x y
            {
                float u, v;
                if(sscanf(line, "vt %f %f", &u, &v) != 2)
                    break;
                texcoords.push_back( make_vec2(u, v) );
            }
        }

        else if(line[0] == 'f')         // triangle a b c, les sommets sont numerotes a partir de 1 ou de la fin du tableau (< 0)
        {
            int ia, ita, ina;
            int ib, itb, inb;
            int ic, itc, inc;
            
            // chaque sommet est decrit par un triplet d'indices, position texcoord et normal
            // v/t/n format
            if(sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d", &ia, &ita, &ina, &ib, &itb, &inb, &ic, &itc, &inc) == 9)
            {
                if(ia < 0) ia= positions.size() + ia; else ia= ia -1;
                if(ita < 0) ita= texcoords.size() + ita; else ita= ita -1;
                if(ina < 0) ina= normals.size() + ina; else ina= ina -1;
                mesh_push_ptn_vertex(data, positions[ia], texcoords[ita], normals[ina]);
                
                if(ib < 0) ib= positions.size() + ib; else ib= ib -1;
                if(itb < 0) itb= texcoords.size() + itb; else itb= itb -1;
                if(inb < 0) inb= normals.size() + inb; else inb= inb -1;
                mesh_push_ptn_vertex(data, positions[ib], texcoords[itb], normals[inb]);

                if(ic < 0) ic= positions.size() + ic; else ic= ic -1;
                if(itc < 0) itc= texcoords.size() + itc; else itc= itc -1;
                if(inc < 0) inc= normals.size() + inc; else inc= inc -1;
                mesh_push_ptn_vertex(data, positions[ic], texcoords[itc], normals[inc]);
            }
            
            // v//n format
            else if(sscanf(line, "f %d//%d %d//%d %d//%d", &ia, &ina, &ib, &inb, &ic, &inc) == 6)
            {
                if(ia < 0) ia= positions.size() + ia; else ia= ia -1;
                if(ina < 0) ina= normals.size() + ina; else ina= ina -1;
                mesh_push_pn_vertex(data, positions[ia], normals[ina]);
                
                if(ib < 0) ib= positions.size() + ib; else ib= ib -1;
                if(inb < 0) inb= normals.size() + inb; else inb= inb -1;
                mesh_push_pn_vertex(data, positions[ib], normals[inb]);

                if(ic < 0) ic= positions.size() + ic; else ic= ic -1;
                if(inc < 0) inc= normals.size() + inc; else inc= inc -1;
                mesh_push_pn_vertex(data, positions[ic], normals[inc]);
            }
            
            // v/t  format
            else if(sscanf(line, "f %d/%d %d/%d %d/%d ", &ia, &ita, &ib, &itb, &ic, &itc) == 6)
            {
                if(ia < 0) ia= positions.size() + ia; else ia= ia -1;
                if(ita < 0) ita= texcoords.size() + ita; else ita= ita -1;
                mesh_push_pt_vertex(data, positions[ia], texcoords[ita]);
                
                if(ib < 0) ib= positions.size() + ib; else ib= ib -1;
                if(itb < 0) itb= texcoords.size() + itb; else itb= itb -1;
                mesh_push_pt_vertex(data, positions[ib], texcoords[itb]);

                if(ic < 0) ic= positions.size() + ic; else ic= ic -1;
                if(itc < 0) itc= texcoords.size() + itc; else itc= itc -1;
                mesh_push_pt_vertex(data, positions[ic], texcoords[itc]);
            }
            
            // v format
            else if(sscanf(line, "f %d %d %d ", &ia, &ib, &ic) == 3)
            {
                if(ia < 0) ia= positions.size() + ia; else ia= ia -1;
                mesh_push_vertex(data, positions[ia]);
                
                if(ib < 0) ib= positions.size() + ib; else ib= ib -1;
                mesh_push_vertex(data, positions[ib]);

                if(ic < 0) ic= positions.size() + ic; else ic= ic -1;
                mesh_push_vertex(data, positions[ic]);
            }
            
            else
                // erreur de structure dans le fichier...
                break;
        }
    }
    
    fclose(in);
    
    if(error)
        printf("loading obj '%s'...\n[error]\n%s\n\n", filename, line_buffer);
    
    return data;
}

