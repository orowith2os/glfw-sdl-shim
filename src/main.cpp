#include <print>
#include <vulkan/vulkan.h>
#include "glfw3.h"

int main() {
    if (!glfwInit()) return -1;

    GLFWwindow* window = glfwCreateWindow(640, 480, "", NULL, NULL);
    if (!window) {
        glfwTerminate();
        return -1;
    }

    while (!glfwWindowShouldClose(window)) {
        glfwWaitEvents();
    }

    glfwTerminate();
    return 0;
}
