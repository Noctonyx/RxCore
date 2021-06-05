//
// Created by shane on 29/12/2020.
//

#include "CommandPool.hpp"
#include "Device.h"
#include "CommandBuffer.hpp"
//#include "Renderer.hpp"

namespace RxCore
{
    CommandPool::CommandPool(Device * device, vk::CommandPool handle, uint32_t qf)
        : device_(device)
        , handle(handle)
        //, queuefamily_(qf)
    {}

    CommandPool::~CommandPool()
    {
        device_->getDevice().destroyCommandPool(handle);
    }

    std::shared_ptr<PrimaryCommandBuffer> CommandPool::GetPrimaryCommandBuffer()
    {
        vk::CommandBufferAllocateInfo cbai;
        cbai.setCommandPool(handle).setCommandBufferCount(1);

        cbai.setLevel(vk::CommandBufferLevel::ePrimary);

        auto res = device_->getDevice().allocateCommandBuffers(cbai);
        return std::make_shared<PrimaryCommandBuffer>(device_, res[0], shared_from_this());
    }

    std::shared_ptr<SecondaryCommandBuffer> CommandPool::GetSecondaryCommandBuffer()
    {
        vk::CommandBufferAllocateInfo cbai;
        cbai.setCommandPool(handle).setCommandBufferCount(1);

        cbai.setLevel(vk::CommandBufferLevel::eSecondary);

        auto res = device_->getDevice().allocateCommandBuffers(cbai);
        return std::make_shared<SecondaryCommandBuffer>(device_, res[0], shared_from_this());
    }

    std::shared_ptr<TransferCommandBuffer> CommandPool::createTransferCommandBuffer()
    {
        vk::CommandBufferAllocateInfo cbai;
        cbai.setCommandPool(handle).setCommandBufferCount(1);

        cbai.setLevel(vk::CommandBufferLevel::ePrimary);

        auto res = device_->getDevice().allocateCommandBuffers(cbai);
        return std::make_shared<TransferCommandBuffer>(device_, res[0], shared_from_this());
    }

} // namespace RXCore
