#version 330 core
out vec4 FragColor;

in vec3 ourColor;
in vec2 TexCoord;

// texture sampler
uniform sampler2D Texture1;
uniform sampler2D Texture2;
uniform float MixValue;

void main()
{
	FragColor = mix(texture(Texture1, TexCoord), texture(Texture2, TexCoord), MixValue)*vec4(ourColor, 1.0);
	//FragColor = texture(Texture2, TexCoord);
}