
#version 460 core 

out vec4 FragColor;

uniform sampler2D uTexture;

in vec2 TexCoord;
in vec3 Normal;

uniform float uTime;

void main() {

    vec3 unitNormal = normalize(Normal);
    vec3 lightDir = normalize(vec3(0.0, 1.0, 0.0));

    float brightness = max(dot(unitNormal, lightDir), 0.0);
    brightness = max(brightness, 0.1);
    
    float red = (sin(uTime) + 1) / 2;
    float blue = (cos(uTime) + 1) / 2;

    FragColor = vec4(red * brightness, blue * brightness, brightness, 1.0);
}

