#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 inNormal;


uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

out vec3 FragPos;
out vec3 Normal;
out vec3 LightPosInViewSpace;
out vec3 LightingColor; 

uniform float AmbientStrength;
uniform float SpecularStrength;

uniform vec3 ObjectColor;
uniform vec3 LightColor;
uniform vec3 LightPos;
uniform vec3 ViewPos;


void main()
{
	gl_Position = Projection*View*Model*vec4(aPos, 1.0);

	LightPosInViewSpace = vec3(View*vec4(LightPos, 1.0));

	Normal = mat3(transpose(inverse(View*Model)))*inNormal;
	FragPos = vec3(View*Model*vec4(aPos, 1.0));

	    // cal diffuse
    vec3 NormalizeNormal = normalize(Normal);
    vec3 LightDir = normalize(LightPosInViewSpace-FragPos);
    float DiffuseFactor = max(dot(NormalizeNormal, LightDir), 0.0);
    vec3 Diffuse = DiffuseFactor*LightColor;

    // cal specular
    vec3 ViewDir = normalize(-FragPos);
    vec3 LightReflectDir = reflect(-LightDir, NormalizeNormal);
    float Spec = pow(max(dot(ViewDir, LightReflectDir), 0.0), 32);
    vec3 Specular = SpecularStrength*Spec*LightColor;



	// cal ambient
    vec3 Ambint = LightColor*AmbientStrength;

	LightingColor = Ambint+Diffuse+Specular;
}