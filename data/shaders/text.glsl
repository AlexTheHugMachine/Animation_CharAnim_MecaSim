
#version 330

#ifdef VERTEX_SHADER

void main( )
{
    vec2 quad[4]= vec2[4]( vec2(-1, -1), vec2(1, -1), vec2(-1,  1), vec2(1,  1) );

    gl_Position= vec4(quad[gl_VertexID], 0, 1);
}
#endif


#ifdef FRAGMENT_SHADER

uniform sampler2D font;
uniform int offset;

layout(std140) uniform textData {
    ivec4 text[3072/4]; // 128x24 console, 8x16 font == 1024x768/2 viewport
};
// argh, avec std140 un int est aligne sur 16B !! tant qu'a faire utilise toutes les composantes...

out vec4 fragment_color;

void main( )
{
    ivec2 pixel= ivec2(gl_FragCoord.xy) - ivec2(0, offset);
    ivec2 cell=  pixel / ivec2(8, 16);
    if(cell.x < 0 || cell.y < 0)
        discard;
    if(cell.x >= 128 || cell.y >= 24)
        discard;
    
    ivec4 codes= text[cell.y * 128/4 + cell.x/4];
    int code= codes[cell.x%4];
    ivec2 glyph= ivec2(code % 16, 7 - code / 16);
    
    ivec2 uv= pixel % ivec2(8, 16) + glyph * ivec2(8, 16);
    vec4 color= texelFetch(font, uv, 0);
    if(color.a < 0.5)
        discard;
    
    fragment_color= color;
}
#endif
