#version 330 core

in vec3 Normal;  
in vec3 FragPos;  
in vec3 LightPosInViewSpace;  
in vec3 LightingColor; 


out vec4 FragColor;


uniform float AmbientStrength;
uniform float SpecularStrength;

uniform vec3 ObjectColor;
uniform vec3 LightColor;
uniform vec3 LightPos;
uniform vec3 ViewPos;

uniform mat4 View;


void main()
{
    //// cal ambient
    //vec3 Ambint = LightColor*AmbientStrength;
//
    //// cal diffuse
    //vec3 NormalizeNormal = normalize(Normal);
    //vec3 LightDir = normalize(LightPosInViewSpace-FragPos);
    //float DiffuseFactor = max(dot(NormalizeNormal, LightDir), 0.0);
    //vec3 Diffuse = DiffuseFactor*LightColor;
//
    //// cal specular
    //vec3 ViewDir = normalize(-FragPos);
    //vec3 LightReflectDir = reflect(-LightDir, NormalizeNormal);
    //float Spec = pow(max(dot(ViewDir, LightReflectDir), 0.0), 32);
    //vec3 Specular = SpecularStrength*Spec*LightColor;

    //FragColor = vec4((Ambint+Diffuse+Specular) * ObjectColor, 1.0);
    FragColor = vec4(LightingColor* ObjectColor, 1.0);
}