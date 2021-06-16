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

        VkPhysicalDevice ChooseBestDevice(std::vector<VkPhysicalDevice> & pds);

        [[nodiscard]] VkFormatProperties GetFormatProperties(const VkFormat format) const;
        [[nodiscard]] VkFormat getSupportedDepthFormat(bool checkSamplingSupport) const;

#if 0
        const VkPhysicalDevice& GetVK() const
        {
            return m_PhysicalDevice;
        };
#endif
        [[nodiscard]] uint32_t GetMemoryIndex(uint32_t memoryTypeBits,
                                              VkMemoryPropertyFlags search) const;
        VkFormat GetSupportedFormat(std::vector<VkFormat> & candidates,
                                      VkImageTiling tiling,
                                      VkFormatFeatureFlagBits flags) const;

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

        [[nodiscard]] VkDeviceSize GetUniformBufferAlignment(VkDeviceSize size) const
        {
            if (size % limits_.minUniformBufferOffsetAlignment == 0) {
                return size;
            }
            return ((size / limits_.minUniformBufferOffsetAlignment) + 1) *
                limits_.minUniformBufferOffsetAlignment;
        }

        [[nodiscard]] VkDeviceSize GetStorageBufferAlignment(VkDeviceSize size) const
        {
            if (size % limits_.minStorageBufferOffsetAlignment== 0) {
                return size;
            }
            return ((size / limits_.minStorageBufferOffsetAlignment) + 1) *
                limits_.minStorageBufferOffsetAlignment;
        }

        const VkPhysicalDevice & GetHandle() const
        {
            return handle_;
        }
    public:
        VkPhysicalDeviceFeatures features_;
    protected:
        void findAvailableQueueFamilies();

        VkSampleCountFlagBits msaaSamples_ = VK_SAMPLE_COUNT_1_BIT;

        VkPhysicalDeviceProperties properties_;

        VkPhysicalDeviceLimits limits_;
        VkPhysicalDeviceMemoryProperties memoryProperties_;

        std::optional<uint32_t> graphicsFamily_;
        std::optional<uint32_t> computeFamily_;
        std::optional<uint32_t> transferFamily_;

        VkPhysicalDevice handle_;
        const Device * Context;
    };
}
