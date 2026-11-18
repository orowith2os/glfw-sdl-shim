// Needs to be up here so glfw3.h sees it
#include <vulkan/vulkan.h>

#include "glfw3.h"
#include "glfw_helpers.hpp"
#include "glfw_window.hpp"

#include <SDL3/SDL_vulkan.h>

#include <print>

const char **
glfwGetRequiredInstanceExtensions (uint32_t* count)
{
  if (!state.vulkan_loaded)
    do_error_things (GLFW_NOT_INITIALIZED, "Vulkan wasn't loaded.");

  auto exts = SDL_Vulkan_GetInstanceExtensions (count);

  if (!exts)
    do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());

  return (const char**)exts;
}

void
glfwInitVulkanLoader (PFN_vkGetInstanceProcAddr loader)
{
  // TODO
  if (loader)
    std::println ("Ignoring custom Vulkan loader argument. This isn't compatible, for now at least.");

  if (state.vulkan_loaded)
    return;

  if (SDL_Vulkan_LoadLibrary (nullptr))
      state.vulkan_loaded = true;
  else
      do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
}

int
glfwVulkanSupported (void)
{
  if (state.vulkan_loaded)
    return GLFW_TRUE;

  if (SDL_Vulkan_LoadLibrary (nullptr))
    {
      SDL_Vulkan_UnloadLibrary ();
      return GLFW_TRUE;
    }
  else
    {
      do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
      return GLFW_FALSE;
    }
}

GLFWvkproc
glfwGetInstanceProcAddress (VkInstance  instance,
                            const char *procname)
{
  if (!glfwVulkanSupported())
    {
      do_error_things (GLFW_API_UNAVAILABLE, "Vulkan doesn't appear to be supported.");
      return nullptr;
    }

  glfwInitVulkanLoader (nullptr);

  auto vk_proc_addr = (PFN_vkGetInstanceProcAddr) SDL_Vulkan_GetVkGetInstanceProcAddr ();
  if (!vk_proc_addr)
    {
      do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
      return nullptr;
    }

  return (GLFWvkproc) vk_proc_addr (instance, procname);
}

int
glfwGetPhysicalDevicePresentationSupport (VkInstance instance,
                                          VkPhysicalDevice device,
                                          uint32_t queuefamily)
{
  if (!SDL_Vulkan_GetPresentationSupport (instance, device, queuefamily))
    {
      do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
      return GLFW_FALSE;
    }
  else
    return GLFW_TRUE;
}

VkResult
glfwCreateWindowSurface (VkInstance instance,
                         GLFWwindow *window,
                         const VkAllocationCallbacks *allocator,
                         VkSurfaceKHR *surface)
{
  if (!SDL_Vulkan_CreateSurface(window->sdl_window,
                                instance,
                                allocator,
                                surface))
    {
      do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
      return VK_ERROR_UNKNOWN;
    }
  else
    return VK_SUCCESS;
}
