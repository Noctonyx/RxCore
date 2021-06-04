//
// Created by shane on 14/02/2021.
//

#ifndef RX_THREADRESOURCES_H
#define RX_THREADRESOURCES_H

#include <memory>
#include "Vulkan/Vulk.hpp"
//#include "Vulkan/Device.h"
#include "Vulkan/DescriptorPool.hpp"
#include "Pooler.h"
#include "Hasher.h"

namespace RxCore
{
    class SecondaryCommandBuffer;
    class DescriptorSet;
    //class DescriptorPool;
    class CommandPool;
    //struct DescriptorPoolGroup;
    //struct DescriptorPoolTemplate;

    struct ThreadResources
    {
        std::shared_ptr<SecondaryCommandBuffer> getCommandBuffer();

        std::shared_ptr<DescriptorSet> getDescriptorSet(
            const DescriptorPoolTemplate & poolTemplate,
            vk::DescriptorSetLayout layout);

        std::shared_ptr<DescriptorSet> getDescriptorSet(
            const DescriptorPoolTemplate & poolTemplate,
            vk::DescriptorSetLayout layout,
            const std::vector<uint32_t> & counts);

        //std::shared_ptr<RxUtil::Pooler<RxCore::DescriptorSet>> getSetPoooler(uint32_t id);
        //void setSetPooler(uint32_t id, std::shared_ptr<RxUtil::Pooler<RxCore::DescriptorSet>> setPooler);

        //void registerDescriptorPool(uint32_t id, const std::vector<vk::DescriptorPoolSize> & poolSizes, uint32_t max);
        //void unregisterDescriptorPool(uint32_t id);

        void freeAllResources();
        void freeUnused();

        std::shared_ptr<CommandPool> pool;
        std::deque<std::shared_ptr<SecondaryCommandBuffer>> buffers;
        //std::unordered_map<uint32_t, std::shared_ptr<RXUtil::Pooler<RXCore::DescriptorSet>>> setPoolers;
        //std::unordered_map<uint32_t, std::shared_ptr<DescriptorPool>> descriptorSetPools;
        std::unordered_map<RxUtil::Hash, DescriptorPoolGroup> descriptorSetPoolGroups;
        std::string threadId;
    };

    extern thread_local ThreadResources threadResources;

    inline void freeThreadResources()
    {
        threadResources.freeUnused();
    }

    inline void freeAllThreadResource()
    {
        threadResources.freeAllResources();
    }
}
#endif //RX_THREADRESOURCES_H
