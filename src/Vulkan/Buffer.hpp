#pragma once

#include "Allocation.h"
#include "Util.h"
#include "Device.h"

namespace RxCore
{
    class Device;

    class Buffer
    {
        friend class DescriptorSet;

    public:
        Buffer(Device * device,
               vk::Buffer handle,
               std::shared_ptr<Allocation> allocation,
               vk::DeviceSize size);

        virtual ~Buffer();

        RX_NO_COPY_NO_MOVE(Buffer)

        [[nodiscard]] vk::Buffer handle() const
        {
            return handle_;
        }

        //[[nodiscard]] const std::shared_ptr<Allocation> & getMemory() const;
        [[nodiscard]] vk::DeviceSize getSize() const;

        void map()
        {
            allocation_->map();
        }

        void unmap()
        {
            allocation_->unmap();
        }

        void update(const void* data, vk::DeviceSize size) const
        {
            allocation_->update(data, size);
        }

        void update(const void* data, vk::DeviceSize offset, vk::DeviceSize size) const
        {
            allocation_->update(data, offset, size);
        }

        void * getPtr()
        {
            return allocation_->getPtr();
        }

        vk::DeviceAddress getDeviceAddress() const
        {
            vk::BufferDeviceAddressInfo bdai{};
            bdai.setBuffer(handle_);

            return device_->getDevice().getBufferAddress(bdai);
        }
        
    private:
        Device * device_;
        const vk::Buffer handle_ = nullptr;
        vk::DeviceSize size_ = 0;
        std::shared_ptr<Allocation> allocation_;
    };
}
