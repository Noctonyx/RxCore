//
// Created by shane on 13/02/2021.
//

#ifndef RX_IPIPELINELAYOUT_H
#define RX_IPIPELINELAYOUT_H

#include "Vulk.hpp"

namespace RxCore
{
    class IPipelineLayout
    {
    public:
        virtual vk::DescriptorSetLayout getDescriptorSetlayout(uint32_t set) = 0;

    };
}

#endif //RX_IPIPELINELAYOUT_H
