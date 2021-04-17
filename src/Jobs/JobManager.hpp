//
// Created by shane on 1/01/2021.
//

#pragma once

#include <iostream>
#include <fstream>
#include <cstdint>
#include <thread>
#include <vector>
#include <atomic>
#include <queue>
#include <functional>
#include <cassert>
#include <Pooler.h>
#include <Vulkan/DescriptorSet.hpp>
//#include "Vulkan/RenderData.h"
#include "optick/optick.h"
#include "Log.h"
#include "ThreadResources.h"

namespace RxCore
{
    class CommandPool;
    class SecondaryCommandBuffer;

    struct JobBase : public std::enable_shared_from_this<JobBase>
    {
        std::atomic_uint16_t childCount;
        std::shared_ptr<JobBase> parent;
        //std::shared_ptr<JobBase> followedJob;
        std::vector<std::shared_ptr<JobBase>> followOns;

        virtual ~JobBase()
        {
            followOns.clear();
            //followedJob.reset();
            parent.reset();
        };

        JobBase(const JobBase & other) = delete;

        JobBase(JobBase && other) noexcept = delete;

        JobBase & operator=(const JobBase & other) = delete;

        JobBase & operator=(JobBase && other) noexcept = delete;

        JobBase()
        {
            childCount.store(1);
        }

        void waitComplete() const
        {
            OPTICK_CATEGORY("Job Wait", Optick::Category::Wait)

            while (childCount.load() > 0) {
                YieldProcessor();
            }
        }

        bool isCompleted() const
        {
            return childCount.load() == 0;
        }

        virtual void execute() = 0;

        void addFollowOnJob(std::shared_ptr<JobBase> j)
        {
            //j->followedJob = shared_from_this();
            followOns.push_back(std::move(j));
        }

        void schedule(bool background = false);
        void schedule(std::shared_ptr<JobBase> parentJob, bool background = false);
    };

    template<typename T>
    struct Job final : JobBase
    {
        std::function<T(void)> f;

        T result;

        explicit Job(std::function<T(void)> function)
            : f(std::move(function)) {}

        void execute() override
        {
            OPTICK_EVENT();
            result = f();
        }
    };

    template<>
    struct Job<void> final : JobBase
    {
        std::function<void(void)> f;

        explicit Job(std::function<void(void)> function)
            : f(std::move(function)) {}

        void execute() override
        {
            OPTICK_EVENT()
            f();
        }
    };

    template<typename T, typename F>
    std::shared_ptr<Job<T>> CreateJob(F f)
    {
        return std::make_shared<Job<T>>(f);
    }

    struct JobQueue
    {
        std::mutex mutex;
        std::condition_variable condVar;

        std::queue<std::shared_ptr<JobBase>> jobs;
        std::queue<std::shared_ptr<JobBase>> backgroundJobs;

        void push(std::shared_ptr<JobBase> job, bool background)
        {
            std::unique_lock<std::mutex> lk(mutex);
            if (background) {
                backgroundJobs.push(job);
            } else {
                jobs.push(job);
            }
            lk.unlock();
            condVar.notify_one();
        }

        std::shared_ptr<JobBase> pop()
        {
            std::unique_lock<std::mutex> lk(mutex);
            if (!jobs.empty()) {
                auto j = jobs.front();
                jobs.pop();
                return j;
            }
            if (!backgroundJobs.empty()) {
                auto j = backgroundJobs.front();
                backgroundJobs.pop();
                return j;
            }
            condVar.wait(lk);

            if (!jobs.empty()) {
                auto j = jobs.front();
                jobs.pop();
                return j;
            }
            if (!backgroundJobs.empty()) {
                auto j = backgroundJobs.front();
                backgroundJobs.pop();
                return j;
            }
            return nullptr;
        }
    };

    class JobManager
    {
    public:
        static inline thread_local uint16_t threadId;
        static inline thread_local std::shared_ptr<JobBase> currentJob = nullptr;
        static inline thread_local ThreadResources resources;

        std::atomic<uint16_t> threadCount_ = 0;
        std::atomic<bool> terminated_;
        static std::atomic<uint16_t> threadStartedCount_;
        std::unique_ptr<JobQueue> queue_;
        std::atomic<bool> shutdown_;
        std::vector<bool> cleans;

        JobManager(const JobManager &) = delete;

        JobManager & operator=(const JobManager &) = delete;

        static JobManager & instance()
        {
            static JobManager instance;
            return instance;
        }

        static ThreadResources & threadData() {
            return resources;
        }

        //static std::shared_ptr<SecondaryCommandBuffer> getCommandBuffer(const RenderStage & stage);
        //static std::shared_ptr<RXUtil::Pooler<RXCore::DescriptorSet>> getSetPoooler(uint32_t id);
        //static void setSetPooler(uint32_t id, std::shared_ptr<RXUtil::Pooler<RXCore::DescriptorSet>> setPooler);

        //static void freeUnusedBuffers();

        JobManager();

        template<typename T, typename F>
        static auto CreateJob(F f)
        {
            return std::make_shared<Job<T>>(f);
        }

        template<typename T, typename F>
        static auto Schedule(F f)
        {
            return instance().Schedule(std::make_shared<Job<T>>(f));
        }

        void JobTask(const uint16_t index)
        {
            OPTICK_THREAD("Worker Thread");

            threadId = index;
            ++threadStartedCount_;
            while (threadStartedCount_.load() < threadCount_.load()) {}

            //std::cout << "Thread Ready " << index << std::endl;
            while (!shutdown_.load()) {
                auto job = queue_->pop();
                if (cleans[index]) {
                    resources.freeUnused();
                    cleans[index] = false;
                }
                if (!job) {
                    continue;
                }
                {
                    OPTICK_EVENT("Process job")
                    {
                        OPTICK_EVENT("Getting Work ready")
                        if (shutdown_) {
                            break;
                        }
                        currentJob = std::move(job);
                    }
                    currentJob->execute();
                    {
                        OPTICK_EVENT("Cleaning Up")

                        --currentJob->childCount;

                        auto p = currentJob->parent;
                        while (p) {
                            --p->childCount;
                            p = p->parent;
                        }
                        if (!shutdown_.load()) {
                            for (auto & f: currentJob->followOns) {
                                //uint16_t n = rand() % threadCount_;
                                queue_->push(f, false);
                            }
                        }
                        currentJob = nullptr;
                    }
                }
            }
            //if (pool) {
                spdlog::debug("Resetting pool");
                //pool.reset();
                resources.freeAllResources();
            //}
            //buffers.clear();
            const uint32_t num = threadCount_.fetch_sub(1);
            if (num == 1) {
                spdlog::info("Last thread cleaning up");
                queue_.reset();
                terminated_.store(true);
                terminated_.notify_all();
            }
        }

        void Shutdown()
        {
            shutdown_.store(true);
            spdlog::info("Shutting down threads");
            queue_->condVar.notify_all();
            spdlog::info("All notified");
            terminated_.wait(false);
            //spdlog::info("About to reset pool");
            //pool.reset();
            resources.freeAllResources();
            spdlog::info("Main thread Pool Reset");
        }

        template<typename T>
        T Schedule(T job, bool background = false)
        {
            if (currentJob) {
                return Schedule(job, currentJob);
            }
            queue_->push(job, background);
            return job;
        }

        template<typename T, typename U>
        T Schedule(T job, U parent, bool background = false)
        {
            job->parent = std::move(parent);
            //++job->childCount;
            auto p = job->parent;
            while (p) {
                ++p->childCount;
                p = p->parent;
            }
            queue_->push(job, background);
            return job;
        }

        void clean()
        {
            for (uint32_t i = 0; i < threadCount_; i++) {
                cleans[i] = true;
            }
            queue_->condVar.notify_all();
        }
    };
}