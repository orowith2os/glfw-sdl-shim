#include <utility>

#include "glfw_window.hpp"
#include "helpers/glfw_helpers.hpp"

#include "glfw3.h"

#define SET_CB(func, member) \
  decltype (GLFWwindow::member) func (GLFWwindow *window, \
                                      decltype (GLFWwindow::member) cb) \
  { \
    ASSERT_GLFW_INIT (nullptr) \
    ASSERT_MAIN_THREAD (nullptr) \
    return window ? std::exchange (window->member, cb) : nullptr; \
  }

extern "C" {

SET_CB (glfwSetCharCallback, char_callback)
SET_CB (glfwSetCharModsCallback, char_mods_callback)
SET_CB (glfwSetDropCallback, drop_callback)
SET_CB (glfwSetCursorEnterCallback, cursor_enter_callback)
SET_CB (glfwSetCursorPosCallback, cursor_pos_callback)
SET_CB (glfwSetFramebufferSizeCallback, framebuffer_size_callback)
SET_CB (glfwSetMouseButtonCallback, mouse_button_callback)
SET_CB (glfwSetWindowFocusCallback, focus_callback)
SET_CB (glfwSetWindowIconifyCallback, iconify_callback)
SET_CB (glfwSetWindowMaximizeCallback, maximized_callback)
SET_CB (glfwSetWindowCloseCallback, close_callback)
SET_CB (glfwSetWindowPosCallback, window_pos_callback)
SET_CB (glfwSetWindowRefreshCallback, window_refresh_callback)
SET_CB (glfwSetWindowContentScaleCallback, content_scale_callback)
SET_CB (glfwSetWindowSizeCallback, window_size_callback)
SET_CB (glfwSetScrollCallback, scroll_callback)
SET_CB (glfwSetKeyCallback, key_callback)
}

#undef SET_CB
