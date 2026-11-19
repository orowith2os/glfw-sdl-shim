#include <print>
#include <vulkan/vulkan.h>
#include "glfw3.h"

int main() {
    glfwSetErrorCallback([](int error, const char* description) {
        std::println(stderr, "GLFW Error ({:#x}): {}", error, description);
    });

    // 1. Initialize GLFW
    if (!glfwInit()) return -1;

    // 2. Create a windowed mode window and its OpenGL context
    GLFWwindow* window = glfwCreateWindow(640, 480, "Minimal GLFW", nullptr, nullptr);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    // 3. Make the window's context current
    glfwMakeContextCurrent(window);

    // 4. Main loop
    while (!glfwWindowShouldClose(window)) {
        // Set background color (RGBA) and clear the screen
        glClearColor(0.2f, 0.6f, 0.6f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // Swap front and back buffers
        glfwSwapBuffers(window);

        // Poll for and process events
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
