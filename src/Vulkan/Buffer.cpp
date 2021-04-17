#include "Buffer.hpp"
#include "PhysicalDevice.hpp"

namespace RxCore
{
    Buffer::Buffer(
        vk::Device device,
        vk::Buffer handle,
        std::shared_ptr<Allocation> allocation,
        vk::DeviceSize size)
        : DeviceObject(device)
        , handle_(handle)
        , size_(size)
        , allocation_(std::move(allocation)) {}

    Buffer::~Buffer()
    {
        device_.destroyBuffer(handle_);
        allocation_.reset();
    }

    vk::DeviceSize Buffer::getSize() const
    {
        return size_;
    }

    const std::shared_ptr<Allocation> & Buffer::getMemory() const
    {
        return allocation_;
    }
} // namespace RXCore
