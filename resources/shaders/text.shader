#shader vertex
#version 330 core
layout (location = 0) in vec4 vertex; // <vec2 pos, vec2 tex>
out vec2 TexCoords;

uniform mat4 projection;

void main()
{
    gl_Position = projection * vec4(vertex.xy, 0.0, 1.0);
    TexCoords = vertex.zw;
} 

#shader fragment
#version 330 core
in vec2 TexCoords;
out vec4 color;

uniform sampler2D text;
uniform vec3 textColor;

void main()
{    
    // Varying the output color's alpha value allows us to make all the glyph's
    // background colors transparent and non-transparent for the actual character
    vec4 sampled = vec4(1.0, 1.0, 1.0, texture(text, TexCoords).r);
    // Vary the text color via our own uniform we can set.
    color = vec4(textColor, 1.0) * sampled;
} 