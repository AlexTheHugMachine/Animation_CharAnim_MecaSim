
#include <vector>

#include "glcore.h"
#include "window.h"

#include "transcoder/basisu_transcoder.h"
#include "ktx2.h"

#include "image.h"
#include "image_io.h"

unsigned read_ktx2_texture( const int unit, const char *filename )
{
    FILE *in= fopen(filename, "rb");
    if(in == nullptr)
    {
        printf("[error] loading ktx2 texture '%s'...\n", filename);
        return 0;
    }
    
#if 0
    size_t offset= 0;
    size_t size= 1024*64;
    std::vector<char> file(size);
    while(true)
    {
        printf("[read buffer] offset %lu size %lu\n", offset, size);
        size_t n= fread(file.data() + offset, 1, size - offset, in);
        if(n == 0 || n < size - offset)
            break;
        
        offset+= n;
        size+= n;
        file.resize(size);
    }
#else
    fseek(in, 0, SEEK_END);
    size_t size= ftell(in);
    
    fseek(in, 0, SEEK_SET);
    
    std::vector<char> file(size);
    if(fread(file.data(), 1, size, in) != size)
    {
        fclose(in);
        printf("[error] reading '%s'...\n", filename);
        return 0;
    }
#endif
    
    fclose(in);
    
    assert(basist::basisu_transcoder_supports_ktx2() == true);
    assert(basist::basisu_transcoder_supports_ktx2_zstd() == true);
    
    basist::basisu_transcoder_init();
    
    basist::ktx2_transcoder transcoder;
    transcoder.init(file.data(), file.size());
    
    printf("  %ux%u %u levels\n", transcoder.get_width(), transcoder.get_height(), transcoder.get_levels());
    printf("  alpha %u, channels %u\n", transcoder.get_has_alpha(), transcoder.get_dfd_total_samples());
    
    printf("  mode etc1s %d, uastc %d\n", transcoder.is_etc1s(), transcoder.is_uastc());
    basist::transcoder_texture_format format= basist::transcoder_texture_format::cTFASTC_4x4_RGBA;
    
    transcoder.start_transcoding();
    
    GLuint texture= 0;
    glGenTextures(1, &texture);
    glActiveTexture(GL_TEXTURE0 + unit);
    glBindTexture(GL_TEXTURE_2D, texture);
    
    for(unsigned level= 0; level <  transcoder.get_levels(); level++)
    {
        basist::ktx2_image_level_info level_info;
        transcoder.get_image_level_info(level_info, level, 0, 0);
        
        printf("level %d: %u 4x4 blocks, %u bytes/block\n", level, level_info.m_total_blocks, basist::basis_get_bytes_per_block_or_pixel(format));
        
        // alloue un buffer pour decompresser la texture
        std::vector<char> tmp(level_info.m_total_blocks * basist::basis_get_bytes_per_block_or_pixel(format));
        
        bool code= transcoder.transcode_image_level(level, /* layer */ 0, /* face */ 0, tmp.data(), tmp.size(), format);
        assert(code);
        
        // todo srgb ou linear !
        
        // initialise la texture openGL
        glCompressedTexImage2D(GL_TEXTURE_2D, level, GL_COMPRESSED_SRGB8_ALPHA8_ASTC_4x4_KHR,
            level_info.m_width, level_info.m_height, 0, tmp.size(), tmp.data());
        
        //~ {
            //~ // export
            //~ Image image(level_info.m_width, level_info.m_height);
            //~ for(int y= 0; y < image.height(); y++)
            //~ for(int x= 0; x < image.width(); x++)
                //~ image(x, y)= Color(tmp[y/4 * level_info.m_num_blocks_x*16 + x/4*16]);
            
            //~ char exportname[1024];
            //~ sprintf(exportname, "level%02d.png", level);
            //~ write_image(image, exportname);
        //~ }
    }
    
    return texture;
}

#if 0
int main( int argc, char **argv )
{
    const char *filename= "ivy.ktx";
    if(argc > 1)
        filename=argv[1];
    
    basist::basisu_transcoder_init();
    
    Window w= create_window(1024,640, 4,3);
    Context c= create_context(w);
    
    unsigned texture= read_ktx2_texture(0, filename);
    return 0;
}
#endif
