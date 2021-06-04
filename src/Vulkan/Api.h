//
// Created by shane on 5/06/2021.
//

#ifndef INDUSTRONAUT_API_H
#define INDUSTRONAUT_API_H

#include "RxCore.h"
#include "Device.h"

namespace RxCore {
    class RxDevice : public RxApi::Device {
    public:
        RxDevice(RxCore::Device *device);

        void getMemBudget(std::vector<RxApi::MemHeapStatus> & heaps) const override;

        RxApi::StorageBufferPtr createStorageBuffer(RxApi::BufferLocation location, size_t size) override;
        RxApi::VertexBufferPtr createVertexBuffer(RxApi::BufferLocation location, size_t size) override;
        RxApi::IndexBufferPtr createIndexBuffer(RxApi::BufferLocation location, size_t size) override;
        RxApi::BufferPtr createBuffer(RxApi::BufferLocation location, size_t size) override;

    private:
        RxCore::Device * device;
    };

    class RxStorageBuffer : public RxApi::StorageBuffer {
    public:
        explicit RxStorageBuffer(const std::shared_ptr<RxCore::Buffer> &buffer);

        void map() override;
        void unmap() override;
        void update(const void *data, size_t size) const override;
        void update(const void *data, size_t offset, size_t size) const override;

    private:
        std::shared_ptr<RxCore::Buffer> buffer;
    };
}
#endif //INDUSTRONAUT_API_H
