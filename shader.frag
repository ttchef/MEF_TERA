
#version 460 core 


out vec4 FragColor;

uniform sampler2D uTexture;

void main() {
    
    
    /*
    if (val < 0.2) {
        FragColor = vec4(0.0, 0.0, 1.0, 1.0);
    }
    else if (val < 0.7) {
        FragColor = vec4(0.8, 0.8, 0.0, 1.0);
    }
    else {
        FragColor = vec4(0.5, 0.5, 0.5, 1.0);
    }
    */ 
    FragColor = vec4(0.5, 1.0, 0.0, 1.0);
    
}

