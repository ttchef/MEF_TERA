
#include "glad/glad.h"
#include <GLFW/glfw3.h>

#include "include/FastNoiseLite.h"

#include <stdio.h>
#include <stdlib.h>

#define SCREENWIDTH 640 
#define SCREENHEIGHT 480

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



