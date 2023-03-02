#version 330 core
out vec4 FragColor;

in vec2 TexCoords;

uniform sampler2D ScreenTexture;

const float offset = 1.0/300.0;

void main()
{
    vec2 offsets[9] = vec2[](
        vec2(-offset, offset), // 左上
        vec2(0.0f, offset), // 正上
        vec2(offset, offset), // 右上
        vec2(-offset, 0.0f), // 左
        vec2(0.0f, 0.0f), // 中
        vec2(offset, 0.0f), // 右
        vec2(-offset, -offset), // 左下
        vec2(0.0f, -offset),    // 正下
        vec2(offset, -offset)   // 右下
    );


    vec3 SampleTex[9];
    for(int i=0; i<9; i++)
    {
        SampleTex[i] = vec3(texture(ScreenTexture, TexCoords.st+offsets[i]));
    }

    vec3 Col = vec3(0.f);

    // 锐化效果
    float Kernel[9] = float[](
        -1, -1, -1,
        -1,  9, -1,
        -1, -1, -1
    );

    // 模糊效果
    float Blur[9] = float[](
        1, 2, 1,
        2, 4, 2,
        1, 2, 1
    );

    for(int i=0; i<9; i++)
    {
        Blur[i] = Blur[i]/16.f;
    }

    // 边缘检测效果
    float EdgeDetection[9] = float[](
        1, 1, 1,
        1, -8, 1,
        1, 1, 1
    );

    for(int i=0; i<9; i++)
    {
        EdgeDetection[i] = EdgeDetection[i]*5;
    }

    for (int i=0; i<9; i++)
    {
        Col += SampleTex[i] * EdgeDetection[i];
    }

    FragColor = vec4(Col, 1.0);
} 