#pragma once

#include "Vulk.hpp"
#include "Util.h"
#include "Device.h"

namespace RxCore
{
    class Shader
    {
    public:
        explicit Shader(const Device * device, const vk::ShaderModule & handle)
            : device_(device)
              , handle_(handle)
        {}

        RX_NO_COPY_NO_MOVE(Shader);

        ~Shader()
        {
            device_->getDevice().destroyShaderModule(handle_);
        }

        [[nodiscard]] vk::ShaderModule Handle() const
        {
            return handle_;
        }

    private:
        const Device * device_;
        vk::ShaderModule handle_;
    };
}
