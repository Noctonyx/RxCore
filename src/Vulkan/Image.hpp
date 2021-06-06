////////////////////////////////////////////////////////////////////////////////
// MIT License
//
// Copyright (c) 2021-2021.  Shane Hyde (shane@noctonyx.com)
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////

#pragma once

#include "Vulk.hpp"
#include "Util.h"

namespace RxCore
{
    class ImageView;
    class Allocation;

    class Image //: public std::enable_shared_from_this<Image>
    {
        friend class Device;

    public:
        Image(
            Device * device,
            vk::Image image,
            vk::Format format,
            std::shared_ptr<Allocation> allocation,
            vk::Extent3D extent)
            : extent_(extent)
            , device_(device)
            , handle_(image)
            , currentLayout_(vk::ImageLayout::eUndefined)
            , format_(format)
            , allocation_(std::move(allocation)) {};

        ~Image();

        RX_NO_COPY_NO_MOVE(Image);
#if 0
        std::shared_ptr<ImageView> createImageView(
            vk::ImageViewType viewType,
            vk::ImageAspectFlagBits aspect,
            uint32_t baseArrayLayer = 0,
            uint32_t layerCount = VK_REMAINING_ARRAY_LAYERS);
#endif
        vk::Image handle() const
        {
            return handle_;
        }

        //vk::Format getFormat() const { return format_;}

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
        ImageView(const Device * device, vk::ImageView handle);
        ~ImageView();

        RX_NO_COPY_NO_MOVE(ImageView);

        vk::ImageView handle_;
        //std::shared_ptr<Image> getImage() const;

    private:
        const Device * device_;
        //std::shared_ptr<Image> image_;
    };
} // namespace RXCore
