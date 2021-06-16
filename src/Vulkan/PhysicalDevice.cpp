#include "PhysicalDevice.hpp"
#include "Log.h"
#include "spdlog/spdlog.h"

namespace RxCore
{
#if 0
    PhysicalDevice::PhysicalDevice(Device * context)
        : Context(context)
    {
        auto & instance = context->instance->GetHandle();

        auto pds = instance.enumeratePhysicalDevices();
        if (pds.empty()) {
            throw std::exception("No Vulkan Devices");
        }
        handle_ = ChooseBestDevice(pds);

        auto di = handle_.getProperties();

        spdlog::info("Physical Device: {0}", di.deviceName);

        properties_ = handle_.getProperties();
        features_ = handle_.getFeatures();
        memoryProperties_ = handle_.getMemoryProperties();
        findAvailableQueueFamilies();
        limits_ = properties_.limits;

        if (!graphicsFamily_.has_value()) {
            spdlog::critical("No Graphics Queue Support");
        }
    }

    PhysicalDevice::~PhysicalDevice() = default;

    VkFormatProperties PhysicalDevice::GetFormatProperties(const VkFormat format) const
    {
        auto p = handle_.getFormatProperties(format);
        return p;
    }

    VkFormat PhysicalDevice::getSupportedDepthFormat(bool checkSamplingSupport) const
    {
        std::vector<VkFormat> formats = {
            VkFormat::eD24UnormS8Uint, VkFormat::eD32SfloatS8Uint, VkFormat::eD32Sfloat,
            VkFormat::eD16UnormS8Uint, VkFormat::eD16Unorm,
        };

        for (auto & format: formats) {
            VkFormatProperties format_properties = handle_.getFormatProperties(format);

            if (format_properties.optimalTilingFeatures &
                VkFormatFeatureFlagBits::eDepthStencilAttachment) {
                if (checkSamplingSupport) {
                    if (!(format_properties.optimalTilingFeatures &
                          VkFormatFeatureFlagBits::eSampledImage)) {
                        continue;
                    }
                }
                return format;
            }
        }
        spdlog::critical("Failed to find suitable depth format ");
        return VkFormat::eUndefined;
    }

    uint32_t PhysicalDevice::GetMemoryIndex(
        uint32_t memoryTypeBits,
        VkMemoryPropertyFlags search) const
    {
        for (uint32_t i = 0; i != memoryProperties_.memoryTypeCount; ++i, memoryTypeBits >>= 1) {
            if (memoryTypeBits & 1) {
                if ((memoryProperties_.memoryTypes[i].propertyFlags & search) == search) {
                    return i;
                }
            }
        }
        spdlog::error("Failed to find suitable memory {}", memoryTypeBits);

        return ~0U;
    }

    VkFormat PhysicalDevice::GetSupportedFormat(
        std::vector<VkFormat> & candidates,
        VkImageTiling tiling,
        VkFormatFeatureFlagBits flags) const
    {
        for (auto & candidate: candidates) {
            // VkFormatProperties Props;
            auto p = handle_.getFormatProperties(candidate);
            if (tiling == VkImageTiling::eOptimal && (p.optimalTilingFeatures & flags) == flags) {
                return candidate;
            }
            if (tiling == VkImageTiling::eLinear && (p.linearTilingFeatures & flags) == flags) {
                return candidate;
            }
        }
        spdlog::error("Failed to find supported format! Returning VK_FORMAT_D32_SFLOAT by default");
        return VkFormat::eD32Sfloat;
    }

    VkPhysicalDevice PhysicalDevice::ChooseBestDevice(std::vector<VkPhysicalDevice> & pds)
    {
        // return pds[0];

        auto ScoreDevice = [](VkPhysicalDevice dev)
        {
            int32_t score = 0;

            auto prop = dev.getProperties();
            if (prop.deviceType == VkPhysicalDeviceType::eDiscreteGpu) {
                score += 20;
            }
            // check supports presentation
            // check queue families
            // prop.
            auto e = dev.enumerateDeviceExtensionProperties();

            return score;
        };
        std::vector<std::pair<VkPhysicalDevice, int32_t>> scores;

        for (auto & pd: pds) {
            scores.emplace_back(pd, ScoreDevice(pd));
        }
        std::sort(
            scores.begin(), scores.end(),
            [&](
                const std::pair<VkPhysicalDevice, int32_t> & a,
                const std::pair<VkPhysicalDevice, int32_t> & b)
            {
                return a.second > b.second;
            });

        return scores[0].first;
    }

    void PhysicalDevice::findAvailableQueueFamilies()
    {
        auto qfps = handle_.getQueueFamilyProperties();

        for (uint32_t i = 0; i < qfps.size(); i++) {
            auto f = qfps[i].queueFlags;
            if (f & VkQueueFlagBits::eGraphics && !(f & VkQueueFlagBits::eCompute) &&
                !(f & VkQueueFlagBits::eTransfer)) {
                graphicsFamily_ = i;
            }
        }
        for (uint32_t i = 0; i < qfps.size(); i++) {
            auto f = qfps[i].queueFlags;
            if (f & VkQueueFlagBits::eCompute && !(f & VkQueueFlagBits::eGraphics) &&
                !(f & VkQueueFlagBits::eTransfer)) {
                computeFamily_ = i;
            }
        }

        for (uint32_t i = 0; i < qfps.size(); i++) {
            auto f = qfps[i].queueFlags;
            if (f & VkQueueFlagBits::eTransfer && !(f & VkQueueFlagBits::eCompute) &&
                !(f & VkQueueFlagBits::eGraphics)) {
                transferFamily_ = i;
            }
        }

        if (!graphicsFamily_.has_value()) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                auto f = qfps[i].queueFlags;
                if (f & VkQueueFlagBits::eGraphics) {
                    graphicsFamily_ = i;
                }
            }
        }

        if (!computeFamily_.has_value()) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                auto f = qfps[i].queueFlags;
                if (f & VkQueueFlagBits::eCompute && !(f & VkQueueFlagBits::eGraphics)) {
                    computeFamily_ = i;
                }
            }
        }

        if (!computeFamily_.has_value()) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                auto f = qfps[i].queueFlags;
                if (f & VkQueueFlagBits::eCompute) {
                    computeFamily_ = i;
                }
            }
        }

        if (!transferFamily_.has_value()) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                auto f = qfps[i].queueFlags;
                if (f & VkQueueFlagBits::eTransfer && !(f & VkQueueFlagBits::eGraphics)) {
                    transferFamily_ = i;
                }
            }
        }

        if (!transferFamily_.has_value()) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                auto f = qfps[i].queueFlags;
                if (f & VkQueueFlagBits::eTransfer) {
                    transferFamily_ = i;
                }
            }
        }

        assert(graphicsFamily_.has_value());
        assert(computeFamily_.has_value());
        assert(transferFamily_.has_value());
    }
#endif
} // namespace RXCore
