
#version 460 core 

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    
    float val = texture(uTexture, TexCoord).r;
    
    if (val < 0.2) {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
    else if (val < 0.7) {
        FragColor = vec4(0.3, 0.6, 0.0, 1.0);
    }
    else {
        FragColor = vec4(0.5, 0.5, 0.5, 1.0);
    }


}
