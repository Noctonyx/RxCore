////////////////////////////////////////////////////////////////////////////////
// MIT License
//
// Copyright (c) 2021.  Shane Hyde
//
// Permission is hereby granted, free of charge, to any person obtaining a copy
// of this software and associated documentation files (the "Software"), to deal
// in the Software without restriction, including without limitation the rights
// to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
// copies of the Software, and to permit persons to whom the Software is
// furnished to do so, subject to the following conditions:
//
// The above copyright notice and this permission notice shall be included in all
// copies or substantial portions of the Software.
//
// THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
// IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
// FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
// AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
// LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
// OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
// SOFTWARE.
//
////////////////////////////////////////////////////////////////////////////////

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
        Queue(Device * device, vk::Queue queue, uint32_t queueFamily);
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
