#version 330 core

in vec3 Normal;  
in vec3 FragPos;  


out vec4 FragColor;


uniform float AmbientStrength;
uniform vec3 ObjectColor;
uniform vec3 LightColor;
uniform vec3 LightPos;

void main()
{
    // cal ambient
    vec3 Ambint = LightColor*AmbientStrength;

    // cal diffuse
    vec3 NormalizeNormal = normalize(Normal);
    vec3 LightDir = normalize(LightPos - FragPos);
    float DiffuseFactor = max(dot(NormalizeNormal, LightDir), 0.0);
    vec3 Diffuse = DiffuseFactor*LightColor;

    FragColor = vec4((Ambint+Diffuse) * ObjectColor, 1.0);
}