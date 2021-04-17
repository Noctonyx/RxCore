//
// Copyright (c) 2020 - Shane Hyde (shane@noctonyx.com)
//

#include "DescriptorPool.hpp"
#include "DescriptorSet.hpp"
#include "Log.h"
#include "optick/optick.h"

namespace RxCore
{
    DescriptorPool::DescriptorPool(vk::Device device, vk::DescriptorPool new_handle)
        : DeviceObject(device)
        , handle(new_handle)
    {
    }

    std::shared_ptr<DescriptorSet> DescriptorPool::allocateDescriptorSet(vk::DescriptorSetLayout layout)
    {
        OPTICK_EVENT()
        std::vector<vk::DescriptorSetLayout> vv(1, VkDescriptorSetLayout(layout));

        vk::DescriptorSetAllocateInfo dsai{
            handle,
            1,
            vv.data()
        };

        auto ds = Device::VkDevice().allocateDescriptorSets(dsai);
        if (ds.empty()) {
            return nullptr;
            //spdlog::critical("Unable to allocate DescriptorSet");
            //throw std::exception("Unable to allocate DescriptorSet");
        }
        return std::make_shared<DescriptorSet>(device_, shared_from_this(), ds[0]);
    }

    void DescriptorPool::handBackDescriptorSet(vk::DescriptorSet descriptor_set)
    {
        OPTICK_EVENT()
        device_.freeDescriptorSets(handle, descriptor_set);
    }

    std::shared_ptr<DescriptorSet> DescriptorPool::allocateDescriptorSet(
        vk::DescriptorSetLayout layout,
        const std::vector<uint32_t> & counts)
    {
        OPTICK_EVENT()
        std::vector<vk::DescriptorSetLayout> vv(1, VkDescriptorSetLayout(layout));

        vk::DescriptorSetVariableDescriptorCountAllocateInfo dsvdcai{};
        dsvdcai.setDescriptorCounts(counts);

        vk::DescriptorSetAllocateInfo dsai{
            handle,
            1,
            vv.data()
        };
        dsai.pNext = &dsvdcai;

        auto ds = Device::VkDevice().allocateDescriptorSets(dsai);
        if (ds.empty()) {
            return nullptr;
            //spdlog::critical("Unable to allocate DescriptorSet");
            //throw std::exception("Unable to allocate DescriptorSet");
        }
        return std::make_shared<DescriptorSet>(device_, shared_from_this(), ds[0]);
    }

    DescriptorPoolGroup::DescriptorPoolGroup(const DescriptorPoolTemplate & poolTemplate)
        : poolSizes(poolTemplate.poolSizes)
        , max(poolTemplate.max)
    {
        OPTICK_EVENT()
    }

    std::shared_ptr<DescriptorSet> DescriptorPoolGroup::getDescriptorSet(vk::DescriptorSetLayout layout)
    {
        OPTICK_EVENT();

        std::stringstream ss;
        ss << std::this_thread::get_id();

        if (!descriptorPool) {
            descriptorPool = iVulkan()->CreateDescriptorPool(poolSizes, max);

            spdlog::debug("Thread {} allocating a descriptorsetpool {}", ss.str(), descriptorPool->handle);
        }
        std::shared_ptr<DescriptorSet> ds;

        try {
            ds = descriptorPool->allocateDescriptorSet(layout);
        } catch (vk::OutOfPoolMemoryError  ) {
            ds = nullptr;
        }
        if (ds) {
            spdlog::debug("Thread {} allocating a descriptorset {} ", ss.str(), ds->handle);

            return ds;
        }
        descriptorPool = iVulkan()->CreateDescriptorPool(poolSizes, max);
        spdlog::debug("*Thread {} allocating a descriptorsetpool {}", ss.str(), descriptorPool->handle);

        ds = descriptorPool->allocateDescriptorSet(layout);
        spdlog::debug("*Thread {} allocating a descriptorset {} ", ss.str(), ds->handle);
        return ds;
    }

    std::shared_ptr<DescriptorSet> DescriptorPoolGroup::getDescriptorSet(
        vk::DescriptorSetLayout layout,
        const std::vector<uint32_t> & counts)
    {
        OPTICK_EVENT()
        std::stringstream ss;
        ss << std::this_thread::get_id();
        if (!descriptorPool) {
            descriptorPool = iVulkan()->CreateDescriptorPool(poolSizes, max);
            spdlog::debug("Thread {} allocating a descriptorsetpool {}", ss.str(), descriptorPool->handle);
        }
        auto ds = descriptorPool->allocateDescriptorSet(layout, counts);
        if (ds) {
            spdlog::debug("Thread {} allocating a descriptorset {}  with counts", ss.str(), ds->handle);
            return ds;
        }
        descriptorPool = iVulkan()->CreateDescriptorPool(poolSizes, max);
        spdlog::debug("*Thread {} allocating a descriptorsetpool {}", ss.str(), descriptorPool->handle);
        ds = descriptorPool->allocateDescriptorSet(layout, counts);
        spdlog::debug("*Thread {} allocating a descriptorset {}  with counts", ss.str(), ds->handle);
        return ds;
    }

    DescriptorPoolGroup::~DescriptorPoolGroup()
    {
        descriptorPool.reset();
    }
}