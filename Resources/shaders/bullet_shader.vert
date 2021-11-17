#version 450

// parent object orientation
layout(binding = 0) uniform UniformBufferObject
{
    mat4 model;
    mat4 view;
    mat4 proj;
} ubo;

// Per vertex
layout(location = 0) in vec3 inPosition;
layout(location = 1) in vec3 inColor;
layout(location = 2) in vec2 inTexCoord;

// Per instance
layout(location = 3) in vec2 inPositionOffset;
layout(location = 4) in float inRotation;
layout(location = 5) in vec4 inMainColor;
layout(location = 6) in vec4 inSubColor;

layout(location = 0) out vec2 fragTexCoord;
layout(location = 1) out vec4 fragMainColor;
layout(location = 2) out vec4 fragSubColor;

void main()
{
    vec4 _finalPos = ubo.proj * ubo.view * ubo.model * mat4(vec4(cos(inRotation), -sin(inRotation), 0.0, 0.0), vec4(sin(inRotation), cos(inRotation), 0.0, 0.0), vec4(0.0, 0.0, 1.0, 0.0), vec4(0.0, 0.0, 0.0, 1.0)) * vec4(inPosition, 1.0) + vec4(inPositionOffset, 0.0, 0.0);

    _finalPos.z = 0.0;

    gl_Position = _finalPos;
    fragTexCoord = inTexCoord;
    fragMainColor = inMainColor;
    fragSubColor = inSubColor;
}
