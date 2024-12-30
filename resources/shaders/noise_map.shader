#shader vertex
#version 330 core
in vec3 position;
in vec2 textureCoord;

out vec2 pass_textureCoord;

uniform mat4 transform;
uniform mat4 projection;
uniform mat4 view;

void main()
{
    gl_Position = projection * view * transform * vec4(position, 1.0);
    pass_textureCoord = textureCoord;
} 

#shader fragment
#version 330 core
in vec2 pass_textureCoord;

out vec4 fragColor;

uniform sampler2D noiseData;

void main()
{
    fragColor = texture(noiseData, pass_textureCoord);
} 