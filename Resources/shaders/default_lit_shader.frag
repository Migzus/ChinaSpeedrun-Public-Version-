#version 450 core

uniform sampler2D textureSampler;

uniform vec4 materialColor;

uniform vec4 lightColor;
uniform vec3 lightPosition;
uniform vec3 cameraPosition;
uniform float ambientStrength = 0.1;

uniform float intensity;
uniform float specularStrength = 0.5;
uniform int specularExponent = 64;

in vec2 uv;
in vec4 color;
in vec3 normalTransposed;
in vec3 fragmentPosition;

out vec4 fragmentColor;

void main() {
    vec3 ambient = color.rgb * ambientStrength;

    vec3 normalCorrection = normalize(normalTransposed);
    vec3 lightDirection = normalize(lightPosition - fragmentPosition);
    float angleFactor = max(dot(normalCorrection, lightDirection), 0.0);
    vec3 diffuse = angleFactor * color.rgb * lightColor.rgb * intensity;

    vec3 viewDirection = normalize(cameraPosition - fragmentPosition);
    float spec = 0.0;
    if (angleFactor > 0.0)
    {
        vec3 reflectionDirection = reflect(-lightDirection, normalCorrection);
        spec = pow(max(dot(viewDirection, reflectionDirection), 0.0), specularExponent);
    }

    vec3 specular = spec * lightColor.rgb * specularStrength;
    vec3 result = ambient + diffuse + specular;

    fragmentColor = texture(textureSampler, uv).bgra; // * materialColor * vec4(result, color.a);
}