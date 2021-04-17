//
// Copyright (c) 2020 - Shane Hyde (shane@noctonyx.com)
//

#pragma once

#include <vector>
#include <optional>
#include <unordered_map>
#include <memory>
#include "Vulk.hpp"
#include "DeviceObject.h"

namespace RxCore
{
    class Buffer;
    class Image;
    class ImageView;
    class DescriptorPool;
    class Device;

    struct CombinedSampler {
        vk::Sampler sampler;
        std::shared_ptr<ImageView> imageView;
    };

    class DescriptorSet : public DeviceObject
    {
    public:
        DescriptorSet(vk::Device device,
                      std::shared_ptr<DescriptorPool> descriptorPool,
                      vk::DescriptorSet newHandle);

        RX_NO_COPY_NO_MOVE(DescriptorSet);

        ~DescriptorSet();

        void updateDescriptor(uint32_t binding,
                              vk::DescriptorType type,
                              std::shared_ptr<Buffer> buffer,
                              const uint32_t range = 0,
                              const uint32_t offset = 0);

        void updateDescriptor(uint32_t binding,
                              vk::DescriptorType type,
                              const std::shared_ptr<Image> & image,
                              vk::Sampler sampler);

        void updateDescriptor(
            uint32_t binding,
            vk::DescriptorType type,
            const std::vector<CombinedSampler> &
                samplerViews);

        void updateDescriptor(uint32_t binding,
                              vk::DescriptorType type,
                              std::shared_ptr<ImageView> imageView,
                              vk::ImageLayout layout,
                              vk::Sampler sampler);

        void setDescriptorOffset(uint32_t binding, const uint32_t offset) {
            offsets_[binding] = offset;
        }

        const vk::DescriptorSet handle;

        void freeResources();

        std::vector<uint32_t> getOffsets() const;

    private:
        std::shared_ptr<DescriptorPool> descriptorPool_;
        std::unordered_map<uint32_t, std::shared_ptr<Buffer>> buffers_;
        std::unordered_map<uint32_t, std::vector<std::shared_ptr<ImageView>>> imageViews_;
        std::vector<std::optional<uint32_t>> offsets_;
    };
} // namespace RXCore
