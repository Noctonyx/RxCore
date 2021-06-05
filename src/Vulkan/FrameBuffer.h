#pragma once

#include "Vulk.hpp"
#include "Device.h"

namespace RxCore
{
    class FrameBuffer
    {
    public:
        FrameBuffer(Device * device, const vk::Framebuffer & handle)
            : device_(device)
              , handle_(handle)
        {}

        FrameBuffer(const FrameBuffer & other) = delete;

        FrameBuffer(FrameBuffer && other) noexcept = delete;

        FrameBuffer & operator=(const FrameBuffer & other) = delete;

        FrameBuffer & operator=(FrameBuffer && other) noexcept = delete;

        ~FrameBuffer()
        {
            device_->getDevice().destroyFramebuffer(handle_);
        }

        [[nodiscard]] vk::Framebuffer Handle() const
        { return handle_; }

    private:
        Device * device_;
        vk::Framebuffer handle_;
    };
}
