//
// Created by shane on 28/12/2020.
//

#include "Queue.hpp"
#include "Device.h"
#include "CommandBuffer.hpp"

namespace RxCore
{
    Queue::Queue(const vk::Queue queue, uint32_t family) : handle(queue), family_(family)
    {}

    Queue::~Queue()
    {
        for (auto & [fence, data]: resources_) {
            Device::VkDevice().destroyFence(fence);
        }

        while (!resources_.empty()) {
            resources_.pop_front();
        }
    }

    void Queue::submitAndWait(std::shared_ptr<PrimaryCommandBuffer> & buffer) const
    {
        std::vector<vk::CommandBuffer> buffer_handles = {buffer->Handle()};

        vk::SubmitInfo si{nullptr, nullptr, buffer_handles};
        auto fence = Device::VkDevice().createFence({});
        handle.submit(si, fence);

        const auto result = Device::VkDevice().waitForFences(1, &fence, true, MAXUINT64);
        assert(result == vk::Result::eSuccess);

        Device::VkDevice().destroyFence(fence);
    }

    void Queue::Submit(std::vector<std::shared_ptr<PrimaryCommandBuffer>> buffs,
                       std::vector<vk::Semaphore> waitSems,
                       std::vector<vk::PipelineStageFlags> waitStages,
                       std::vector<vk::Semaphore> signalSemaphores)
    {
        auto fence = Device::VkDevice().createFence({});

        std::vector<vk::CommandBuffer> buffer_handles(buffs.size());

        std::ranges::transform(buffs, buffer_handles.begin(),
                               [](std::shared_ptr<PrimaryCommandBuffer> & b) {
                                   return b->Handle();
                               });

        assert(waitStages.size() == waitSems.size());

        vk::SubmitInfo si{waitSems, waitStages, buffer_handles, signalSemaphores};
        resources_.emplace_back(fence, std::move(buffs));
        handle.submit(si, fence);
    }

    void Queue::ReleaseCompleted()
    {
        while (!resources_.empty()) {
            auto & [fence, data] = resources_.front();

            auto fs = Device::VkDevice().getFenceStatus(fence);
            if (fs == vk::Result::eSuccess) {
                resources_.pop_front();
                Device::VkDevice().destroyFence(fence);
            } else {
                break;
            }
        }
    }

    const vk::Queue Queue::GetHandle() const
    {
        return handle;
    }
} // namespace RXCore
