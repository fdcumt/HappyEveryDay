#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec3 inNormal;


uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;


out vec3 FragPos;
out vec3 Normal;

void main()
{
	Normal = mat3(transpose(inverse(Model)))*inNormal;
	FragPos = vec3(Model*vec4(aPos, 1.0));

	gl_Position = Projection*View*Model*vec4(aPos, 1.0);
}