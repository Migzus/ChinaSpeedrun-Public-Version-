#version 450

layout(binding = 1) uniform sampler2D texMainSampler;
layout(binding = 2) uniform sampler2D texSubSampler;

layout(location = 0) in vec2 fragTexCoord;
layout(location = 1) in vec4 fragMainColor;
layout(location = 2) in vec4 fragSubColor;

layout(location = 0) out vec4 outColor;

void main()
{
    vec4 _mainCol = texture(texMainSampler, fragTexCoord) * fragMainColor;
    vec4 _subCol = texture(texSubSampler, fragTexCoord) * fragSubColor;
    vec4 _finalTex = clamp(_mainCol + _subCol, 0.0, 1.0);

    outColor = _finalTex;
}
