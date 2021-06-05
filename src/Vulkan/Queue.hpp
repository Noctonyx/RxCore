//
// Created by shane on 28/12/2020.
//

#ifndef RX_QUEUE_HPP
#define RX_QUEUE_HPP

#include <deque>
#include "Vulk.hpp"

namespace RxCore
{
    class PrimaryCommandBuffer;
    class Device;
    class CommandBuffer;

    class Queue
    {
    public:
        Queue(Device * device, const vk::Queue queue, uint32_t queueFamily);
        ~Queue();

        void ReleaseCompleted();
        void submitAndWait(std::shared_ptr<PrimaryCommandBuffer> & buffer) const;
        void Submit(std::vector<std::shared_ptr<PrimaryCommandBuffer>> buffs,
                    std::vector<vk::Semaphore> waitSems,
                    std::vector<vk::PipelineStageFlags> waitStages,
                    std::vector<vk::Semaphore> signalSemaphores);

        uint32_t family() const
        {
            return family_;
        }

    private:
        Device * device_;
        std::deque<std::tuple<vk::Fence, std::vector<std::shared_ptr<PrimaryCommandBuffer>>>>
            resources_;
        // vk::Fence fence_;
        const vk::Queue handle;
        uint32_t family_;

    public:
        const vk::Queue GetHandle() const;
    };
} // namespace RXCore
#endif // RX_QUEUE_HPP
