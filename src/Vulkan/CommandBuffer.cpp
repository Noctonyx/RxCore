#include "CommandBuffer.hpp"
#include "Buffer.hpp"
#include "VertexBuffer.h"
#include "IndexBuffer.hpp"
#include "DescriptorSet.hpp"
#include "FrameBuffer.h"
#include "Queue.hpp"
#include "optick/optick.h"
#include "Image.hpp"

namespace RxCore
{
    void CommandBuffer::begin()
    {
        OPTICK_EVENT()
        started_ = true;
        buffers_.clear();
        descriptorSets_.clear();

        handle_.begin({{}, nullptr});
    }

    void SecondaryCommandBuffer::begin(vk::RenderPass renderPass, uint32_t subPass)
    {
        OPTICK_EVENT()
        started_ = true;
        buffers_.clear();
        descriptorSets_.clear();

        vk::CommandBufferBeginInfo cbbi;
        vk::CommandBufferInheritanceInfo cbii;
        cbii.setRenderPass(renderPass).setSubpass(subPass);

        cbbi.setFlags(vk::CommandBufferUsageFlagBits::eRenderPassContinue);
        cbbi.setPInheritanceInfo(&cbii);
        handle_.begin(cbbi);
    }

    void CommandBuffer::end()
    {
        OPTICK_EVENT();

        handle_.end();
    }

    void CommandBuffer::beginRenderPass(
        vk::RenderPass renderPass,
        std::shared_ptr<FrameBuffer> fb,
        const vk::Extent2D extent,
        const std::vector<vk::ClearValue> & clearValues)
    {
        OPTICK_EVENT()
        vk::RenderPassBeginInfo rpbi{
            renderPass,
            fb->Handle(),
            {{0, 0}, extent},
            clearValues
        };
        frameBuffers_.emplace(std::move(fb));
        handle_.beginRenderPass(rpbi, vk::SubpassContents::eSecondaryCommandBuffers);
    }

    void CommandBuffer::setViewport(
        float x,
        float y,
        float width,
        float height,
        float minDepth,
        float maxDepth) const
    {
        OPTICK_EVENT()
        handle_.setViewport(0, {{x, y, width, height, minDepth, maxDepth}});
    }

    void CommandBuffer::setScissor(vk::Rect2D rect) const
    {
        OPTICK_EVENT()
        handle_.setScissor(0, {rect});
    }

    void CommandBuffer::clearScissor() const
    {
        handle_.setScissor(0, {});
    }

    void CommandBuffer::BindDescriptorSet(
        uint32_t firstSet,
        std::shared_ptr<DescriptorSet> usedSet)
    {
        handle_.bindDescriptorSets(
            vk::PipelineBindPoint::eGraphics,
            currentLayout_,
            firstSet,
            {usedSet->handle},
            usedSet->getOffsets());

        descriptorSets_.emplace(std::move(usedSet));
    }

    void CommandBuffer::pushConstant(
        vk::ShaderStageFlags shaderFlags,
        uint32_t offset,
        uint32_t size,
        const void * ptr) const
    {
        handle_.pushConstants(currentLayout_, shaderFlags, offset, size, ptr);
    }

    void CommandBuffer::bindPipeline(vk::Pipeline pipeline)
    {
        handle_.bindPipeline(vk::PipelineBindPoint::eGraphics, pipeline);
    }

    void CommandBuffer::bindVertexBuffer(std::shared_ptr<Buffer> buffer)
    {
        handle_.bindVertexBuffers(0, {buffer->handle()}, {0});
        buffers_.emplace(std::move(buffer));
    }

    void CommandBuffer::DrawIndexed(
        uint32_t indexCount,
        uint32_t instanceCount,
        uint32_t firstIndex,
        int32_t vertexOffset,
        uint32_t firstInstance)
    {
        handle_.drawIndexed(indexCount, instanceCount, firstIndex, vertexOffset, firstInstance);
    }

    void CommandBuffer::bindIndexBuffer(std::shared_ptr<IndexBuffer> buffer, uint64_t offset)
    {
        if (buffer->is16Bit_) {
            handle_.bindIndexBuffer(buffer->handle(), offset, vk::IndexType::eUint16);
        } else {
            handle_.bindIndexBuffer(buffer->handle(), offset, vk::IndexType::eUint32);
        }
        buffers_.emplace(std::move(buffer));
    }

    void CommandBuffer::EndRenderPass()
    {
        handle_.endRenderPass();
    }

    vk::ImageMemoryBarrier CommandBuffer::ImageBarrier(
        vk::Image image,
        vk::AccessFlags srcAccessMask,
        vk::AccessFlags destAccessMask,
        vk::ImageLayout oldLayout,
        vk::ImageLayout newLayout)
    {
        auto srr = vk::ImageSubresourceRange();
        srr.setAspectMask(vk::ImageAspectFlagBits::eColor)
           .setLevelCount(VK_REMAINING_MIP_LEVELS)
           .setLayerCount(VK_REMAINING_ARRAY_LAYERS);

        auto ib = vk::ImageMemoryBarrier();
        ib.setSrcAccessMask(srcAccessMask)
          .setDstAccessMask(destAccessMask)
          .setOldLayout(oldLayout)
          .setNewLayout(newLayout)
          .setImage(image)
          .setSubresourceRange(srr); //.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);

        return ib;
    }

    void CommandBuffer::Draw(
        uint32_t vertexCount,
        uint32_t instanceCount,
        uint32_t firstVertex,
        uint32_t firstInstance)
    {
        handle_.draw(vertexCount, instanceCount, firstVertex, firstInstance);
    }

#if 0
    void CommandBuffer::PushBuffer(
        const PipelineLayout & layout,
        uint32_t binding,
        std::shared_ptr<Buffer> buffer,
        vk::DescriptorType type,
        uint32_t set
    )
    {
        vk::DescriptorBufferInfo bi = buffer->GetDescriptor();

        vk::WriteDescriptorSet descriptors[1];
        descriptors[0].dstBinding = binding;
        descriptors[0].descriptorCount = 1;
        descriptors[0].descriptorType = type;
        descriptors[0].pBufferInfo = &bi;

        handle_.pushDescriptorSetKHR(
            vk::PipelineBindPoint::eGraphics,
            layout.handle,
            set,
            1,
            descriptors);
        _buffers.emplace(std::move(buffer));
    }
#endif
    void PrimaryCommandBuffer::executeSecondaries(uint16_t sequence)
    {
        OPTICK_EVENT("Execute 2nd")
        std::vector<vk::CommandBuffer> bufs(secondaries_[sequence].size());

        if (bufs.empty()) {
            return;
        }

        std::ranges::transform(
            secondaries_[sequence], bufs.begin(), [](auto & b)
            {
                return b->Handle();
            });
        handle_.executeCommands(bufs);
    }

    void PrimaryCommandBuffer::executeSecondary(const std::shared_ptr<SecondaryCommandBuffer> & secondary) {
        handle_.executeCommands({ secondary->Handle() });
        secondaries2_.push_back(secondary);
    }

    void PrimaryCommandBuffer::addSecondaryBuffer(
        std::shared_ptr<SecondaryCommandBuffer> secondary,
        uint16_t sequence)
    {
        if (!secondary || !secondary->wasStarted()) {
            return;
        }
        if (!secondaries_.contains(sequence)) {
            secondaries_[sequence] = {};
        }
        secondaries_[sequence].push_back(std::move(secondary));
    }

    void TransferCommandBuffer::copyBuffer(
        std::shared_ptr<Buffer> source,
        std::shared_ptr<Buffer> dest,
        vk::DeviceSize srcOffset,
        vk::DeviceSize destOffset,
        vk::DeviceSize size)
    {
        vk::BufferCopy bc{srcOffset, destOffset, size};
        handle_.copyBuffer(source->handle(), dest->handle(), 1, &bc);
        (void) buffers_.emplace(std::move(source));
        (void) buffers_.emplace(std::move(dest));
    }

    void TransferCommandBuffer::submitAndWait()
    {
        auto queue = RxCore::iVulkan()->transferQueue_;

        vk::SubmitInfo si{
            nullptr, nullptr,
            handle_
        };
        auto fence = Device::VkDevice().createFence({});
        queue->GetHandle().submit(si, fence);

        const auto result = Device::VkDevice().waitForFences(1, &fence, true, MAXUINT64);
        assert(result == vk::Result::eSuccess);

        Device::VkDevice().destroyFence(fence);
        buffers_.clear();
    }

    void TransferCommandBuffer::copyBufferToImage(
        std::shared_ptr<Buffer> source,
        std::shared_ptr<Image> dest,
        vk::Extent3D extent,
        uint32_t layerCount,
        uint32_t baseArrayLayer, uint32_t mipLevel)
    {
        vk::BufferImageCopy bc{
            0,
            0,
            0,
            {vk::ImageAspectFlagBits::eColor, mipLevel, baseArrayLayer, layerCount},
            {0, 0, 0},
            extent
        };
        handle_.copyBufferToImage(
            source->handle(),
            dest->handle(),
            vk::ImageLayout::eTransferDstOptimal,
            1,
            &bc);

        images_.emplace(std::move(dest));
        buffers_.emplace(std::move(source));
    }

    void TransferCommandBuffer::imageTransition(
        std::shared_ptr<Image> dest,
        vk::ImageLayout destLayout,
        uint32_t mipLevel)
    {
        //vk::PipelineStageFlags src_stage, dest_stage;

        vk::ImageMemoryBarrier imb{};
        imb.image = dest->handle();
        imb.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        imb.subresourceRange.baseMipLevel = mipLevel;
        imb.subresourceRange.levelCount = 1;
        imb.subresourceRange.baseArrayLayer = 0;
        imb.subresourceRange.layerCount = 1;
        imb.oldLayout = vk::ImageLayout::eUndefined;
        imb.newLayout = destLayout;
        imb.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
        imb.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);


        std::vector<vk::ImageMemoryBarrier> vi = {imb};

        handle_.pipelineBarrier(
            vk::PipelineStageFlagBits::eTransfer,
            vk::PipelineStageFlagBits::eTransfer,
            {},
            {},
            {},
            {imb});
        images_.emplace(std::move(dest));
    }
} // namespace RXCore
