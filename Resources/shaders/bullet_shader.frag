#version 450

layout(binding = 2) uniform sampler2D texMainSampler;
layout(binding = 3) uniform sampler2D texSubSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec3 fragMainColor;
layout(location = 2) in vec3 fragSubColor;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 _mainCol = texture(texMainSampler, fragTexCoord) * vec4(fragMainColor, 1.0);
    vec4 _subCol = texture(texSubSampler, fragTexCoord) * vec4(fragSubColor, 1.0);
    vec4 _finalTex = clamp(_mainCol + _subCol, 0.0, 1.0);

    outColor = _finalTex;
}
