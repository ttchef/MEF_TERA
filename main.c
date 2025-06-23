
#include <GL/gl.h>
#include <GLFW/glfw3.h>

#include <stdio.h>

int main() {
    
    if (!glfwInit()) {
        fprintf(stderr, "Failed to initialize GLFW\n");
        return -1;
    }

    GLFWwindow* window = glfwCreateWindow(640, 480, "MEF TERA", NULL, NULL);
    if (!window) {
        fprintf(stderr, "Failed to create a Window!\n");
        glfwTerminate();
        return -1;
    }

    glfwMakeContextCurrent(window);

    while (!glfwWindowShouldClose(window)) {
        
        glClear(GL_COLOR_BUFFER_BIT);

        glfwSwapBuffers(window);
        glfwPollEvents();

    }

    glfwDestroyWindow(window);
    glfwTerminate();


    return 0;
}



