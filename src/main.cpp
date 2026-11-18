#include <print>
#include <vulkan/vulkan.h>
#include "glfw3.h"

int main() {
    const char *error_str = nullptr;
    std::println ("glfw initialized: {}", glfwInit ());
    glfwInitVulkanLoader (nullptr);
    std::println ("error log: {}, {}", glfwGetError(&error_str), error_str);

    glfwTerminate ();
    return 0;
}
