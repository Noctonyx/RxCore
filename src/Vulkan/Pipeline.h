#pragma once

#include "Vulk.hpp"
#include "Util.h"
#include "Device.h"

namespace RxCore
{
    class Pipeline
    {
    public:
        explicit Pipeline(const vk::Pipeline& handle)
            : handle_(handle) {}

        RX_NO_COPY_NO_MOVE(Pipeline);

        ~Pipeline()
        {
            Device::VkDevice().destroyPipeline(handle_);
        }

        [[nodiscard]] vk::Pipeline Handle() const
        {
            return handle_;
        }

    private:
        vk::Pipeline handle_;
    };
}
