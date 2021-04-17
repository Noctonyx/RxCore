//
// Created by shane on 29/12/2020.
//

#ifndef RX_COMMANDPOOL_HPP
#define RX_COMMANDPOOL_HPP

#include "Vulk.hpp"
#include "DeviceObject.h"
//#include "Rendering/Renderer.hpp"

namespace RxCore
{
    class CommandBuffer;
    class SecondaryCommandBuffer;
    class PrimaryCommandBuffer;
    class TransferCommandBuffer;

    class RenderPass;

    class CommandPool : public DeviceObject,  public std::enable_shared_from_this<CommandPool>
    {
    public:
        typedef std::shared_ptr<CommandBuffer> ptr;

        CommandPool(vk::Device device, vk::CommandPool handle, uint32_t qf);
        //CommandPool(uint32_t queueFamily);
        ~CommandPool();

        std::shared_ptr<PrimaryCommandBuffer> GetPrimaryCommandBuffer();
        std::shared_ptr<TransferCommandBuffer> createTransferCommandBuffer();
        std::shared_ptr<SecondaryCommandBuffer> GetSecondaryCommandBuffer();

        vk::CommandPool GetHandle() const { return handle; }

    private:
        vk::CommandPool handle;
        //uint32_t queuefamily_;
    };
} // namespace RXCore
#endif // RX_COMMANDPOOL_HPP
