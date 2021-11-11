#version 450

layout(binding = 0) uniform UniformBufferSpriteObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
    vec2 spriteScale;
} ubo;

layout(location = 0) in vec2 inPosition;
layout(location = 1) in vec2 inTexCoord;
layout(location = 2) in vec3 mainColor;
layout(location = 3) in vec3 subColor;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec3 fragMainColor;
layout(location = 2) out vec3 fragSubColor;

mat4 scale2D(mat4 matrix, vec2 s)
{
    matrix[0][0] *= s.x;
    matrix[1][1] *= s.y;

    return matrix;
}

void main()
{
    gl_Position = ubo.proj * ubo.view * scale2D(ubo.model, ubo.spriteScale) * vec4(inPosition, 0.0, 1.0);
    fragTexCoord = inTexCoord;
    fragMainColor = mainColor;
    fragSubColor = subColor;
}
