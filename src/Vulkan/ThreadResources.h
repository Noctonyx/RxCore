//
// Created by shane on 14/02/2021.
//

#pragma once

#include <memory>
#include "Vulkan/Vulk.hpp"
#include "Vulkan/DescriptorPool.hpp"
#include "Pooler.h"

namespace RxCore
{
    class SecondaryCommandBuffer;
    class CommandPool;

    struct ThreadResources
    {
        std::shared_ptr<SecondaryCommandBuffer> getCommandBuffer();

        void freeAllResources();
        void freeUnused();

        std::shared_ptr<CommandPool> pool;
        std::deque<std::shared_ptr<SecondaryCommandBuffer>> buffers;
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
