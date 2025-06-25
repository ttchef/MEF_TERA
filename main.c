
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
        return NULL;
    }
    
    char* buffer = malloc(sizeof(char) * MAX_SHADER_LENGTH);
    if (buffer == NULL) {
        fprintf(stderr, "[ERROR] Couldnt Allocate memory for Shader!\n");
        fclose(file);
        return NULL;
    }
    size_t bytesRead = fread(buffer, sizeof(char), MAX_SHADER_LENGTH, file);
    buffer[bytesRead] = '\0';

    fclose(file);

    return buffer;
}

void printShaderLog(unsigned int shader) {
    int len = 0;
    int chWritten = 0;
    char* log;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char*)malloc(len);
        glGetShaderInfoLog(shader, len, &chWritten, log);
        printf("[ERROR] Shader Log: %s\n", log);
        free(log);
    }
}

void printProgramLog(unsigned int program) {
    int len = 0;
    int chWritten = 0;
    char* log;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &len);
    if (len > 0) {
        log = (char*)malloc(len);
        glGetProgramInfoLog(program, len, &chWritten, log);
        printf("[ERROR] Shader Log: %s\n", log);
        free(log);
    }
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

    // precompute noise 
    for (int y = 0; y < SCREENHEIGHT; y++) {
        for (int x = 0; x < SCREENWIDTH; x++) {
            noiseData[y * SCREENWIDTH + x] = (fnlGetNoise2D(&noise, x, y) + 1.0f) / 2.0f;
        }
    }

    // OpenGL stuff 
    unsigned int VAO;
    glGenVertexArrays(1, &VAO);
    glBindVertexArray(VAO);

    unsigned int VBO;
    glGenBuffers(1, &VBO);
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    
    float vertices[] = {
        // Cords            // Texutre Cords
        -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
        -1.0f, 1.0f, 0.0f,  0.0f, 1.0f,
        1.0f, -1.0f, 0.0f,  1.0f, 0.0f,
        1.0f, 1.0f, 0.0f,   1.0f, 1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    unsigned int indices[] = {
        0, 1, 2, 
        1, 2, 3
    };

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);

    // Texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, SCREENWIDTH, SCREENHEIGHT, 0, GL_RED, GL_FLOAT, noiseData);

    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

    // Shader
    unsigned int vShader = glCreateShader(GL_VERTEX_SHADER);
    unsigned int fShader = glCreateShader(GL_FRAGMENT_SHADER);

    const char* vShaderSource = getShaderCode("shader.vert");
    const char* fShaderSource = getShaderCode("shader.frag");

    glShaderSource(vShader, 1, &vShaderSource, NULL);
    glShaderSource(fShader, 1, &fShaderSource, NULL);

    free((void*)vShaderSource);
    free((void*)fShaderSource);

    glCompileShader(vShader);
    printShaderLog(vShader);
    glCompileShader(fShader);
    printShaderLog(fShader);

    unsigned int shaderProgram = glCreateProgram();
    glAttachShader(shaderProgram, vShader);
    glAttachShader(shaderProgram, fShader);
    glLinkProgram(shaderProgram);
    printProgramLog(shaderProgram);


    while (!glfwWindowShouldClose(window)) {
        
        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT);

        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        
        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();

    free(noiseData);

    return 0;
}



