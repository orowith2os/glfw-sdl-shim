#include <print>
#include "glfw3.h"

int main() {
    const char *error_str = nullptr;
    std::println ("glfw initialized: {}", glfwInit ());
    std::println ("error log: {}, {}", glfwGetError(&error_str), error_str);

    glfwTerminate ();
    return 0;
}
