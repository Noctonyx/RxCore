#pragma once

#include <memory>
#include <unordered_map>
#include <RxCore.h>
#include "Vulk.hpp"
#include "Memory.h"
#include "SDL.h"
//#include <GLFW/glfw3.h>
#include "Hasher.h"

namespace RxCore
{
    class Instance;
    class PhysicalDevice;
    class Surface;
    class VertexBuffer;
    class IndexBuffer;
    class Queue;
    class CommandPool;
    class Buffer;
    class Image;
    class DescriptorPool;
    class CommandPool;
    class Shader;

    struct MemHeapStatus
    {
        uint64_t budget;
        uint64_t usage;
    };

    class Device
    {
    protected:
        static Device * context_;

    public:
        Device(SDL_Window * window);

        Device(const Device & other) = delete;
        Device & operator=(const Device & other) = delete;
        Device & operator=(Device && other) = delete;

        ~Device();

    public:
        static Device * Context()
        {
            return context_;
        }

        static vk::Device VkDevice()
        {
            return context_->getDevice();
        }

        void WaitIdle() const;

        std::shared_ptr<DescriptorPool> CreateDescriptorPool(
            std::vector<vk::DescriptorPoolSize> poolSizes,
            uint32_t max);

        std::shared_ptr<CommandPool> CreateGraphicsCommandPool();

        std::shared_ptr<Image> createImage(
            const vk::Format format,
            const vk::Extent3D extent,
            uint32_t mipLevels,
            uint32_t layers,
            vk::ImageUsageFlags usage,
            vk::ImageType type = vk::ImageType::e2D) const;
#if 0
        std::shared_ptr<Image> Create2DImage(
            vk::Format format,
            vk::Extent2D extent,
            uint32_t layers,
            vk::ImageUsageFlags usage) const;

        std::shared_ptr<Image> CreateImage(
            vk::Format format,
            vk::Extent3D extent,
            uint32_t layers,
            vk::ImageUsageFlags usage,
            vk::ImageType type = vk::ImageType::e2D) const;

        std::shared_ptr<Image> createImageWithoutMemory(
            vk::Format format,
            vk::Extent3D extent,
            uint32_t layers,
            vk::ImageUsageFlags usage,
            vk::ImageType type = vk::ImageType::e2D, uint32_t mipLevels = 1) const;

        std::shared_ptr<Image> Create2DImage(
            vk::Format format,
            uint32_t width,
            uint32_t height)
        const;

        std::shared_ptr<Image> Create2DImage(
            vk::Format format,
            uint32_t width,
            uint32_t height,
            uint8_t * pixels,
            uint32_t size) const;
#endif
        std::shared_ptr<Shader> createShader(const std::vector<uint32_t> & bytes) const;

        vk::Format GetDepthFormat(bool checkSamplingSupport) const;

        void transferBuffer(
            std::shared_ptr<Buffer> src,
            std::shared_ptr<Buffer> dst,
            size_t size,
            size_t srcOffset = 0,
            size_t destOffset = 0) const;
        void transitionImageLayout(
            const std::shared_ptr<Image> & image,
            vk::ImageLayout dstLayout)
        const;

        void transferBufferToImage(
            std::shared_ptr<Buffer> src,
            std::shared_ptr<Image> dst,
            vk::Extent3D extent,
            vk::ImageLayout destLayout,
            uint32_t layerCount,
            uint32_t baseArrayLayer, uint32_t mipLevel = 0) const;

        [[nodiscard]] vk::Device getDevice() const
        {
            return handle_;
        };
#if 0
        std::shared_ptr<Memory> allocateMemory(
            const vk::MemoryPropertyFlags memFlags,
            const vk::MemoryRequirements & memReq) const;
#endif
        void getMemBudget(std::vector<RxApi::MemHeapStatus> & heaps) const;
        // =----  Creating objects

        vk::Sampler createSampler(const vk::SamplerCreateInfo & sci);
        vk::DescriptorSetLayout createDescriptorSetLayout(const vk::DescriptorSetLayoutCreateInfo & dslci);
        vk::PipelineLayout createPipelineLayout(const vk::PipelineLayoutCreateInfo & plci);

        std::shared_ptr<CommandPool> createCommandPool(const vk::CommandPoolCreateInfo & cci);
        // std::shared_ptr<Buffer> createUniformBuffer(MemoryType memType, const uint64_t size, void * data);
        std::shared_ptr<Buffer> createStagingBuffer(size_t size, const void * data) const;
        std::shared_ptr<Buffer> createBuffer(
            const vk::BufferUsageFlags & flags,
            VmaMemoryUsage memType,
            vk::DeviceSize size,
            void * data = nullptr) const;

#if 0
        std::shared_ptr<IndexBuffer> createIndexBuffer(
            VmaMemoryUsage memType,
            uint32_t indexCount,
            bool is16) const;
#endif
        std::shared_ptr<IndexBuffer> createIndexBuffer(
            VmaMemoryUsage memType,
            uint32_t indexCount,
            bool is16,
            void * data = nullptr) const;
#if 0
        std::shared_ptr<VertexBuffer> createVertexBuffer(
            VmaMemoryUsage memType,
            uint32_t vertexCount,
            uint32_t vertexSize) const;
#endif
        std::shared_ptr<VertexBuffer> createVertexBuffer(
            VmaMemoryUsage memType,
            uint32_t vertexCount,
            uint32_t vertexSize,
            void * data = nullptr) const;

        // =---- Getting Information

        [[nodiscard]] vk::DeviceSize getUniformBufferAlignment(vk::DeviceSize size) const;
        [[nodiscard]] vk::DeviceSize getStorageBufferAlignment(vk::DeviceSize size) const;

        // =---- Actions

        void clearQueues();

        // Window * window;
        std::unique_ptr<Instance> instance;
        std::shared_ptr<PhysicalDevice> physicalDevice;
        std::shared_ptr<Surface> surface;
        VmaAllocator allocator{};

        std::shared_ptr<CommandPool> transferCommandPool_;
        std::shared_ptr<Queue> graphicsQueue_;
        std::shared_ptr<Queue> transferQueue_;
        std::shared_ptr<Queue> computeQueue_;
        void createDevice();

    private:
        vk::Device handle_;

        std::unordered_map<RxUtil::Hash, vk::Sampler> samplers_;
        std::unordered_map<RxUtil::Hash, vk::PipelineLayout> pipelineLayouts_;
        std::unordered_map<RxUtil::Hash, vk::DescriptorSetLayout> descriptorSetLayouts_;

        RxUtil::Hash getHashForSampler(vk::Sampler) const;
        RxUtil::Hash getHashForDescriptorSetLayout(vk::DescriptorSetLayout) const;
        //std::unordered_map<RXUtil::Hash, std::shared_ptr<Sampler>> samplers_;
        RxUtil::Hash getHash(const vk::SamplerCreateInfo & sci) const;
        RxUtil::Hash getHash(const vk::PipelineLayoutCreateInfo & plci) const;
        RxUtil::Hash getHash(const vk::DescriptorSetLayoutCreateInfo & dslci) const;
        RxUtil::Hash getHash(const vk::DescriptorSetLayoutBindingFlagsCreateInfo & dslbfci) const;
        RxUtil::Hash getPNextHash(const void * pNext) const;

        void createQueues();
    };

    inline Device * iVulkan()
    {
        return Device::Context();
    }
} // namespace RXCore
