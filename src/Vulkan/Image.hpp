#pragma once

#include "Vulk.hpp"
#include "Util.h"

namespace RxCore
{
    class ImageView;
    class Allocation;

    class Image : public std::enable_shared_from_this<Image>
    {
        friend class Device;

    public:
        explicit Image(
            Device * device,
            vk::Image image,
            vk::Format format,
            std::shared_ptr<Allocation> allocation,
            vk::Extent3D extent)
            : device_(device)
            , extent_(extent)
            , handle_(image)
            , currentLayout_(vk::ImageLayout::eUndefined)
            , format_(format)
            , allocation_(std::move(allocation)) {};

        ~Image();

        RX_NO_COPY_NO_MOVE(Image);

        std::shared_ptr<ImageView> createImageView(
            vk::ImageViewType viewType,
            vk::ImageAspectFlagBits aspect,
            uint32_t baseArrayLayer = 0,
            uint32_t layerCount = VK_REMAINING_ARRAY_LAYERS);

        vk::Image handle() const
        {
            return handle_;
        }

    public:
        vk::Extent3D extent_;

    private:
        Device * device_;
        vk::Image handle_;
        vk::ImageLayout currentLayout_;
        vk::Format format_ = vk::Format::eUndefined;
        vk::ImageType imageType_ = vk::ImageType::e2D;
        std::shared_ptr<Allocation> allocation_;
    };

    class ImageView
    {
    public:
        ImageView(Device * device, vk::ImageView handle, std::shared_ptr<Image> image);
        ~ImageView();

        RX_NO_COPY_NO_MOVE(ImageView);

        vk::ImageView handle;
        std::shared_ptr<Image> getImage() const;

    private:
        Device * device_;
        std::shared_ptr<Image> image_;
    };
} // namespace RXCore
