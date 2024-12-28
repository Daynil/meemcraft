#shader vertex
#version 330 core

// GLSL is the opengl shading language
// gl_Position must be a vec4, and since we told OpenGL we are passing in a vec3, we do so here
in vec3 position;
in vec2 textureCoord;

out vec2 pass_textureCoord;

uniform mat4 transform;
uniform mat4 projection;
//uniform mat4 view;

void main()
{
    // This is a predefined variable by OpenGL
    //l_Position = vec4(position, 1.0);
    //gl_Position = projection * view * transform * vec4(position, 1.0);
    gl_Position = projection * transform * vec4(position, 1.0);
    pass_textureCoord = textureCoord;
};

#shader fragment
#version 330 core

in vec2 pass_textureCoord;

out vec4 fragColor;

uniform sampler2D modelTexture;
uniform vec4 u_color;

void main()
{
    vec4 texture_color = texture(modelTexture, pass_textureCoord);
    texture_color.rgb = mix(texture_color.rgb, u_color.rgb, u_color.a);

    fragColor = texture_color;
};