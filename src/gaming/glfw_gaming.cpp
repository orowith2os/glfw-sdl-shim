#pragma GCC diagnostic ignored "-Wunused-parameter"

#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"

constexpr int max_glfw_joysticks = 16;

extern "C" {

int
glfwJoystickPresent (int jid)
{
  return GLFW_FALSE; // TODO
}

const float *
glfwGetJoystickAxes (int jid, int *count)
{
  return nullptr; // TODO
}

const unsigned char *
glfwGetJoystickButtons (int jid, int *count)
{
  return nullptr; // TODO
}

const unsigned char *
glfwGetJoystickHats (int jid, int *count)
{
  return nullptr; // TODO
}

const char *
glfwGetJoystickName (int jid)
{
  return nullptr; // TODO
}

const char *
glfwGetJoystickGUID (int jid)
{
  return nullptr; // TODO
}

void
glfwSetJoystickUserPointer (int jid, void *pointer)
{
  // TODO
}

void *
glfwGetJoystickUserPointer (int jid)
{
  return nullptr; // TODO
}

int
glfwJoystickIsGamepad (int jid)
{
  return GLFW_FALSE; // TODO
}

GLFWjoystickfun
glfwSetJoystickCallback (GLFWjoystickfun callback)
{
  ASSERT_GLFW_INIT (nullptr)
  ASSERT_MAIN_THREAD (nullptr)

  GLFWjoystickfun previous = state.joystick_callback;
  state.joystick_callback = callback;
  return previous;
}

int
glfwUpdateGamepadMappings (const char *string)
{
  return GLFW_FALSE; // TODO
}

const char *
glfwGetGamepadName (int jid)
{
  return nullptr; // TODO
}

int
glfwGetGamepadState (int jid, GLFWgamepadstate *gamepad_state)
{
  return GLFW_FALSE; // TODO
}

} // extern "C"
