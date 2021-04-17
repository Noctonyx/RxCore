//
// Created by shane on 3/02/2021.
//

#ifndef RX_DEVICEOBJECT_H
#define RX_DEVICEOBJECT_H

#include "Vulk.hpp"
#include "Util.h"

namespace RxCore
{
    class DeviceObject
    {
    public:
        explicit DeviceObject(vk::Device device) : device_(device)
        {}

    protected:
        vk::Device device_;
    };
} // namespace RXCore

#endif // RX_DEVICEOBJECT_H
