//
// Created by shane on 8/02/2021.
//

#ifndef RX_ALLOCATION_H
#define RX_ALLOCATION_H

#include "Vulk.hpp"

namespace RxCore
{
    class Allocation
    {
    public :
        Allocation(
            VmaAllocator allocator,
            VmaAllocation allocation);

        ~Allocation();

        void map()
        {
            if (!mappedPtr_) {
                vmaMapMemory(allocator_, allocation_, &mappedPtr_);
            }
        }

        void unmap()
        {
            if (mappedPtr_) {
                vmaUnmapMemory(allocator_, allocation_);
                mappedPtr_ = nullptr;
            }
        }

        void update(const void * data, vk::DeviceSize size) const
        {
            assert(mappedPtr_);
            (void) memcpy(mappedPtr_, data, size);
        }

        void update(const void * data, vk::DeviceSize offset, vk::DeviceSize size) const
        {
            assert(mappedPtr_);
            (void) memcpy(static_cast<uint8_t *>(mappedPtr_) + offset, data, size);
        }

        std::byte * getPtr()
        {
            return reinterpret_cast<std::byte *>( mappedPtr_);
        }

    private:
        VmaAllocation allocation_ = nullptr;
        VmaAllocator allocator_ = nullptr;
        void * mappedPtr_ = nullptr;
    };
}
#endif //RX_ALLOCATION_H
