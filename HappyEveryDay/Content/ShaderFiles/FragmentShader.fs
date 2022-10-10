#version 330 core

out vec4 FragColor;
//in vec3 OurColor;
in vec3 CurPos;

// in vec4 vertexColor; // the input variable from the vertex shader (same name and same type)  



void main()
{
    FragColor = vec4(CurPos,1.0);
}