//
// Created by shane on 14/02/2021.
//

#include "ThreadResources.h"
#include "Vulkan/Device.h"
#include "Vulkan/CommandPool.hpp"
#include "Vulkan/DescriptorPool.hpp"
#include "Vulkan/DescriptorSet.hpp"
#include "optick/optick.h"
#include "Log.h"

namespace RxCore
{
    thread_local ThreadResources threadResources;

    void ThreadResources::freeAllResources()
    {
        descriptorSetPoolGroups.clear();
        pool.reset();
        buffers.clear();
    }

    std::shared_ptr<SecondaryCommandBuffer> ThreadResources::getCommandBuffer()
    {
        if (pool == nullptr) {
            pool = RxCore::Device::Context()->CreateGraphicsCommandPool();
        }
        // freeUnusedBuffers();
        auto b = pool->GetSecondaryCommandBuffer();
        buffers.push_back(b);
        return b;
    }

    void ThreadResources::freeUnused()
    {
        OPTICK_EVENT("Check for buffers to free")
        while (!buffers.empty() && buffers.front().use_count() == 1) {
            OPTICK_EVENT("Free buffer")
            buffers.pop_front();
        }
        /*
        for (auto & x: setPoolers) {
            x.second->freeUnused();
        }
         */
    }
#if 0
    std::shared_ptr<RXUtil::Pooler<RXCore::DescriptorSet>> ThreadResources::getSetPoooler(uint32_t id)
    {
        if (!setPoolers.contains(id)) {
            return nullptr;
        }
        return setPoolers[id];
    }

    void ThreadResources::setSetPooler(uint32_t id, std::shared_ptr<RXUtil::Pooler<RXCore::DescriptorSet>> setPooler)
    {
        setPoolers.insert_or_assign(id, setPooler);
    }
#endif
    std::shared_ptr<DescriptorSet> ThreadResources::getDescriptorSet(
        const DescriptorPoolTemplate & poolTemplate,
        vk::DescriptorSetLayout layout)
    {
        OPTICK_EVENT()
        if (!descriptorSetPoolGroups.contains(poolTemplate.hash)) {
            std::stringstream ss;
            ss << std::this_thread::get_id();

            spdlog::debug("Thread {} allocating a descriptorsetpoolgroup ", ss.str());
            descriptorSetPoolGroups.insert_or_assign(poolTemplate.hash, DescriptorPoolGroup(poolTemplate));
        }
        auto & pool_group = descriptorSetPoolGroups.at(poolTemplate.hash);
        return pool_group.getDescriptorSet(layout);
    }

    std::shared_ptr<DescriptorSet> ThreadResources::getDescriptorSet(
        const DescriptorPoolTemplate & poolTemplate,
        vk::DescriptorSetLayout layout,
        const std::vector<uint32_t> & counts)
    {
        if (!descriptorSetPoolGroups.contains(poolTemplate.hash)) {
            std::stringstream ss;
            ss << std::this_thread::get_id();

            spdlog::debug("Thread {} allocating a descriptorsetpoolgroup ", ss.str());
            descriptorSetPoolGroups.insert_or_assign(poolTemplate.hash, DescriptorPoolGroup(poolTemplate));
        }
        auto & pool_group = descriptorSetPoolGroups.at(poolTemplate.hash);
        return pool_group.getDescriptorSet(layout, counts);
    }

#if 0
    void ThreadResources::registerDescriptorPool(
        uint32_t id,
        const std::vector<vk::DescriptorPoolSize> & poolSizes,
        uint32_t max)
    {
        descriptorSetPoolGroups.emplace(id, DescriptorPoolGroup(poolSizes, max));
    }

    void ThreadResources::unregisterDescriptorPool(uint32_t id)
    {
        descriptorSetPoolGroups.erase(id);
    }
#endif

}