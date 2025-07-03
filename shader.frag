
#version 460 core 

out vec4 FragColor;

uniform sampler2D uTexture;

in vec2 TexCoord;
in vec3 Normal;

void main() {

    vec3 unitNormal = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.0, 1.0, 0.0));

    float brightness = max(dot(unitNormal, lightDir), 0.0);
    brightness = max(brightness, 0.1);
    
    FragColor = vec4(brightness, brightness, brightness, 1.0);
}

