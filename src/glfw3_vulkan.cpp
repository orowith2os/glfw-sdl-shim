// Needs to be up here so glfw3.h sees it
#include <vulkan/vulkan.h>

#include "glfw3.h"
#include "glfw_helpers.hpp"

#include <SDL3/SDL_vulkan.h>

#include <print>

VkResult
glfwCreateWindowSurface (VkInstance instance,
                         GLFWwindow *window,
                         const VkAllocationCallbacks *allocator,
                         VkSurfaceKHR *surface)
{
  // TODO
  return VK_ERROR_UNKNOWN;
}

int
glfwGetPhysicalDevicePresentationSupport (VkInstance instance,
                                          VkPhysicalDevice device,
                                          uint32_t queuefamily)
{
  if (!glfwVulkanSupported())
    {
      do_error_things (GLFW_API_UNAVAILABLE, "Vulkan doesn't appear to be supported.");
      return GLFW_FALSE;
    }

  glfwInitVulkanLoader (nullptr);

  if (!SDL_Vulkan_GetPresentationSupport (instance, device, queuefamily))
    {
      do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
      return GLFW_FALSE;
    }
  else
    return GLFW_TRUE;
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
