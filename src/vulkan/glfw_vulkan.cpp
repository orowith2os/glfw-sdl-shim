// Needs to be up here so glfw3.h sees it
#include <vulkan/vulkan.h>

#include "glfw3.h"
#include "helpers/glfw_helpers.hpp"
#include "window/glfw_window.hpp"

#include <SDL3/SDL_vulkan.h>

#include <print>

#define ENSURE_VULKAN_INIT()\
  glfwInit ();\
  glfwInitVulkanLoader (nullptr);\

const char **
glfwGetRequiredInstanceExtensions (uint32_t* count)
{
  ENSURE_VULKAN_INIT ()
  const char **result = nullptr;

  if (!state.vulkan_loaded)
    do_error_things (GLFW_NOT_INITIALIZED, "Vulkan wasn't loaded.");

  run_on_main_sync ([&]()
    {
      auto exts = SDL_Vulkan_GetInstanceExtensions (count);

      if (!exts)
        do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());

      result = (const char**) exts;
    });

  return result;
}

void
glfwInitVulkanLoader (PFN_vkGetInstanceProcAddr loader)
{
  glfwInit ();

  // TODO
  if (loader)
    std::println ("Ignoring custom Vulkan loader argument. This isn't compatible, for now at least.");

  if (state.vulkan_loaded)
    return;
  run_on_main_sync([&]()
    {
      if (SDL_Vulkan_LoadLibrary (nullptr))
          state.vulkan_loaded = true;
      else
          do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
    });
}

int
glfwVulkanSupported (void)
{
  ENSURE_VULKAN_INIT ()
  int result;

  if (state.vulkan_loaded)
    return GLFW_TRUE;

  run_on_main_sync ([&]()
    {
      if (SDL_Vulkan_LoadLibrary (nullptr))
        {
          SDL_Vulkan_UnloadLibrary ();
          result = GLFW_TRUE;
        }
      else
        {
          do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
          result = GLFW_FALSE;
        }
    });

  return result;
}

GLFWvkproc
glfwGetInstanceProcAddress (VkInstance  instance,
                            const char *procname)
{
  ENSURE_VULKAN_INIT ()
  GLFWvkproc result = nullptr;

  if (!glfwVulkanSupported())
    {
      do_error_things (GLFW_API_UNAVAILABLE, "Vulkan doesn't appear to be supported.");
      return nullptr;
    }

  glfwInitVulkanLoader (nullptr);
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
          do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
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

  run_on_main_sync ([&]()
    {
      if (!SDL_Vulkan_CreateSurface(window->sdl_window,
                                    instance,
                                    allocator,
                                    surface))
        {
          do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
          result = VK_ERROR_UNKNOWN;
        }
      else
        result = VK_SUCCESS;
    });

  return result;
}
