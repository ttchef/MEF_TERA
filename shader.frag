
#version 460 core 

out vec4 FragColor;

uniform sampler2D uTexture;
uniform float cubeHeight;

void main() {
    
    vec3 color;
    if (cubeHeight < 20) {
        color = vec3(0.0, 0.0, 1.0);
    }
    else if (cubeHeight < 40) {
        color = vec3(0.8, 0.8, 0.2);
    }
    else if (cubeHeight < 80) {
        color = vec3(0.5, 0.5, 0.5);
    }
    else if (cubeHeight < 300) {
        color = vec3(1.0, 1.0, 1.0);
    }
    FragColor = vec4(color, 1.0);
}

