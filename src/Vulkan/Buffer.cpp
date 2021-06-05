#include "Buffer.hpp"
#include "PhysicalDevice.hpp"

namespace RxCore
{
    Buffer::Buffer(
        Device * device,
        vk::Buffer handle,
        std::shared_ptr<Allocation> allocation,
        vk::DeviceSize size)
        : device_(device)
          , handle_(handle)
          , size_(size)
          , allocation_(std::move(allocation))
    {}

    Buffer::~Buffer()
    {
        device_->getDevice().destroyBuffer(handle_);
        allocation_.reset();
    }

    vk::DeviceSize Buffer::getSize() const
    {
        return size_;
    }
}
