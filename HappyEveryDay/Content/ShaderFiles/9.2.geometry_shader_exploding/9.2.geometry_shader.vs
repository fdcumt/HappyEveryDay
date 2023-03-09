#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 2) in vec2 aTexCoords;

out VS_OUT {
    vec2 texCoords;
} vs_out;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
    vs_out.texCoords = aTexCoords;
    gl_Position = Projection * View * Model * vec4(aPos, 1.0); 
}