
#include <ctype.h>
#include <cstdio>

#include "wavefront.h"


Mesh read_mesh( const char *filename )
{
    Mesh data= create_mesh(GL_TRIANGLES);
    
    FILE *in= fopen(filename, "rt");
    if(in == NULL)
    {
        printf("loading mesh '%s'... failed.\n", filename);
        return data;
    }
    
    printf("loading mesh '%s'...\n", filename);
    
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
            int id, itd, ind;
            int code;
            
            // chaque sommet est decrit par un triplet d'indices, position texcoord et normal
            // v/t/n format
            code= sscanf(line, "f %d/%d/%d %d/%d/%d %d/%d/%d %d/%d/%d", &ia, &ita, &ina, &ib, &itb, &inb, &ic, &itc, &inc, &id, &itd, &ind);
            if(code >= 9)
            {
                if(ia < 0) ia= (int) positions.size() + ia; else ia= ia -1;
                if(ita < 0) ita= (int) texcoords.size() + ita; else ita= ita -1;
                if(ina < 0) ina= (int) normals.size() + ina; else ina= ina -1;
                push_vertex(data, positions[ia], texcoords[ita].x, texcoords[ita].y , normals[ina]);
                
                if(ib < 0) ib= (int) positions.size() + ib; else ib= ib -1;
                if(itb < 0) itb= (int) texcoords.size() + itb; else itb= itb -1;
                if(inb < 0) inb= (int) normals.size() + inb; else inb= inb -1;
                push_vertex(data, positions[ib], texcoords[itb].x, texcoords[itb].y, normals[inb]);

                if(ic < 0) ic= (int) positions.size() + ic; else ic= ic -1;
                if(itc < 0) itc= (int) texcoords.size() + itc; else itc= itc -1;
                if(inc < 0) inc= (int) normals.size() + inc; else inc= inc -1;
                push_vertex(data, positions[ic], texcoords[itc].x, texcoords[itc].y, normals[inc]);

                if(code == 12)
                {
                    push_vertex(data, positions[ia], texcoords[ita].x, texcoords[ita].y , normals[ina]);
                    push_vertex(data, positions[ic], texcoords[itc].x, texcoords[itc].y, normals[inc]);
                    
                    if(id < 0) id= (int) positions.size() + id; else id= id -1;
                    if(itd < 0) itd= (int) texcoords.size() + itd; else itd= itd -1;
                    if(ind < 0) ind= (int) normals.size() + ind; else ind= ind -1;
                    push_vertex(data, positions[id], texcoords[itd].x, texcoords[itd].y , normals[ind]);
                }
            }
            else
            {
                // v//n format
                code= sscanf(line, "f %d//%d %d//%d %d//%d %d//%d", &ia, &ina, &ib, &inb, &ic, &inc, &id, &ind);
                if(code >= 6)
                {
                    if(ia < 0) ia= (int) positions.size() + ia; else ia= ia -1;
                    if(ina < 0) ina= (int) normals.size() + ina; else ina= ina -1;
                    push_vertex(data, positions[ia], normals[ina]);
                
                    if(ib < 0) ib= (int) positions.size() + ib; else ib= ib -1;
                    if(inb < 0) inb= (int) normals.size() + inb; else inb= inb -1;
                    push_vertex(data, positions[ib], normals[inb]);

                    if(ic < 0) ic= (int) positions.size() + ic; else ic= ic -1;
                    if(inc < 0) inc= (int) normals.size() + inc; else inc= inc -1;
                    push_vertex(data, positions[ic], normals[inc]);
                    
                    if(code == 8)
                    {
                        push_vertex(data, positions[ia], normals[ina]);
                        push_vertex(data, positions[ic], normals[inc]);
                        
                        if(id < 0) id= (int) positions.size() + id; else id= id -1;
                        if(ind < 0) ind= (int) normals.size() + ind; else ind= ind -1;
                        push_vertex(data, positions[id], normals[ind]);
                    }
                }
                else
                {
                    // v/t  format
                    code= sscanf(line, "f %d/%d %d/%d %d/%d %d/%d", &ia, &ita, &ib, &itb, &ic, &itc, &id, &itd);
                    if(code >= 6)
                    {
                        if(ia < 0) ia= (int) positions.size() + ia; else ia= ia -1;
                        if(ita < 0) ita= (int) texcoords.size() + ita; else ita= ita -1;
                        push_vertex(data, positions[ia], texcoords[ita].x, texcoords[ita].y);
                
                        if(ib < 0) ib= (int) positions.size() + ib; else ib= ib -1;
                        if(itb < 0) itb= (int) texcoords.size() + itb; else itb= itb -1;
                        push_vertex(data, positions[ib], texcoords[itb].x, texcoords[itb].y);

                        if(ic < 0) ic= (int) positions.size() + ic; else ic= ic -1;
                        if(itc < 0) itc= (int) texcoords.size() + itc; else itc= itc -1;
                        push_vertex(data, positions[ic], texcoords[itc].x, texcoords[itc].y);
                        
                        if(code == 8)
                        {
                            push_vertex(data, positions[ia], texcoords[ita].x, texcoords[ita].y);
                            push_vertex(data, positions[ic], texcoords[itc].x, texcoords[itc].y);
                            
                            if(id < 0) id= (int) positions.size() + id; else id= id -1;
                            if(itd < 0) itd= (int) texcoords.size() + itd; else itd= itd -1;
                            push_vertex(data, positions[id], texcoords[itd].x, texcoords[itd].y);
                        }
                    }
                    else
                    {
                        // v format
                        code= sscanf(line, "f %d %d %d %d", &ia, &ib, &ic, &id);
                        if(code >= 3)
                        {
                            if(ia < 0) ia= (int) positions.size() + ia; else ia= ia -1;
                            push_vertex(data, positions[ia]);
                
                            if(ib < 0) ib= (int) positions.size() + ib; else ib= ib -1;
                            push_vertex(data, positions[ib]);

                            if(ic < 0) ic= (int) positions.size() + ic; else ic= ic -1;
                            push_vertex(data, positions[ic]);
                            
                            if(code == 4)
                            {
                                push_vertex(data, positions[ia]);
                                push_vertex(data, positions[ic]);
                                
                                if(id < 0) id= (int) positions.size() + id; else id= id -1;
                                push_vertex(data, positions[id]);
                            }
                        }
                        else
                            // erreur de structure dans le fichier...
                            break;
                    }
                }
            }
        }
    }
    
    fclose(in);
    
    if(error)
        printf("loading mesh '%s'...\n[error]\n%s\n\n", filename, line_buffer);
    
    return data;
}

int write_mesh( const Mesh& mesh, const char *filename )
{
    if(mesh.primitives != GL_TRIANGLES)
        return -1;
    if(mesh.positions.size() == 0)
        return -1;
    if(filename == NULL)
        return -1;
    
    FILE *out= fopen(filename, "wt");
    if(out == NULL)
        return -1;
    
    printf("writing mesh '%s'...\n", filename);
    
    for(unsigned int i= 0; i < (unsigned int) mesh.positions.size(); i++)
        fprintf(out, "v %f %f %f\n", mesh.positions[i].x, mesh.positions[i].y, mesh.positions[i].z);
    fprintf(out, "\n");
    
    bool has_texcoords= (mesh.texcoords.size() == mesh.positions.size());
    for(unsigned int i= 0; i < (unsigned int) mesh.texcoords.size(); i++)
        fprintf(out, "vt %f %f\n", mesh.texcoords[i].x, mesh.texcoords[i].y);
    fprintf(out, "\n");
    
    bool has_normals= (mesh.normals.size() == mesh.positions.size());
    for(unsigned int i= 0; i < (unsigned int) mesh.normals.size(); i++)
        fprintf(out, "vn %f %f %f\n", mesh.normals[i].x, mesh.normals[i].y, mesh.normals[i].z);
    fprintf(out, "\n");
    
    bool has_indices= (mesh.indices.size() > 0);
    unsigned int n= has_indices ? (unsigned int) mesh.indices.size() : (unsigned int) mesh.positions.size();
    
    for(unsigned int i= 0; i +2 < n; i+= 3)
    {
        fprintf(out, "f");
        for(unsigned int k= 0; k < 3; k++)
        {
            unsigned int id= has_indices ? mesh.indices[i+k] +1 : i+k +1;
            fprintf(out, " %u", id);
            if(has_texcoords && has_normals)
                fprintf(out, "/%u/%u", id, id);
            else if(has_texcoords)
                fprintf(out, "/%u", id);
            else if(has_normals)
                fprintf(out, "//%u", id);
        }
        fprintf(out, "\n");
    }
    
    fclose(out);
    return 0;
}
