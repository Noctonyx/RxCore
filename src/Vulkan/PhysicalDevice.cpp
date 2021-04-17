#include "PhysicalDevice.hpp"
#include "Log.h"
#include "spdlog/spdlog.h"

namespace RxCore
{
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

    vk::FormatProperties PhysicalDevice::GetFormatProperties(const vk::Format format) const
    {
        auto p = handle_.getFormatProperties(format);
        return p;
    }

    vk::Format PhysicalDevice::getSupportedDepthFormat(bool checkSamplingSupport) const
    {
        std::vector<vk::Format> formats = {
            vk::Format::eD24UnormS8Uint, vk::Format::eD32SfloatS8Uint, vk::Format::eD32Sfloat,
            vk::Format::eD16UnormS8Uint, vk::Format::eD16Unorm,
        };

        for (auto & format: formats) {
            vk::FormatProperties format_properties = handle_.getFormatProperties(format);

            if (format_properties.optimalTilingFeatures &
                vk::FormatFeatureFlagBits::eDepthStencilAttachment) {
                if (checkSamplingSupport) {
                    if (!(format_properties.optimalTilingFeatures &
                          vk::FormatFeatureFlagBits::eSampledImage)) {
                        continue;
                    }
                }
                return format;
            }
        }
        spdlog::critical("Failed to find suitable depth format ");
        return vk::Format::eUndefined;
    }

    uint32_t PhysicalDevice::GetMemoryIndex(
        uint32_t memoryTypeBits,
        vk::MemoryPropertyFlags search) const
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

    vk::Format PhysicalDevice::GetSupportedFormat(
        std::vector<vk::Format> & candidates,
        vk::ImageTiling tiling,
        vk::FormatFeatureFlagBits flags) const
    {
        for (auto & candidate: candidates) {
            // VkFormatProperties Props;
            auto p = handle_.getFormatProperties(candidate);
            if (tiling == vk::ImageTiling::eOptimal && (p.optimalTilingFeatures & flags) == flags) {
                return candidate;
            }
            if (tiling == vk::ImageTiling::eLinear && (p.linearTilingFeatures & flags) == flags) {
                return candidate;
            }
        }
        spdlog::error("Failed to find supported format! Returning VK_FORMAT_D32_SFLOAT by default");
        return vk::Format::eD32Sfloat;
    }

    vk::PhysicalDevice PhysicalDevice::ChooseBestDevice(std::vector<vk::PhysicalDevice> & pds)
    {
        // return pds[0];

        auto ScoreDevice = [](vk::PhysicalDevice dev)
        {
            int32_t score = 0;

            auto prop = dev.getProperties();
            if (prop.deviceType == vk::PhysicalDeviceType::eDiscreteGpu) {
                score += 20;
            }
            // check supports presentation
            // check queue families
            // prop.
            auto e = dev.enumerateDeviceExtensionProperties();

            return score;
        };
        std::vector<std::pair<vk::PhysicalDevice, int32_t>> scores;

        for (auto & pd: pds) {
            scores.emplace_back(pd, ScoreDevice(pd));
        }
        std::sort(
            scores.begin(), scores.end(),
            [&](
                const std::pair<vk::PhysicalDevice, int32_t> & a,
                const std::pair<vk::PhysicalDevice, int32_t> & b)
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
            if (f & vk::QueueFlagBits::eGraphics && !(f & vk::QueueFlagBits::eCompute) &&
                !(f & vk::QueueFlagBits::eTransfer)) {
                graphicsFamily_ = i;
            }
        }
        for (uint32_t i = 0; i < qfps.size(); i++) {
            auto f = qfps[i].queueFlags;
            if (f & vk::QueueFlagBits::eCompute && !(f & vk::QueueFlagBits::eGraphics) &&
                !(f & vk::QueueFlagBits::eTransfer)) {
                computeFamily_ = i;
            }
        }

        for (uint32_t i = 0; i < qfps.size(); i++) {
            auto f = qfps[i].queueFlags;
            if (f & vk::QueueFlagBits::eTransfer && !(f & vk::QueueFlagBits::eCompute) &&
                !(f & vk::QueueFlagBits::eGraphics)) {
                transferFamily_ = i;
            }
        }

        if (!graphicsFamily_.has_value()) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                auto f = qfps[i].queueFlags;
                if (f & vk::QueueFlagBits::eGraphics) {
                    graphicsFamily_ = i;
                }
            }
        }

        if (!computeFamily_.has_value()) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                auto f = qfps[i].queueFlags;
                if (f & vk::QueueFlagBits::eCompute && !(f & vk::QueueFlagBits::eGraphics)) {
                    computeFamily_ = i;
                }
            }
        }

        if (!computeFamily_.has_value()) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                auto f = qfps[i].queueFlags;
                if (f & vk::QueueFlagBits::eCompute) {
                    computeFamily_ = i;
                }
            }
        }

        if (!transferFamily_.has_value()) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                auto f = qfps[i].queueFlags;
                if (f & vk::QueueFlagBits::eTransfer && !(f & vk::QueueFlagBits::eGraphics)) {
                    transferFamily_ = i;
                }
            }
        }

        if (!transferFamily_.has_value()) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                auto f = qfps[i].queueFlags;
                if (f & vk::QueueFlagBits::eTransfer) {
                    transferFamily_ = i;
                }
            }
        }

        assert(graphicsFamily_.has_value());
        assert(computeFamily_.has_value());
        assert(transferFamily_.has_value());
    }
} // namespace RXCore
