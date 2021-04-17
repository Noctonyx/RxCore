//
// Copyright (c) 2020 - Shane Hyde (shane@noctonyx.com)
//

#include "DescriptorSet.hpp"
#include <utility>
#include <algorithm>
#include <optional>
#include "Buffer.hpp"
#include "Image.hpp"
#include "DescriptorPool.hpp"

namespace RxCore
{
    DescriptorSet::DescriptorSet(
        vk::Device device,
        std::shared_ptr<DescriptorPool> descriptorPool,
        vk::DescriptorSet newHandle
    )
    //: VulkanResource<DescriptorSet, vk::DescriptorSet>(context)
        : DeviceObject(device)
        , handle(newHandle)
        , descriptorPool_(std::move(descriptorPool)) {}

    DescriptorSet::~DescriptorSet()
    {
        buffers_.clear();
        descriptorPool_->handBackDescriptorSet(handle);
    }

    void DescriptorSet::updateDescriptor(
        uint32_t binding,
        vk::DescriptorType type,
        std::shared_ptr<Buffer> buffer,
        const uint32_t range,
        const uint32_t offset
    )
    {
        offsets_.resize(std::max(binding + 1, static_cast<uint32_t>(offsets_.size())));

        vk::DescriptorBufferInfo dbi{buffer->handle(), 0, range == 0 ? buffer->size_ : range};

        std::vector<vk::WriteDescriptorSet> wds{
            {handle, binding, {}, 1, type, {}, &dbi},
        };
        buffers_.insert_or_assign(binding, std::move(buffer));
        //buffers_.push_back(buffer);

        if (type == vk::DescriptorType::eUniformBufferDynamic || type == vk::DescriptorType::eStorageBufferDynamic) {
            offsets_[binding] = offset;
        } else {
            offsets_[binding] = std::nullopt;
        }
        Device::VkDevice().updateDescriptorSets(
            static_cast<uint32_t>(wds.size()),
            wds.data(),
            0,
            nullptr
        );
    }

    // TODO: fix this
    void DescriptorSet::updateDescriptor(
        uint32_t binding,
        vk::DescriptorType type,
        const std::shared_ptr<Image> & image,
        vk::Sampler sampler
    )
    {
        offsets_.resize(std::max(binding + 1, static_cast<uint32_t>(offsets_.size())));
        auto image_view = image->createImageView(
            vk::ImageViewType::e2D,
            vk::ImageAspectFlagBits::eColor, 0, 1);

        if (imageViews_.contains(binding)) {
            imageViews_.erase(binding);
        }
        imageViews_.emplace(binding, std::vector<std::shared_ptr<ImageView>>{image_view});
//        if (imageViews_.size() < binding + 1) {
        //          imageViews_.resize(binding + 1);
//        }
        //      imageViews_[binding] = image_view;
#if 0
        if (samplers_.size() < binding + 1) {
            samplers_.resize(binding + 1);
        }
        samplers_[binding] = sampler;
#endif
//        if (images_.size() < binding + 1) {
        //          images_.resize(binding + 1);
        //    }
//        images_[binding] = image;

        vk::DescriptorImageInfo dii = {
            sampler, image_view->handle, vk::ImageLayout::eShaderReadOnlyOptimal
        };
        vk::WriteDescriptorSet wds;
        wds.setDescriptorType(type)
           .setDescriptorCount(1)
           .setDstArrayElement(0)
           .setDstSet(handle)
           .setDstBinding(binding)
           .setPImageInfo(&dii);

        device_.updateDescriptorSets({wds}, {});
    }

    void DescriptorSet::updateDescriptor(
        uint32_t binding,
        vk::DescriptorType type,
        std::shared_ptr<ImageView> imageView,
        vk::ImageLayout layout,
        vk::Sampler sampler)
    {
        offsets_.resize(std::max(binding + 1, static_cast<uint32_t>(offsets_.size())));
        vk::DescriptorImageInfo dii = {
            sampler, imageView->handle, layout
        };
        if (imageViews_.contains(binding)) {
            imageViews_.erase(binding);
        }
        imageViews_.emplace(binding, std::vector<std::shared_ptr<ImageView>>{imageView});
//        if (images_.size() < binding + 1) {
        //          images_.resize(binding + 1);
        //    }
        //  images_[binding] = std::move(imageView->getImage());

        //if (imageViews_.size() < binding + 1) {
//            imageViews_.resize(binding + 1);
//        }
        //      imageViews_[binding] = std::move(imageView);
#if 0
        if (samplers_.size() < binding + 1) {
            samplers_.resize(binding + 1);
        }
        samplers_[binding] = std::move(sampler);
#endif
        vk::WriteDescriptorSet wds;
        wds.setDescriptorType(type)
           .setDescriptorCount(1)
           .setDstArrayElement(0)
           .setDstSet(handle)
           .setDstBinding(binding)
           .setPImageInfo(&dii);
        device_.updateDescriptorSets({wds}, {});
    }
#if 0
    std::shared_ptr<ImageView> DescriptorSet::getBoundImageView(uint32_t binding) const
    {
        if (imageViews_.size() <= binding) {
            return nullptr;
        }
        return imageViews_[binding];
    }
#endif
    void DescriptorSet::freeResources()
    {
        buffers_.clear();
        //images_.clear();
        //samplers_.clear();
        imageViews_.clear();
    }

    void DescriptorSet::updateDescriptor(
        uint32_t binding,
        vk::DescriptorType type,
        const std::vector<CombinedSampler> & samplerViews)
    {
//        vk::DescriptorImageInfo dii = {
        //          sampler->handle, imageView->handle, layout
        //    };
        if (samplerViews.size() == 0) {
            return;
        }
        offsets_.resize(std::max(binding + 1, static_cast<uint32_t>(offsets_.size())));

        std::vector<vk::DescriptorImageInfo> dii = {};
        for (const auto & sampler_view : samplerViews) {
            dii.emplace_back(
                sampler_view.sampler,
                sampler_view.imageView->handle,
                vk::ImageLayout::eShaderReadOnlyOptimal);
        }
        std::vector<std::shared_ptr<ImageView>> iviews(samplerViews.size());
        std::transform(
            samplerViews.cbegin(), samplerViews.cend(),
            iviews.begin(),
            [](const CombinedSampler & i) -> std::shared_ptr<ImageView> { return i.imageView; });

        if (imageViews_.contains(binding)) {
            imageViews_.erase(binding);
        }
        imageViews_.emplace(binding, iviews);
        //std::vector<vk::DescriptorImageInfo> dii;

        vk::WriteDescriptorSet wds;
        wds.setDescriptorType(type)
           .setDescriptorCount(static_cast<uint32_t>( dii.size()))
           .setDstArrayElement(0)
           .setDstSet(handle)
           .setDstBinding(binding)
           .setImageInfo(dii);

        device_.updateDescriptorSets({wds}, {});
    }

    std::vector<uint32_t> DescriptorSet::getOffsets() const
    {
        std::vector<uint32_t> v;

        for (auto & x: offsets_) {
            if (x.has_value()) {
                v.push_back(x.value());
            }
        }
        return v;
    }
}
