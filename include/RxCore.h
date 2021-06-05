////////////////////////////////////////////////////////////////////////////////
// MIT License
//
// Copyright (c) 2021.  Shane Hyde
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

#pragma once

#include <vector>
#include <deque>
#include <memory>
#include <optick/optick.h>
#include <Hasher.h>

namespace RxCore {
    class RxDevice;
    class SwapChain;
    class ImageView;
}

namespace RxApi
{
    class Device;
    class Window;
    class Buffer;
    class StorageBuffer;
    class StorageDynamicBuffer;
    class UniformBuffer;
    class UniformDynamicBuffer;
    class VertexBuffer;
    class IndexBuffer;
    class CommandBuffer;
    class SecondaryCommandBuffer;
    class DescriptorPool;
    class DescriptorSet;
    class Image;
    class ImageView;
    class FrameBuffer;
    class CommandPool;
    class Pipeline;
    class Shader;
    class Surface;
    class SwapChain;

    struct MemHeapStatus;

    using DevicePtr = Device *;
    using WindowPtr = Window *;
    using BufferPtr = std::shared_ptr<Buffer>;
    using StorageBufferPtr = std::shared_ptr<StorageBuffer>;
    using StorageDynamicBufferPtr = std::shared_ptr<StorageDynamicBuffer>;
    using UniformBufferPtr = std::shared_ptr<UniformBuffer>;
    using UniformDynamicBufferPtr = std::shared_ptr<UniformDynamicBuffer>;
    using VertexBufferPtr = std::shared_ptr<VertexBuffer>;
    using IndexBufferPtr = std::shared_ptr<IndexBuffer>;
    using CommandBufferPtr = std::shared_ptr<CommandBuffer>;
    using SecondaryCommandBufferPtr = std::shared_ptr<SecondaryCommandBuffer>;
    using DescriptorSetPtr = std::shared_ptr<DescriptorSet>;
    using DescriptorPoolPtr = std::shared_ptr<DescriptorPool>;
    using ImagePtr = std::shared_ptr<Image>;
    using ImageViewPtr = std::shared_ptr<ImageView>;
    using FrameBufferPtr = std::shared_ptr<FrameBuffer>;
    using CommandPoolPtr = std::shared_ptr<CommandPool>;
    using PipelinePtr = std::shared_ptr<Pipeline>;
    using ShaderPtr = std::shared_ptr<Shader>;
    using SurfacePtr = std::shared_ptr<Surface>;
    using SwapChainPtr = std::unique_ptr<SwapChain>;

    enum class BufferLocation : uint8_t
    {
        GPU,
        CPU,
        CPU_to_GPU
    };

    enum class ImageFormat : uint32_t { };

    enum class PipelineStageFlags : uint32_t { };

    enum class DescriptorType : uint32_t { };

    class Device
    {
    public:
        virtual ~Device() = default;

        virtual StorageBufferPtr createStorageBuffer(BufferLocation location, size_t size) = 0;
        virtual StorageDynamicBufferPtr createStorageDynamicBuffer(
            BufferLocation location,
            size_t size) = 0;
        virtual UniformBufferPtr createUniformBuffer(BufferLocation location, size_t size) = 0;
        virtual UniformDynamicBufferPtr createUniformDynamicBuffer(
            BufferLocation location,
            size_t blockSize,
            size_t count) = 0;
        virtual VertexBufferPtr createVertexBuffer(BufferLocation location, size_t size) = 0;
        virtual IndexBufferPtr createIndexBuffer(BufferLocation location, size_t size) = 0;
        virtual BufferPtr createBuffer(BufferLocation location, size_t size) = 0;

        virtual void getMemBudget(std::vector<MemHeapStatus> & heaps) const = 0;
        SurfacePtr getSurface() const;
        void waitIdle() const;
    private:
        std::unique_ptr<RxCore::RxDevice> pImpl;
    };

    class Buffer
    {
    public:
        virtual ~Buffer() = default;

        virtual void map() = 0;
        virtual void unmap() = 0;
        virtual void update(const void * data, size_t size) const = 0;
        virtual void update(const void * data, size_t offset, size_t size) const = 0;
    };

    class StorageBuffer : public Buffer
    {
    public:
    };

    class StorageDynamicBuffer : public StorageBuffer
    {
    public:
        virtual size_t getAlignment() = 0;
    };

    class UniformBuffer : public Buffer
    {
    public:
    };

    class UniformDynamicBuffer : public UniformBuffer
    {
    public:
        virtual size_t getAlignment() = 0;
    };

    struct Semaphore
    {
        intptr_t ptr;
    };

    struct PipelineLayout
    {
        intptr_t ptr;
    };

    struct RenderPass
    {
        intptr_t ptr;
    };

    struct Sampler
    {
        intptr_t ptr;
    };

    struct DescriptorSetLayout
    {
        intptr_t ptr;
    };

    struct Extent
    {
        uint32_t width;
        uint32_t height;
    };

    struct Extent3D
    {
        uint32_t width;
        uint32_t height;
        uint32_t levels;
    };

    struct Offset
    {
        int32_t x;
        int32_t y;
    };

    struct Rect
    {
        Offset offset;
        Extent extent;
    };

    using DeviceAddress = uint64_t;

    class Image
    {
    public:
        virtual ~Image() = default;
    };

    class ImageView
    {
    public:
        ~ImageView() = default;
        ImageView(std::shared_ptr<RxCore::ImageView> pImpl);
    private:
        std::shared_ptr<RxCore::ImageView> pImpl;
    };

    class CommandBuffer
    {
    public:
        virtual ~CommandBuffer() = default;
    };

    class SecondaryCommandBuffer : public CommandBuffer
    {
    public:
        ~SecondaryCommandBuffer() override = default;
    };

    class DescriptorPool {
    public:
    };

    class DescriptorSet
    {
    public:
        virtual ~DescriptorSet() = default;

        virtual void updateDescriptor(uint32_t binding,
                                      BufferPtr buffer,
                                      const uint32_t size = 0,
                                      const uint32_t offset = 0) = 0;
    };

    class FrameBuffer
    {
    public:
        virtual ~FrameBuffer() = default;
    };

    class CommandPool
    {
    public:
        virtual ~CommandPool() = default;
    };

    class Pipeline
    {
    public:
        virtual ~Pipeline() = default;
    };

    class Shader
    {
    public:
        virtual ~Shader() = default;
    };

    class Window
    {
    public:
        virtual ~Window() = default;

        virtual uint32_t getWidth() const = 0;
        virtual uint32_t getHeight() const = 0;
    };

    class Surface
    {
    public:
        virtual ~Surface() = default;

        virtual SwapChainPtr CreateSwapChain() = 0;
    };

    class SwapChain
    {
    public:
        ~SwapChain() = default;

        [[nodiscard]] bool swapChainOutOfDate() const;
        void setSwapChainOutOfDate(bool status);
        [[nodiscard]] Extent getExtent() const;
        std::tuple<RxApi::ImageViewPtr,RxApi::Semaphore, uint32_t> acquireNextImage();
        void presentImage(RxApi::ImageViewPtr imageView, RxApi::Semaphore renderComplete);

    private:
        std::unique_ptr<RxCore::SwapChain> pImpl;
    };

    struct MemHeapStatus
    {
        uint64_t budget;
        uint64_t usage;
    };

    struct JobBase : public std::enable_shared_from_this<JobBase>
    {
        using Ptr = std::shared_ptr<JobBase>;

        std::atomic_uint16_t childCount;
        Ptr parent;
        //std::shared_ptr<JobBase> followedJob;
        std::vector<Ptr> followOns;

        virtual ~JobBase()
        {
            followOns.clear();
            //followedJob.reset();
            parent.reset();
        }

        JobBase(const JobBase & other) = delete;

        JobBase(JobBase && other) noexcept = delete;

        JobBase & operator=(const JobBase & other) = delete;

        JobBase & operator=(JobBase && other) noexcept = delete;

        JobBase()
        {
            childCount.store(1);
        }

        void waitComplete() const;

        bool isCompleted() const
        {
            return childCount.load() == 0;
        }

        virtual void execute() = 0;

        void addFollowOnJob(std::shared_ptr<JobBase> j)
        {
            //j->followedJob = shared_from_this();
            followOns.push_back(std::move(j));
        }

        void schedule(bool background = false);
        void schedule(std::shared_ptr<JobBase> parentJob, bool background = false);
    };

    //    StorageBufferPtr createStorageBuffer(Device device, BufferLocation location, size_t size);
    //  BufferPtr createUniformBuffer(Device device, BufferLocation location, size_t size);
    //BufferPtr createVertexBuffer(Device device, BufferLocation location, size_t size);
    //BufferPtr createIndexBuffer(Device device, BufferLocation location, size_t size);

    struct ThreadResources
    {
        SecondaryCommandBufferPtr getCommandBuffer();

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

    std::unique_ptr<Window> createWindow(uint32_t width, uint32_t height, const char * title);
    std::unique_ptr<Device> createDevice(Window *);
}
