//
// Created by shane on 1/01/2021.
//

#include "JobManager.hpp"
#include "Vulkan/Device.h"
#include "Vulkan/CommandPool.hpp"

namespace RxCore
{
    std::atomic<uint16_t> JobManager::threadStartedCount_ = 0;
#if 0
    std::shared_ptr<SecondaryCommandBuffer> JobManager::getCommandBuffer(const RenderStage & stage)
    {
        if (pool == nullptr) {
            pool = Device::Context()->CreateGraphicsCommandPool();
        }
        // freeUnusedBuffers();
        auto b = pool->GetSecondaryCommandBuffer(stage);
        buffers.push_back(b);
        return b;
    }
#endif
    JobManager::JobManager()
    {
        threadCount_ = static_cast<uint16_t>(std::thread::hardware_concurrency() - 1);
        spdlog::info("Starting {} worker threads", threadCount_);
        queue_ = std::make_unique<JobQueue>();
        cleans.resize(threadCount_);

        threadStartedCount_ = 0;
        for (uint16_t i = 0; i < threadCount_; ++i) {
            spdlog::debug("Starting thread {}", i);
            auto th = std::thread(&JobManager::JobTask, this, i);
            th.detach();
        }
    }
#if 0
    void JobManager::freeUnusedBuffers()
    {
        OPTICK_EVENT("Check for buffers to free")
        while (!buffers.empty() && buffers.front().use_count() == 1) {
            OPTICK_EVENT("Free buffer")
            buffers.pop_front();
        }
        for(auto & x: setPoolers) {
            x.second->freeUnused();
        }
    }
#endif
#if 0
    std::shared_ptr<RXUtil::Pooler<RXCore::DescriptorSet>> JobManager::getSetPoooler(uint32_t id)
    {
        if(!setPoolers.contains(id)) {
            return nullptr;
        }
        return setPoolers[id];
    }

    void JobManager::setSetPooler(uint32_t id, std::shared_ptr<RXUtil::Pooler<RXCore::DescriptorSet>> setPooler)
    {
        setPoolers.insert_or_assign(id, setPooler);
    }
#endif
    void JobBase::schedule(bool background)
    {
        JobManager::instance().Schedule(shared_from_this(), background);
    }

    void JobBase::schedule(std::shared_ptr<JobBase> parentJob, bool background)
    {
        JobManager::instance().Schedule(shared_from_this(), parentJob, background);
    }
}
