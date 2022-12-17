#version 330 core

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TextureCoord;  

out vec4 FragColor;


struct Material {
    float Shininess;
    vec3 Ambient;
    sampler2D Diffuse;
    sampler2D Specular;    
    sampler2D Emission;
}; 

struct Light {
    vec3 Position;
    vec3 Direction;
    //vec3 Direction;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Cutoff; // 切光角

    float Constant;
    float Linear;
    float Quadratic;
};

uniform vec3 ViewPos;

uniform Material material;
uniform Light light;


void main()
{
    vec3 DirLight2Frag = normalize(FragPos-light.Position);

    if (dot(DirLight2Frag, normalize(light.Direction))>=light.Cutoff)
    {
        float DisLight2FragPos = length(light.Position-FragPos);

        // cal attenuation
        float Attenuation = 1/(light.Constant+light.Linear*DisLight2FragPos+light.Quadratic*DisLight2FragPos*DisLight2FragPos);

        // cal ambient
        vec3 Ambint = light.Ambient*texture(material.Diffuse, TextureCoord).rgb;

        // cal diffuse
        vec3 NormalizeNormal = normalize(Normal);
        //vec3 LightDir = normalize(light.Direction);
        vec3 LightDir = normalize(light.Position-FragPos);
        float DiffuseFactor = max(dot(NormalizeNormal, LightDir), 0.0);
        vec3 Diffuse = light.Diffuse*DiffuseFactor*texture(material.Diffuse, TextureCoord).rgb;

        // cal specular
        vec3 ViewDir = normalize(ViewPos - FragPos);
        vec3 LightReflectDir = reflect(-LightDir, NormalizeNormal);
        float SpecFactor = pow(max(dot(ViewDir, LightReflectDir), 0.0), material.Shininess);
        vec3 Specular = SpecFactor*light.Specular*texture(material.Specular, TextureCoord).rgb;

        // cal emission
        //vec3 Emission = texture(material.Emission, TextureCoord).rgb;

        //vec3 Result = (Ambint+Diffuse+Specular)*Attenuation+Emission;
        vec3 Result =Ambint + (Diffuse+Specular)*Attenuation;
        //vec3 Result = Ambint+Diffuse+Emission;
        vec3 TestColor = vec3(DiffuseFactor, DiffuseFactor, DiffuseFactor);
        FragColor = vec4(Result, 1.0);
    }
    else
    {
        FragColor = vec4(light.Ambient*texture(material.Diffuse, TextureCoord).rgb*2, 1.0);
    }
}