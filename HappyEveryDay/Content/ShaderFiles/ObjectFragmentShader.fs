#version 330 core

in vec3 Normal;  
in vec3 FragPos;  

out vec4 FragColor;


struct Material {
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;    
    float Shininess;
}; 

struct Light {
    vec3 Position;
    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

uniform vec3 ViewPos;

uniform Material material;
uniform Light light;


void main()
{
    // cal ambient
    vec3 Ambint = light.Ambient*material.Ambient;

    // cal diffuse
    vec3 NormalizeNormal = normalize(Normal);
    vec3 LightDir = normalize(light.Position-FragPos);
    float DiffuseFactor = max(dot(NormalizeNormal, LightDir), 0.0);
    vec3 Diffuse = light.Diffuse*material.Diffuse*DiffuseFactor;

    // cal specular
    vec3 ViewDir = normalize(ViewPos - FragPos);
    vec3 LightReflectDir = reflect(-LightDir, NormalizeNormal);
    float SpecFactor = pow(max(dot(ViewDir, LightReflectDir), 0.0), material.Shininess);
    vec3 Specular = SpecFactor*light.Specular*material.Specular;

    vec3 Result = Ambint+Diffuse+Specular;
    vec3 TestColor = vec3(DiffuseFactor, DiffuseFactor, DiffuseFactor);
    FragColor = vec4(Result, 1.0);
}