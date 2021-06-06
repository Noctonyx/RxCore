////////////////////////////////////////////////////////////////////////////////
// MIT License
//
// Copyright (c) 2020-2021.  Shane Hyde (shane@noctonyx.com)
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

#include "DescriptorPool.hpp"
#include "DescriptorSet.hpp"
#include "Log.h"
#include "optick/optick.h"

namespace RxCore
{
    DescriptorPool::DescriptorPool(Device * device, vk::DescriptorPool new_handle)
        : handle(new_handle)
        , device_(device)
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

        auto ds =  device_->getDevice().allocateDescriptorSets(dsai);
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
        device_->getDevice().freeDescriptorSets(handle, descriptor_set);
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

        auto ds =  device_->getDevice().allocateDescriptorSets(dsai);
        if (ds.empty()) {
            return nullptr;
            //spdlog::critical("Unable to allocate DescriptorSet");
            //throw std::exception("Unable to allocate DescriptorSet");
        }
        return std::make_shared<DescriptorSet>(device_, shared_from_this(), ds[0]);
    }
#if 0
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
#endif
}