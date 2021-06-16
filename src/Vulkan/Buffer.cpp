#include "Buffer.hpp"

namespace RxCore
{
    Buffer::Buffer(
        Device * device,
        VkBuffer handle,
        std::shared_ptr<Allocation> allocation,
        VkDeviceSize size)
        : device_(device)
          , handle_(handle)
          , size_(size)
          , allocation_(std::move(allocation))
    {}

    Buffer::~Buffer()
    {
        vkDestroyBuffer(device_->getDevice(), handle_, nullptr);
        allocation_.reset();
    }

    VkDeviceSize Buffer::getSize() const
    {
        return size_;
    }
}
