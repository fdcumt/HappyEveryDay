#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 inNormal;


uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;

uniform vec3 LightPos;


out vec3 FragPos;
out vec3 Normal;
out vec3 LightPosInViewSpace;

void main()
{
	Normal = mat3(transpose(inverse(View*Model)))*inNormal;
	FragPos = vec3(View*Model*vec4(aPos, 1.0));
	LightPosInViewSpace = vec3(View*vec4(LightPos, 1.0));

	gl_Position = Projection*View*Model*vec4(aPos, 1.0);
}