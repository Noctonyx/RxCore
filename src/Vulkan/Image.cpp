#include "Image.hpp"
#include "vulkan/vulkan.hpp"

namespace RxCore
{
    Image::~Image()
    {
        device_.destroyImage(handle_);
        allocation_.reset();
    }

    std::shared_ptr<ImageView> Image::createImageView(
        vk::ImageViewType viewType,
        vk::ImageAspectFlagBits aspect,
        uint32_t baseArrayLayer,
        uint32_t layerCount)
    {
        vk::ImageViewCreateInfo ivci;

        ivci.setViewType(viewType).setFormat(format_).setImage(handle_).setSubresourceRange(
            {aspect, 0, VK_REMAINING_MIP_LEVELS, baseArrayLayer, layerCount});

        auto h = device_.createImageView(ivci);
        return std::make_shared<ImageView>(device_, h, shared_from_this());
    }

    ImageView::ImageView(vk::Device device, vk::ImageView handle, std::shared_ptr<Image> image)
        : DeviceObject(device)
        , handle(handle)
        , image_(std::move(image)) {}

    ImageView::~ImageView()
    {
        image_.reset();
        device_.destroyImageView(handle);
    }

    std::shared_ptr<Image> ImageView::getImage() const
    {
        return image_;
    }
} // namespace RXCore
