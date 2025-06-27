
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#define FNL_IMPL
#include "include/FastNoiseLite.h"
#include "include/HandmadeMath.h"

#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>

unsigned int screenWidth = 1000;
unsigned int screenHeight = 1000;

#define MAX_SHADER_LENGTH 3000
#define VERTEX_WIDTH 600
#define VERTEX_DEPTH 600

// needs to be freed 
char* getShaderCode(const char* filepath) {
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

float* computeVertices(unsigned int width, unsigned int depth) {
    
    unsigned int verticesCount = width * depth;
    float* vertices = malloc(sizeof(float) * verticesCount);

    float stepZ = 1.0f / depth;
    float stepX = 1.0f / width;

    unsigned int index = 0;
    for (float z = -1.0f; z < 1; z += stepZ) {
        for (float x = -1.0f; x < 1; x += stepX) {
            vertices[index++] = x;
            vertices[index++] = 0.0f;
            vertices[index++] = z;
        }
    }
    
    return vertices;
    
}

void glfwSizeCallback(GLFWwindow* window, int width, int height) {
    
    screenWidth = width;
    screenHeight = height;
    glViewport(0, 0, width, height);
}

int main() {
    
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    GLFWwindow* window = glfwCreateWindow(screenWidth, screenHeight, "MEF TERA", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create a Window!\n");
        glfwTerminate();
        return -1;
    }

    // glfw Setup
    glfwMakeContextCurrent(window);
    glfwSetWindowSizeCallback(window, glfwSizeCallback);

    // Glad & OpenGL Setup
    gladLoadGL();
    glViewport(0, 0, screenWidth, screenHeight);
    glEnable(GL_DEPTH_TEST);
    glEnable(GL_LEQUAL);

    // Create Noise 
    fnl_state noise = fnlCreateState();
    noise.noise_type = FNL_NOISE_OPENSIMPLEX2;
    float* noiseData = malloc(screenWidth * screenHeight * sizeof(float));

    // precompute noise 
    for (int y = 0; y < screenHeight; y++) {
        for (int x = 0; x < screenWidth; x++) {
            noiseData[y * screenWidth + x] = (fnlGetNoise2D(&noise, x, y) + 1.0f) / 2.0f;
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
        1.0f, -1.0f, -1.0f,
        1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, 1.0f,
        -1.0f, -1.0f, -1.0f,
        1.0f, 1.0f, -1.0f,
        1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, 1.0f,
        -1.0f, 1.0f, -1.0f
    };

    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    unsigned int EBO;
    glGenBuffers(1, &EBO);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);

    unsigned int indices[] = {
        1, 2, 3,
        7, 6, 5,
        4, 5, 1,
        5, 6, 2, 
        2, 6, 7,
        0, 3, 7,
        0, 1, 3,
        4, 7, 5,
        0, 4, 1,
        1, 5, 2,
        3, 2, 7,
        4, 0, 7
    };

    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
    glEnableVertexAttribArray(0);

    /*
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));
    glEnableVertexAttribArray(1);
    */

    // Texture
    unsigned int texture;
    glGenTextures(1, &texture);
    glBindTexture(GL_TEXTURE_2D, texture);

    glTexImage2D(GL_TEXTURE_2D, 0, GL_R32F, screenWidth, screenHeight, 0, GL_RED, GL_FLOAT, noiseData);

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
    glUseProgram(shaderProgram);

    // matrices
    HMM_Vec3 cubeLoc = (HMM_Vec3){0.0f, -2.0f, 0.0f};
    HMM_Mat4 mMat, vMat, mvMat, pMat;

    // Camera
    HMM_Vec3 cameraLoc = (HMM_Vec3){-0.0f, -0.0f, -8.0f};
    HMM_Vec3 cameraOri = (HMM_Vec3){0.0f, 0.0f, 1.0f};
    HMM_Vec3 cameraUp = (HMM_Vec3){0.0f, 1.0f, 0.0f};

    float cameraSpeed = 5.0f;
    float cameraSens = 50.0f;
    float actualSpeed = 0.0f; 

    double oldMouseX, oldMouseY = 0;
    bool cameraFirstClick = true;

    double lastTime = 0.0;
    double currentTime = glfwGetTime();
    float deltaTime = (float)(currentTime - lastTime);
    lastTime = currentTime;
    actualSpeed = cameraSpeed * deltaTime;

    while (!glfwWindowShouldClose(window)) {
        
        currentTime = glfwGetTime();
        deltaTime = (float)(currentTime - lastTime);
        lastTime = currentTime;

        actualSpeed = cameraSpeed * deltaTime;

        glClearColor(0, 0, 0, 0);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        glBindVertexArray(VAO);
        glUseProgram(shaderProgram);
        
        mMat = HMM_Translate(cubeLoc);
        vMat = HMM_LookAt_RH(cameraLoc, HMM_AddV3(cameraLoc, cameraOri), cameraUp);
        mvMat = HMM_MulM4(vMat, mMat);

        // NO = NDC-Z from -1 to 1
        // RH = Right Handed Coord System
        pMat = HMM_Perspective_RH_NO(1.0472f, (float)screenWidth / (float)screenHeight, 0.1f, 1000.0f);

        unsigned int mvLoc = glGetUniformLocation(shaderProgram, "mv_matrix");
        unsigned int projLoc = glGetUniformLocation(shaderProgram, "proj_matrix");

        glUniformMatrix4fv(mvLoc, 1, GL_FALSE, (float*)mvMat.Elements);
        glUniformMatrix4fv(projLoc, 1, GL_FALSE, (float*)pMat.Elements);

        glDrawElements(GL_TRIANGLES, 38, GL_UNSIGNED_INT, 0);

        // Input
        if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS) {
            HMM_Vec3 newPos = HMM_MulV3F(cameraOri, actualSpeed);
            cameraLoc = HMM_AddV3(cameraLoc, newPos);
        }
        if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS) {
            HMM_Vec3 negated_co = {-cameraOri.X, -cameraOri.Y, -cameraOri.Z};
            HMM_Vec3 newPos = HMM_MulV3F(negated_co, actualSpeed);
            cameraLoc = HMM_AddV3(cameraLoc, newPos);
        }
        if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS) {
            HMM_Vec3 cross_ori = HMM_Cross(cameraOri, cameraUp);
            HMM_Vec3 normalized_cross = HMM_NormV3(cross_ori);
            HMM_Vec3 negated_normalized_cross = {-normalized_cross.X, -normalized_cross.Y, -normalized_cross.Z};
            HMM_Vec3 newPos = HMM_MulV3F(negated_normalized_cross, actualSpeed);
            cameraLoc = HMM_AddV3(cameraLoc, newPos);
        }
        if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS) {
            HMM_Vec3 cross_ori = HMM_Cross(cameraOri, cameraUp);
            HMM_Vec3 normalized_cross = HMM_NormV3(cross_ori);
            HMM_Vec3 newPos = HMM_MulV3F(normalized_cross, actualSpeed);
            cameraLoc = HMM_AddV3(cameraLoc, newPos);
        }
        if (glfwGetKey(window, GLFW_KEY_SPACE) == GLFW_PRESS) {
            HMM_Vec3 newPos = HMM_MulV3F(cameraUp, actualSpeed);
            cameraLoc = HMM_AddV3(cameraLoc, newPos);
        }
        if (glfwGetKey(window, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS) {
            HMM_Vec3 negated_up = {-cameraUp.X, -cameraUp.Y, -cameraUp.Z};
            HMM_Vec3 newPos = HMM_MulV3F(negated_up, actualSpeed);
            cameraLoc = HMM_AddV3(cameraLoc, newPos);
        }

        if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_PRESS) {
            actualSpeed *= 2.0f;
        }
        else if (glfwGetKey(window, GLFW_KEY_LEFT_CONTROL) == GLFW_RELEASE) {
            actualSpeed *= 0.5f;
        }

        if (glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_PRESS) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_HIDDEN);

            double mouseX, mouseY;
            float rotX, rotY;
            
            glfwGetCursorPos(window, &mouseX, &mouseY);

            if (cameraFirstClick) {
                glfwSetCursorPos(window, screenWidth / 2.0f, screenHeight / 2.0f);
                cameraFirstClick = false;
                oldMouseX = mouseX;
                oldMouseY = mouseY;
            }
                           
            rotX = cameraSens * (float)(mouseY - oldMouseY) / screenHeight;
            rotY = cameraSens * (float)(mouseX - oldMouseX) / screenWidth;

            HMM_Vec3 cross_ori = HMM_Cross(cameraOri, cameraUp);
            HMM_Vec3 norm = HMM_NormV3(cross_ori);

            HMM_Vec3 newOri = HMM_RotateV3AxisAngle_RH(cameraOri, norm, -rotX * HMM_PI / 180.0f);
            
            cameraOri = newOri;
            cameraOri = HMM_RotateV3AxisAngle_RH(cameraOri, cameraUp, -rotY * HMM_PI / 180.0f);
            cameraOri = HMM_NormV3(cameraOri);

            oldMouseX = mouseX;
            oldMouseY = mouseY;

        }
        else if (!cameraFirstClick && glfwGetMouseButton(window, GLFW_MOUSE_BUTTON_LEFT) == GLFW_RELEASE) {
            glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
            cameraFirstClick = true;
            glfwSetCursorPos(window, screenWidth / 2.0f, screenHeight / 2.0f);  
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();

    free(noiseData);

    return 0;
}



