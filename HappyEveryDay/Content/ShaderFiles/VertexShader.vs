#version 330 core
layout (location = 0) in vec3 aPos; // the position variable has attribute position 0
layout (location = 1) in vec3 aColor; // the color variable has attribute position 1
  
//out vec3 OurColor; // specify a color output to the fragment shader
out vec3 CurPos; 

uniform float XOffset;

void main()
{
    gl_Position = vec4(aPos.x+XOffset, -aPos.y, aPos.z, 1.0); // see how we directly give a vec3 to vec4's constructor
   // OurColor = aColor;
    CurPos = aPos;
}