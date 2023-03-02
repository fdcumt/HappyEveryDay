#version 330 core
layout (location = 0) in vec3 aPos;

layout (std140) uniform Matrices
{
    mat4 Projection;
    mat4 View;
};
uniform mat4 Model;

void main()
{
    gl_Position = Projection * View * Model * vec4(aPos, 1.0);
}  