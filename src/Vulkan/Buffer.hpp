#pragma once

#include "DeviceObject.h"
#include "Allocation.h"
#include "Util.h"

namespace RxCore
{
    class Buffer : public DeviceObject
    {
        friend class DescriptorSet;

    public:
        Buffer(vk::Device device,
               vk::Buffer handle,
               std::shared_ptr<Allocation> allocation,
               vk::DeviceSize size);

        virtual ~Buffer();

        RX_NO_COPY_NO_MOVE(Buffer)

        [[nodiscard]] vk::Buffer handle() const
        {
            return handle_;
        }

        [[nodiscard]] const std::shared_ptr<Allocation> & getMemory() const;
        [[nodiscard]] vk::DeviceSize getSize() const;

    private:
        const vk::Buffer handle_ = nullptr;
        vk::DeviceSize size_ = 0;
        std::shared_ptr<Allocation> allocation_;
    };
}
