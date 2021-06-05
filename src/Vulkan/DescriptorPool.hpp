//
// Copyright (c) 2020 - Shane Hyde (shane@noctonyx.com)
//

#ifndef AMX_DESCRIPTORPOOL_HPP
#define AMX_DESCRIPTORPOOL_HPP

#include <vector>
#include <memory>
#include <deque>
#include "Device.h"

namespace RxCore
{
    class DescriptorSet;
    class Device;
#if 0
    struct DescriptorPoolTemplate
    {
        DescriptorPoolTemplate(const std::vector<vk::DescriptorPoolSize> & poolSizes, uint32_t max)
            : poolSizes(poolSizes)
            , max(max)
        {
            RxUtil::Hasher h;

            h.u32(max);
            h.u32(static_cast<uint32_t>( poolSizes.size()));
            for (auto & s: poolSizes) {
                h.u32(s.descriptorCount);
                h.u32(static_cast<uint32_t>(s.type));
            }

            hash = h.get();
        }

        std::vector<vk::DescriptorPoolSize> poolSizes;
        uint32_t max;
        RxUtil::Hash hash;
    };
#endif
    class DescriptorPool : public std::enable_shared_from_this<DescriptorPool>
    {
    public:
        explicit DescriptorPool(Device * device, vk::DescriptorPool new_handle);

        DescriptorPool(vk::Device device, const DescriptorPool & other) = delete;
        //DescriptorPool(DescriptorPool&& other) = delete;
        DescriptorPool & operator=(const DescriptorPool & other) = delete;
        //DescriptorPool& operator=(DescriptorPool&& other) = delete;

        virtual ~DescriptorPool()
        {
            device_->destroyDescriptorPool(this);
        };

        std::shared_ptr<DescriptorSet> allocateDescriptorSet(vk::DescriptorSetLayout layout);
        std::shared_ptr<DescriptorSet> allocateDescriptorSet(
            vk::DescriptorSetLayout layout,
            const std::vector<uint32_t> & counts);
        void handBackDescriptorSet(vk::DescriptorSet descriptor_set);

        const vk::DescriptorPool handle;
    private:
        Device * device_;
    };
#if 0
    struct DescriptorPoolGroup
    {
        explicit DescriptorPoolGroup(const DescriptorPoolTemplate & poolTemplate);
        virtual ~DescriptorPoolGroup();
        //DescriptorPoolGroup();

        std::shared_ptr<DescriptorPool> descriptorPool;
        std::vector<vk::DescriptorPoolSize> poolSizes;
        uint32_t max;

        std::shared_ptr<DescriptorSet> getDescriptorSet(vk::DescriptorSetLayout layout);
        std::shared_ptr<DescriptorSet> getDescriptorSet(
            vk::DescriptorSetLayout layout,
            const std::vector<uint32_t> & counts);
    };
#endif
}
#endif //AMX_DESCRIPTORPOOL_HPP
