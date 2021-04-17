#pragma once

#include "Vulk.hpp"
#include "Util.h"
#include "Device.h"

namespace RxCore
{
    class Shader
    {
    public:
        explicit Shader(const vk::ShaderModule & handle)
            : handle_(handle) {}

        RX_NO_COPY_NO_MOVE(Shader);

        ~Shader()
        {
            Device::VkDevice().destroyShaderModule(handle_);
        }

        [[nodiscard]] vk::ShaderModule Handle() const
        {
            return handle_;
        }

    private:
        vk::ShaderModule handle_;
    };
}
