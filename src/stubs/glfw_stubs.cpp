#include "glfw3.h"

extern "C" {

int
glfwRawMouseMotionSupported ()
{
  return GLFW_TRUE;
}

void
glfwSetGamma (GLFWmonitor *, float)
{
}

const GLFWgammaramp *
glfwGetGammaRamp (GLFWmonitor *)
{
  return nullptr;
}

void
glfwSetGammaRamp (GLFWmonitor *, const GLFWgammaramp *)
{
}

} // extern "C"
