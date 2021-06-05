#pragma once

#include <optional>
#include "Vulk.hpp"
#include "Instance.hpp"
#include "Device.h"
//#include "VulkanResource.hpp"

namespace RxCore
{
    class PhysicalDevice
    {
        friend class Device;
        //friend class Renderer;

    public:
        explicit PhysicalDevice(Device * context);
        ~PhysicalDevice();

        PhysicalDevice(const PhysicalDevice & pd) = delete;
        PhysicalDevice(const PhysicalDevice && pd) = delete;
        PhysicalDevice & operator=(const PhysicalDevice & other) = delete;

        vk::PhysicalDevice ChooseBestDevice(std::vector<vk::PhysicalDevice> & pds);

        [[nodiscard]] vk::FormatProperties GetFormatProperties(const vk::Format format) const;
        [[nodiscard]] vk::Format getSupportedDepthFormat(bool checkSamplingSupport) const;

#if 0
        const vk::PhysicalDevice& GetVK() const
        {
            return m_PhysicalDevice;
        };
#endif
        [[nodiscard]] uint32_t GetMemoryIndex(uint32_t memoryTypeBits,
                                              vk::MemoryPropertyFlags search) const;
        vk::Format GetSupportedFormat(std::vector<vk::Format> & candidates,
                                      vk::ImageTiling tiling,
                                      vk::FormatFeatureFlagBits flags) const;

        [[nodiscard]] uint32_t GetGraphicsQueueFamily() const
        {
            return graphicsFamily_.value();
        }

        [[nodiscard]] uint32_t GetComputeQueueFamily() const
        {
            return computeFamily_.value();
        }

        [[nodiscard]] uint32_t GetTransferQueueFamily() const
        {
            return transferFamily_.value();
        }

        [[nodiscard]] vk::DeviceSize GetUniformBufferAlignment(vk::DeviceSize size) const
        {
            if (size % limits_.minUniformBufferOffsetAlignment == 0) {
                return size;
            }
            return ((size / limits_.minUniformBufferOffsetAlignment) + 1) *
                limits_.minUniformBufferOffsetAlignment;
        }

        [[nodiscard]] vk::DeviceSize GetStorageBufferAlignment(vk::DeviceSize size) const
        {
            if (size % limits_.minStorageBufferOffsetAlignment== 0) {
                return size;
            }
            return ((size / limits_.minStorageBufferOffsetAlignment) + 1) *
                limits_.minStorageBufferOffsetAlignment;
        }

        const vk::PhysicalDevice & GetHandle() const
        {
            return handle_;
        }
    public:
        vk::PhysicalDeviceFeatures features_;
    protected:
        void findAvailableQueueFamilies();

        vk::SampleCountFlagBits msaaSamples_ = vk::SampleCountFlagBits::e1;

        vk::PhysicalDeviceProperties properties_;

        vk::PhysicalDeviceLimits limits_;
        vk::PhysicalDeviceMemoryProperties memoryProperties_;

        std::optional<uint32_t> graphicsFamily_;
        std::optional<uint32_t> computeFamily_;
        std::optional<uint32_t> transferFamily_;

        vk::PhysicalDevice handle_;
        const Device * Context;
    };
}
