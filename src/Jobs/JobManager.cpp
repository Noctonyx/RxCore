//
// Created by shane on 1/01/2021.
//

#include "JobManager.hpp"

namespace RxCore
{
    std::atomic<uint16_t> JobManager::threadStartedCount_ = 0;

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
}

void RxApi::JobBase::schedule(bool background)
{
    RxCore::JobManager::instance().Schedule(shared_from_this(), background);
}

void RxApi::JobBase::schedule(std::shared_ptr<JobBase> parentJob, bool background)
{
    RxCore::JobManager::instance().Schedule(shared_from_this(), parentJob, background);
}

void RxApi::JobBase::waitComplete() const
{
    OPTICK_CATEGORY("Job Wait", Optick::Category::Wait)

    while (childCount.load() > 0) {
        YieldProcessor();
    }
}
