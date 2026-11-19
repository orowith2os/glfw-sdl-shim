// Needs to be up here so glfw3.h sees it
#include <vulkan/vulkan.h>

#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"
#include "window/glfw_window.hpp"

#include <SDL3/SDL_vulkan.h>

#include <print>

#define ENSURE_VULKAN_INIT()\
  glfwInitVulkanLoader (nullptr);\

#define ASSERT_VULKAN_LOADED_PTR()\
  if (!state.vulkan_loaded)\
    {\
      do_error_things (GLFW_API_UNAVAILABLE, "Vulkan isn't available.");\
      return nullptr;\
    }\

extern "C" {

const char **
glfwGetRequiredInstanceExtensions (uint32_t* count)
{
  ASSERT_VULKAN_LOADED_PTR ()
  const char **result = nullptr;

  if (!assert_glfw_init () || !count)
    return nullptr;

  ASSERT_MAIN_THREAD_PTR()

  run_on_main_sync ([&]()
    {
      auto exts = SDL_Vulkan_GetInstanceExtensions (count);

      if (!exts)
        {
          do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
          *count = 0;
        }

      result = (const char**) exts;
    });

  return result;
}

void
glfwInitVulkanLoader (PFN_vkGetInstanceProcAddr loader)
{
  glfwInit ();

  if (state.vulkan_loaded)
    return;

  if (loader)
    // We'd need to deal with extension stuff ourselves...
    std::println ("Ignoring custom Vulkan loader argument. This isn't compatible, for now at least.");

  run_on_main_sync([&]()
    {
      if (SDL_Vulkan_LoadLibrary (nullptr))
          state.vulkan_loaded = true;
      else
          do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
    });
}

int
glfwVulkanSupported ()
{
  ENSURE_VULKAN_INIT ()
  ASSERT_GLFW_INIT_INT ()

  if (state.vulkan_loaded)
    return GLFW_TRUE;
  else
    return GLFW_FALSE;
}

GLFWvkproc
glfwGetInstanceProcAddress (VkInstance  instance,
                            const char *procname)
{
  ENSURE_VULKAN_INIT()
  ASSERT_VULKAN_LOADED_PTR()

  GLFWvkproc result = nullptr;

  run_on_main_sync ([&]()
    {
      auto vk_proc_addr = (PFN_vkGetInstanceProcAddr) SDL_Vulkan_GetVkGetInstanceProcAddr ();

      if (!vk_proc_addr)
          do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
      else
          result = (GLFWvkproc) vk_proc_addr (instance, procname);
    });

  return result;
}

int
glfwGetPhysicalDevicePresentationSupport (VkInstance instance,
                                          VkPhysicalDevice device,
                                          uint32_t queuefamily)
{
  ENSURE_VULKAN_INIT ()
  int result;

  if (!state.vulkan_loaded)
    return GLFW_FALSE;

  run_on_main_sync ([&]()
    {
      if (!SDL_Vulkan_GetPresentationSupport (instance, device, queuefamily))
        {
          // This might be a bit spammy. Can always re-enable later.
          //do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
          result = GLFW_FALSE;
        }
      else
        result = GLFW_TRUE;
    });

    return result;
}

VkResult
glfwCreateWindowSurface (VkInstance instance,
                         GLFWwindow *window,
                         const VkAllocationCallbacks *allocator,
                         VkSurfaceKHR *surface)
{
  ENSURE_VULKAN_INIT ()
  VkResult result;

  if (!state.vulkan_loaded || !window || !surface)
    return VK_ERROR_INITIALIZATION_FAILED;

  if (window->client_api != GLFW_NO_API)
    return VK_ERROR_NATIVE_WINDOW_IN_USE_KHR;

  run_on_main_sync ([&]()
    {
      if (!SDL_Vulkan_CreateSurface(window->sdl_window,
                                    instance,
                                    allocator,
                                    surface))
        {
          do_error_things (GLFW_PLATFORM_ERROR, SDL_GetError ());
          result = VK_ERROR_UNKNOWN;
        }
      else
        result = VK_SUCCESS;
    });

  return result;
}

} // extern "C"
