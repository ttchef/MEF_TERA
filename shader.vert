
#version 460 core

layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 aTextureCoord;
layout (location = 2) in vec3 aNormal;

uniform mat4 mv_matrix;
uniform mat4 proj_matrix;

out vec2 TexCoord;
out vec3 Normal;

void main() {
    
    TexCoord = aTextureCoord;
    Normal = aNormal;
    gl_Position = proj_matrix * mv_matrix * vec4(aPos, 1.0);

}

