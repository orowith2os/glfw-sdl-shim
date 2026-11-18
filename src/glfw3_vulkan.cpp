// Needs to be up here so glfw3.h sees it
#include <vulkan/vulkan.h>

#include "glfw3.h"
#include "glfw_helpers.hpp"

#include <SDL3/SDL_vulkan.h>

#include <print>

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
  if (loader)
    std::println ("Ignoring custom Vulkan loader argument. This isn't compatible, for now at least.");

  if (state.vulkan_loaded)
    return;

  if (SDL_Vulkan_LoadLibrary (nullptr))
      state.vulkan_loaded = true;
  else
      do_error_things (GLFW_API_UNAVAILABLE, SDL_GetError ());
}
