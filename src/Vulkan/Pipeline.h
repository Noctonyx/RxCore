#pragma once

#include "Vulk.hpp"
#include "Util.h"
#include "Device.h"

namespace RxCore
{
    class Pipeline
    {
    public:
        explicit Pipeline(Device * device, const vk::Pipeline & handle)
            : device_(device)
            , handle_(handle)
        {}

        RX_NO_COPY_NO_MOVE(Pipeline);

        ~Pipeline()
        {
            device_->getDevice().destroyPipeline(handle_);
        }

        [[nodiscard]] vk::Pipeline Handle() const
        {
            return handle_;
        }

    private:
        Device * device_;
        vk::Pipeline handle_;
    };
}
