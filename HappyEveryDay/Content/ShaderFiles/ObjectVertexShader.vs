#version 330 core
layout (location = 0) in vec3 inPos;
layout (location = 1) in vec2 inTextureCoord;
layout (location = 2) in vec3 inNormal;


uniform mat4 Model;
uniform mat4 View;
uniform mat4 Projection;


out vec3 FragPos;
out vec2 TextureCoord;
out vec3 Normal;

void main()
{
	Normal = mat3(transpose(inverse(Model)))*inNormal;
	FragPos = vec3(Model*vec4(inPos, 1.0));
	TextureCoord = inTextureCoord;

	gl_Position = Projection*View*Model*vec4(inPos, 1.0);
}