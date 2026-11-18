#include <print>
#include "glfw3.h"

int main() {
	std::println ("glfw initialized: {}", glfwInit ());

    glfwTerminate ();
	return 0;
}
