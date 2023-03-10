#version 330 core
layout (triangles) in;
layout (line_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 ViewSpaceNormal;
} vs_in[];

uniform mat4 Projection;

const float Magnitude = 0.2;

void GenerateLine(int i)
{
    gl_Position = Projection * gl_in[i].gl_Position;
    EmitVertex();

    gl_Position = Projection * vec4(gl_in[i].gl_Position.xyz+vs_in[i].ViewSpaceNormal*Magnitude, 1);
    EmitVertex();

    EndPrimitive();
}

void main() 
{    
    GenerateLine(0);
    GenerateLine(1);
    GenerateLine(2);
}