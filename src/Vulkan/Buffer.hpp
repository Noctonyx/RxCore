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
        //Buffer(vk::Device device, vk::Buffer handle, vk::DeviceSize size);
        Buffer(vk::Device device,
               vk::Buffer handle,
               std::shared_ptr<Allocation> allocation,
               vk::DeviceSize size);
#if 0
        Buffer(
            const vk::BufferUsageFlags & flags,
            const vk::MemoryPropertyFlags & memFlags,
            vk::DeviceSize size,
            const void * data
        );
#endif
        virtual ~Buffer();

        RX_NO_COPY_NO_MOVE(Buffer);

        vk::Buffer handle() const
        {
            return handle_;
        }
        // [[nodiscard]] vk::MemoryRequirements getMemoryRequirements() const;
//        void allocateMemory(const vk::MemoryPropertyFlags & memFlags, const void * data =
//        nullptr);
#if 0
        void bindMemory(std::shared_ptr<Allocation> memory, vk::DeviceSize offset = 0)
        {
            vmaBindBufferMemory(a)
            VulkanContext::VkDevice().bindBufferMemory(handle, memory->handle, offset);
            allocation_ = std::move(memory);
        }
#endif
        [[nodiscard]] const std::shared_ptr<Allocation> & getMemory() const;
        [[nodiscard]] vk::DeviceSize getSize() const;

    private:
        const vk::Buffer handle_ = nullptr;
        vk::DeviceSize size_ = 0;
        std::shared_ptr<Allocation> allocation_;
    };
} // namespace RXCore
