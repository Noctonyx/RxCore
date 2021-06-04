//
// Created by shane on 4/06/2021.
//

#include <RxCore.h>
#include "Api.h"
#include "Buffer.hpp"

namespace RxCore {
    void RxDevice::getMemBudget(std::vector<RxApi::MemHeapStatus> &heaps) const
    {
        device->getMemBudget(heaps);
    }

    RxDevice::RxDevice(RxCore::Device *device) : device(device)
    {}

    RxApi::StorageBufferPtr RxDevice::createStorageBuffer(RxApi::BufferLocation location, size_t size)
    {
        auto buf = device->createBuffer(vk::BufferUsageFlagBits::eStorageBuffer | vk::BufferUsageFlagBits::eShaderDeviceAddress | vk::BufferUsageFlagBits::eTransferDst,
                             location == RxApi::BufferLocation::GPU ?   VMA_MEMORY_USAGE_GPU_ONLY : VMA_MEMORY_USAGE_CPU_TO_GPU, size);

        return std::make_shared<RxStorageBuffer>(buf);
    }

    void RxStorageBuffer::map()
    {
        buffer->map();
    }

    void RxStorageBuffer::unmap()
    {
        buffer->unmap();
    }

    void RxStorageBuffer::update(const void *data, size_t size) const
    {

    }

    void RxStorageBuffer::update(const void *data, size_t offset, size_t size) const
    {

    }
}