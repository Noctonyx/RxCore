//#define VMA_IMPLEMENTATION
//#include "vk_mem_alloc.hpp"

#include "Instance.hpp"
#include <GLFW/glfw3.h>
#include <vector>
#include <memory>
#include "Log.h"
#include "spdlog/spdlog.h"

VULKAN_HPP_DEFAULT_DISPATCH_LOADER_DYNAMIC_STORAGE

namespace RxCore
{
    Instance::Instance(Device * context)
        : Context(context)
    {
        CreateInstance();
    }

    Instance::~Instance()
    {
        Handle.destroyDebugUtilsMessengerEXT(debugMessenger_);
    }

    void Instance::CreateInstance()
    {
        vk::DynamicLoader dl;
        PFN_vkGetInstanceProcAddr vkGetInstanceProcAddr =
            dl.getProcAddress<PFN_vkGetInstanceProcAddr>("vkGetInstanceProcAddr");

        VULKAN_HPP_DEFAULT_DISPATCHER.init(vkGetInstanceProcAddr);

        vk::ApplicationInfo ai{
            "RX",
            VK_MAKE_VERSION(0, 0, 1),
            "RXCore",
            VK_MAKE_VERSION(0, 0, 1),
            VK_API_VERSION_1_2
        };
        enabledExtensions_ = GetInstanceExtensions();
        //m_EnabledLayers.push_back("VK_LAYER_KHRONOS_validation");
        const std::vector<const char *> list_layers =
            {
                "VK_LAYER_KHRONOS_validation"
            };

        Handle = vk::createInstance(
            {
                {},
                &ai,
                static_cast<uint32_t>(list_layers.size()),
                list_layers.data(),
                static_cast<uint32_t>(enabledExtensions_.size()),
                enabledExtensions_.data()
            }
        );
        VULKAN_HPP_DEFAULT_DISPATCHER.init(Handle);
        EnableDebugMessages();
    }

    void Instance::EnableDebugMessages()
    {
        vk::DebugUtilsMessengerCreateInfoEXT ci;
        ci.messageSeverity = vk::DebugUtilsMessageSeverityFlagBitsEXT::eVerbose |
                             vk::DebugUtilsMessageSeverityFlagBitsEXT::eWarning |
                             vk::DebugUtilsMessageSeverityFlagBitsEXT::eError;

        ci.messageType = vk::DebugUtilsMessageTypeFlagBitsEXT::eGeneral |
                         vk::DebugUtilsMessageTypeFlagBitsEXT::eValidation |
                         vk::DebugUtilsMessageTypeFlagBitsEXT::ePerformance;

        ci.pfnUserCallback = DebugCallback;
        ci.pUserData = nullptr;

        auto result = GetHandle().createDebugUtilsMessengerEXT(&ci, nullptr, &debugMessenger_);
        assert(result == vk::Result::eSuccess);
        if(result != vk::Result::eSuccess) {
            throw std::exception("Unable to create debug util messenger");
        }

    }

    std::vector<const char *> Instance::GetInstanceExtensions()
    {
        uint32_t count = 0;
        const char ** extensions = glfwGetRequiredInstanceExtensions(&count);
        std::vector<const char *> list(extensions, extensions + count);

        list.push_back(VK_EXT_DEBUG_UTILS_EXTENSION_NAME);

        return instanceExtensions_;
       // return list;
    }

    VkBool32 Instance::DebugCallback(
        VkDebugUtilsMessageSeverityFlagBitsEXT severity,
        VkDebugUtilsMessageTypeFlagsEXT /*messageType*/,
        const VkDebugUtilsMessengerCallbackDataEXT * callbackData,
        void * /*userData*/
    )
    {
        switch (severity) {
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_WARNING_BIT_EXT:
                spdlog::warn(callbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_ERROR_BIT_EXT:
                spdlog::critical(callbackData->pMessage);
//                throw std::exception(callbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_INFO_BIT_EXT:
                spdlog::info(callbackData->pMessage);
                break;
            case VK_DEBUG_UTILS_MESSAGE_SEVERITY_VERBOSE_BIT_EXT:
            default:
                spdlog::debug(callbackData->pMessage);
                break;
        }
        return VK_FALSE;
    }
}
