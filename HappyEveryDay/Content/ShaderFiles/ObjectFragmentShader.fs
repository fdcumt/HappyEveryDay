#version 330 core

in vec3 Normal;  
in vec3 FragPos;  
in vec2 TextureCoord;  

out vec4 FragColor;

#define PointLightNumber 4

struct FDirectionLight 
{
    vec3 Direction;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;
};

struct FPointLight 
{
    vec3 Position;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Constant;
    float Linear;
    float Quadratic;
};

struct FSpotLight {
    vec3 Position;
    vec3 Direction;
    //vec3 Direction;

    vec3 Ambient;
    vec3 Diffuse;
    vec3 Specular;

    float Cutoff; // 切光角
    float OuterCutoff; // 外切光角

    float Constant;
    float Linear;
    float Quadratic;
};

struct Material 
{
    float Shininess;
    vec3 Ambient;
    sampler2D Diffuse;
    sampler2D Specular;    
    sampler2D Emission;
}; 


uniform vec3 ViewPos;
uniform FDirectionLight DirectionLight;
uniform FPointLight PointLights[PointLightNumber];
uniform FSpotLight SpotLight;

uniform Material material;


vec3 CalcDirectionLight(FDirectionLight InLight, vec3 InNormal, vec3 InViewDir);
vec3 CalcPointLight(FPointLight InLight, vec3 InNormal, vec3 InViewDir);
vec3 CalcSpotLight(FSpotLight InLight, vec3 InNormal, vec3 InViewDir);

void main()
{
    vec3 Nor = normalize(Normal);
    vec3 ViewDir = normalize(FragPos-ViewPos);

    // calc direction light
    vec3 Result = CalcDirectionLight(DirectionLight, Nor, ViewDir);

    // calc point light
    for (int i=0; i<PointLightNumber; i++)
    {
       // Result += CalcPointLight(PointLights[i], Nor, ViewDir);
    }

    // calc spot light
    Result += CalcSpotLight(SpotLight, Nor, ViewDir);

    // calc emission light
   // Result += texture(material.Emission, TextureCoord).rgb;

    FragColor = vec4(Result, 1.0);
}

// InViewDir: ViewPos to FragPos
vec3 CalcDirectionLight(FDirectionLight InLight, vec3 InNormal, vec3 InViewDir)
{
    vec3 LightDir = normalize(InLight.Direction);

    // calc ambient
    vec3 Ambient = InLight.Ambient*texture(material.Diffuse, TextureCoord).rgb;

    // calc diffuse
    float DiffuseFactor = max(dot(-LightDir, InNormal), 0.0);
    vec3 Diffuse = DiffuseFactor*InLight.Diffuse*texture(material.Diffuse, TextureCoord).rgb;

    // calc specular
    vec3 RefectLight = reflect(-LightDir, InNormal);
    float SpecularFactor = pow(max(dot(RefectLight, -InViewDir), 0.0), material.Shininess);
    vec3 Specular = SpecularFactor*InLight.Specular*texture(material.Specular, TextureCoord).rgb;

    return Ambient+Diffuse+Specular;
}

vec3 CalcPointLight(FPointLight InLight, vec3 InNormal, vec3 InViewDir)
{
    vec3 LightDir = normalize(FragPos-InLight.Position);

    float Distance = length(FragPos-InLight.Position);
    float Attenuation = 1/(InLight.Constant+InLight.Linear*Distance+InLight.Quadratic*Distance*Distance);

    // calc ambient
    vec3 Ambient = InLight.Ambient*texture(material.Diffuse, TextureCoord).rgb;

    // calc diffuse
    float DiffuseFactor = max(dot(-LightDir, InNormal), 0.0);
    vec3 Diffuse = DiffuseFactor*InLight.Diffuse*texture(material.Diffuse, TextureCoord).rgb;

    // calc specular
    vec3 RefectLight = reflect(-LightDir, InNormal);
    float SpecularFactor = pow(max(dot(RefectLight, -InViewDir), 0.0), material.Shininess);
    vec3 Specular = SpecularFactor*InLight.Specular*texture(material.Specular, TextureCoord).rgb;

    return (Ambient+Diffuse+Specular)*Attenuation;
}

vec3 CalcSpotLight(FSpotLight InLight, vec3 InNormal, vec3 InViewDir)
{
    vec3 LightDir = normalize(FragPos-InLight.Position);

    float Distance = length(FragPos-InLight.Position);

    float CosTheta = dot(LightDir, normalize(InLight.Direction));
    float SpotFactor = clamp((CosTheta-InLight.OuterCutoff)/(InLight.Cutoff-InLight.OuterCutoff), 0.0, 1.0);

    float Attenuation = 1/(InLight.Constant+InLight.Linear*Distance+InLight.Quadratic*Distance*Distance);

    // calc ambient
    vec3 Ambient = InLight.Ambient*texture(material.Diffuse, TextureCoord).rgb;

    // calc diffuse
    float DiffuseFactor = max(dot(-LightDir, InNormal), 0.0);
    vec3 Diffuse = DiffuseFactor*InLight.Diffuse*texture(material.Diffuse, TextureCoord).rgb;

    // calc specular
    vec3 RefectLight = reflect(-LightDir, InNormal);
    float SpecularFactor = pow(max(dot(RefectLight, -InViewDir), 0.0), material.Shininess);
    vec3 Specular = SpecularFactor*InLight.Specular*texture(material.Specular, TextureCoord).rgb;

    return (Ambient+Diffuse+Specular)*Attenuation*SpotFactor;
}
