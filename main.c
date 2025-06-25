
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#define FNL_IMPL
#include "include/FastNoiseLite.h"

#include <stdio.h>
#include <stdlib.h>

#define SCREENWIDTH 640 
#define SCREENHEIGHT 480

#define MAX_SHADER_LENGTH 3000

// needs to be freed 
const char* getShaderCode(const char* filepath) {
    FILE* file = fopen(filepath, "r");
    if (file == NULL) {
        fprintf(stderr, "[ERROR] Failed to read from file: %s\n", filepath);
        return "";
    }
    
    char* buffer = malloc(sizeof(char) * MAX_SHADER_LENGTH);
    size_t bytesRead = fread(buffer, sizeof(char), MAX_SHADER_LENGTH, file);
    buffer[bytesRead] = '\0';

    fclose(file);

    return buffer;
}

int main() {
    
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(SCREENWIDTH, SCREENHEIGHT, "MEF TERA", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create a Window!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);
    gladLoadGL();

    // Create Noise 
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    float* noiseData = malloc(SCREENWIDTH * SCREENHEIGHT * sizeof(float));
    int index = 0;

    // precompute noise 
    for (int y = 0; y < SCREENHEIGHT; y++) {
        for (int x = 0; x < SCREENWIDTH; x++) {
            noiseData[index++] = fnlGetNoise2D(&noise, x, y);
        }
    }

    // OpenGL stuff 
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vShaderSource = getShaderCode("shader.vert");
    const char* fShaderSource = getShaderCode("shader.frag");

    glShaderSource(vShader, 1, &vShaderSource, NULL);
    glShaderSource(fShader, 1, &fShaderSource, NULL);

    glCompileShader(vShader);
    glCompileShader(fShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);

    while (!glfwWindowShouldClose(window)) {
        
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);

        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();

    free(noiseData);

    return 0;
}



