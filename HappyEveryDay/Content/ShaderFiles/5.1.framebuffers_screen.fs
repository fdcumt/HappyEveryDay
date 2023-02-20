#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ScreenTexture;

void main()
{
    vec3 CurColor = texture(ScreenTexture, TexCoords).rgb;
    float Average = 0.2126*CurColor.r+0.7152 *CurColor.g+CurColor.b*0.0722; // 加权灰度
    FragColor = vec4(Average, Average, Average, 1.0);
} 