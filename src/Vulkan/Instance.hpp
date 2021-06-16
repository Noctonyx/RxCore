#pragma once

#include "Vulk.hpp"
#include <vector>
#include <memory>
#include <string>

#include "SDL_video.h"

namespace RxCore
{
    class Device;

    class Instance
    {
    public:
        explicit Instance(Device * context, SDL_Window* window);

        ~Instance();

        void CreateInstance(SDL_Window* window);

        void EnableDebugMessages();
        //void DisableDebugMessages();

        std::vector<const char *> GetInstanceExtensions(SDL_Window * window);

        const VkInstance & GetHandle() const
        {
            return Handle;
        }

    protected:
        static VKAPI_ATTR VkBool32 VKAPI_CALL DebugCallback(
            VkDebugUtilsMessageSeverityFlagBitsEXT severity,
            VkDebugUtilsMessageTypeFlagsEXT messageType,
            const VkDebugUtilsMessengerCallbackDataEXT * callbackData,
            void * userData
        );
#if 0
        VkResult CreateDebugUtilsMessengerEXT(
            const VkInstance& instance,
            const VkDebugUtilsMessengerCreateInfoEXT* pCreateInfo,
            const VkAllocationCallbacks* pAllocator,
            VkDebugUtilsMessengerEXT* pDebugMessenger
        );

        void DestroyDebugUtilsMessengerEXT(
            const VkInstance& instance,
            VkDebugUtilsMessengerEXT debugMessenger,
            const VkAllocationCallbacks* pAllocator
        );
#endif
        //VkInstance m_Instance;
        //VkDebugUtilsMessengerEXT debugMessenger_ = 0;
        VkDebugUtilsMessengerEXT debugMessenger_;
        std::vector<const char *> enabledExtensions_;
        //std::vector<const char*> m_EnabledLayers;

        VkInstance Handle;
        const Device* Context;

        std::vector<const char *> instanceExtensions_ =
        {
            VK_KHR_SURFACE_EXTENSION_NAME,
            VK_KHR_WIN32_SURFACE_EXTENSION_NAME,
            VK_KHR_GET_PHYSICAL_DEVICE_PROPERTIES_2_EXTENSION_NAME,
            VK_EXT_DEBUG_UTILS_EXTENSION_NAME,
        };

        /** Device extension support for the swap chain */
    };
}
