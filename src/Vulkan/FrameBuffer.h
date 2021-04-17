#pragma once

#include "Vulk.hpp"
#include "Device.h"

namespace RxCore
{
    class FrameBuffer
    {
    public:
        explicit FrameBuffer(const vk::Framebuffer & handle)
            : handle_(handle) {}

        FrameBuffer(const FrameBuffer & other) = delete;

        FrameBuffer(FrameBuffer && other) noexcept = delete;

        FrameBuffer & operator=(const FrameBuffer & other) = delete;

        FrameBuffer & operator=(FrameBuffer && other) noexcept = delete;

        ~FrameBuffer() { Device::VkDevice().destroyFramebuffer(handle_); }

        [[nodiscard]] vk::Framebuffer Handle() const { return handle_; }

    private:
        vk::Framebuffer handle_;
    };
}
