#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 aNormal;

out VS_OUT {
    vec3 ViewSpaceNormal;
} vs_out;

uniform mat4 Projection;
uniform mat4 View;
uniform mat4 Model;

void main()
{
    mat4 TempMat = View * Model;
    vs_out.ViewSpaceNormal = mat3(TempMat)*aNormal;
    gl_Position =  TempMat*vec4(aPos, 1.0); 

    //mat3 normalMatrix = mat3(transpose(inverse(View * Model)));
    //vs_out.ViewSpaceNormal = vec3(vec4(normalMatrix * aNormal, 0.0));
    //gl_Position = View * Model * vec4(aPos, 1.0); 
}
