
#version 460 core 

in vec2 TexCoord;

out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    
    float val = texture(uTexture, TexCoord).r;
    FragColor = vec4(val, val, val, 1.0);

}
