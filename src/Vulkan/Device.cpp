#include "Device.h"
#include "Instance.hpp"
#include "PhysicalDevice.hpp"
#include "SwapChain.hpp"
#include "CommandPool.hpp"
#include "CommandBuffer.hpp"
#include "Buffer.hpp"
#include "Queue.hpp"
#include "Image.hpp"
#include "DescriptorPool.hpp"
#include "IndexBuffer.hpp"
#include "VertexBuffer.h"
#include "Shader.h"
#include <Hasher.h>
#include "Log.h"
#include "optick/optick.h"
#include "SDL_vulkan.h"

namespace RxCore
{
    Device * Device::context_ = nullptr;

    Device::Device(SDL_Window * window)
    {
        instance = std::make_unique<Instance>(this, window);
        physicalDevice = std::make_shared<PhysicalDevice>(this);
        createDevice();
#if 1
        VmaAllocatorCreateInfo allocator_info = {};
        allocator_info.vulkanApiVersion = VK_API_VERSION_1_2;
        allocator_info.physicalDevice = physicalDevice->GetHandle();
        allocator_info.device = handle_;
        allocator_info.instance = instance->GetHandle();
        allocator_info.flags = VMA_ALLOCATOR_CREATE_EXT_MEMORY_BUDGET_BIT | VMA_ALLOCATOR_CREATE_BUFFER_DEVICE_ADDRESS_BIT;

#if _DEBUG
        VmaRecordSettings rs;
        rs.flags = 0;
        rs.pFilePath = "D:\\vma.record.csv";
#endif
        //allocator_info.pRecordSettings = &rs;

        vmaCreateAllocator(&allocator_info, &allocator);

        VmaBudget budget[VK_MAX_MEMORY_HEAPS];
        vmaGetBudget(allocator, budget);
#endif
        ::VkSurfaceKHR surface_khr;
        // vk::Win32SurfaceCreateInfoKHR
        if(!SDL_Vulkan_CreateSurface(window, static_cast<VkInstance>(instance->GetHandle()), &surface_khr)) {
        //if (glfwCreateWindowSurface(static_cast<VkInstance>(instance->GetHandle()), window, nullptr,
          //                          &surface_khr) != VK_SUCCESS) {
            spdlog::critical("Failed to created the window surface!");
        }
        Device::context_ = this;

        surface_ = surface_khr;
        getSurfaceDetails();
        selectPresentationQueueFamily();
        selectSurfaceFormat();
        selectPresentationMode();

        //surface = std::make_shared<Surface>(this, (vk::SurfaceKHR) surface_khr);

        createQueues();

        transferCommandPool_ = createCommandPool({{}, physicalDevice->GetTransferQueueFamily()});

#if USE_OPTICK
        ::VkDevice d = handle_;
        ::VkPhysicalDevice pd = physicalDevice->GetHandle();
        ::VkQueue q = graphicsQueue_->GetHandle();
        uint32_t qf = physicalDevice->GetGraphicsQueueFamily();
#endif
        OPTICK_GPU_INIT_VULKAN(&(d), &(pd), &(q), &(qf), 1, nullptr)
    }

    void Device::createQueues()
    {
        graphicsQueue_ =
            std::make_shared<Queue>(
                handle_.getQueue(physicalDevice->GetGraphicsQueueFamily(), 0),
                physicalDevice->GetGraphicsQueueFamily());
        if (physicalDevice->GetComputeQueueFamily() != physicalDevice->GetGraphicsQueueFamily()) {
            computeQueue_ = std::make_shared<Queue>(
                handle_.getQueue(physicalDevice->GetComputeQueueFamily(), 0),
                physicalDevice->GetComputeQueueFamily());
        } else {
            computeQueue_ = graphicsQueue_;
        }

        if (physicalDevice->GetTransferQueueFamily() != physicalDevice->GetGraphicsQueueFamily()) {
            if (physicalDevice->GetTransferQueueFamily() !=
                physicalDevice->GetComputeQueueFamily()) {
                transferQueue_ = std::make_shared<Queue>(
                    handle_.getQueue(physicalDevice->GetTransferQueueFamily(), 0),
                    physicalDevice->GetTransferQueueFamily());
            } else {
                transferQueue_ = computeQueue_;
            }
        } else {
            transferQueue_ = graphicsQueue_;
        }
    }

    void Device::createDevice()
    {
        std::vector<vk::DeviceQueueCreateInfo> dqci;
        //        std::vector<float> pris = {1.f, 1.f, 1.f};
        std::vector<float> pris2 = {1.f};

        auto & n = dqci.emplace_back();

        n.setQueueFamilyIndex(physicalDevice->GetGraphicsQueueFamily())
         .setQueueCount(1)
         .setQueuePriorities(pris2);

        if (physicalDevice->GetComputeQueueFamily() != physicalDevice->GetGraphicsQueueFamily()) {
            dqci.push_back({{}, physicalDevice->GetComputeQueueFamily(), 1, pris2.data()});
        }

        if (physicalDevice->GetTransferQueueFamily() != physicalDevice->GetGraphicsQueueFamily() &&
            physicalDevice->GetTransferQueueFamily() != physicalDevice->GetComputeQueueFamily()) {
            dqci.push_back({{}, physicalDevice->GetTransferQueueFamily(), 1, pris2.data()});
        }

        vk::DeviceCreateInfo ci{};
        vk::PhysicalDeviceFeatures2 feat{};

        vk::PhysicalDeviceBufferDeviceAddressFeatures bdaf{};
        bdaf.setBufferDeviceAddress(true);

        // vk::PhysicalDeviceFloat16Int8FeaturesKHR f168;
        // f168.shaderInt8 = true;

        // vk::PhysicalDevice8BitStorageFeatures f8;
        // f8.storageBuffer8BitAccess = true;
        // f8.uniformAndStorageBuffer8BitAccess = true;

        // vk::PhysicalDevice16BitStorageFeatures f16;
        // f16.storageBuffer16BitAccess = true;
        // f16.uniformAndStorageBuffer16BitAccess = true;

        feat.features.samplerAnisotropy = true;
        feat.features.sampleRateShading = true;
        // feat.features.shaderInt16 = true;

        vk::PhysicalDeviceDescriptorIndexingFeatures dif;
        dif.shaderSampledImageArrayNonUniformIndexing = true;
        dif.shaderStorageBufferArrayNonUniformIndexing = true;
        dif.shaderUniformBufferArrayNonUniformIndexing = true;
        dif.descriptorBindingVariableDescriptorCount = true;
        dif.runtimeDescriptorArray = true;
        dif.descriptorBindingPartiallyBound = true;
        dif.descriptorBindingSampledImageUpdateAfterBind = true;

        feat.pNext = &dif;
        // f8.pNext = &f16;
        // f16.pNext = &f168;
        // f168.pNext = &dif;

        feat.features.depthClamp = true;

        const std::vector<const char *> list_extensions = {
            VK_KHR_SWAPCHAIN_EXTENSION_NAME,
            //VK_EXT_BUFFER_DEVICE_ADDRESS_EXTENSION_NAME,
            // VK_KHR_MAINTENANCE1_EXTENSION_NAME,
            // VK_KHR_PUSH_DESCRIPTOR_EXTENSION_NAME,
            // VK_KHR_16BIT_STORAGE_EXTENSION_NAME,
            // VK_KHR_8BIT_STORAGE_EXTENSION_NAME,
            // VK_KHR_SHADER_FLOAT16_INT8_EXTENSION_NAME,
            VK_EXT_DESCRIPTOR_INDEXING_EXTENSION_NAME, VK_EXT_MEMORY_BUDGET_EXTENSION_NAME
        };

        const std::vector<const char *> list_layers = {"VK_LAYER_KHRONOS_validation"};
        /*
                std::vector<float> pris = {1.f,1.f};

                auto gqf = Context->physicalDevice->GetGraphicsQueueFamily()

                vk::DeviceQueueCreateInfo dqci[2] = {
                    {
                        {}, Context->physicalDevice->GetGraphicsQueueFamily(),        2, pris.data()
                    },
                    {
                        {}, Context->physicalDevice->GetComputeQueueFamily().value(), 1, pris.data()
                    }
                };
        */
        // ci.pEnabledFeatures = &feat;
        ci.pNext = &bdaf;
        bdaf.pNext = &feat;
        ci.pQueueCreateInfos = dqci.data();
        ci.queueCreateInfoCount = static_cast<uint32_t>(dqci.size());

        ci.enabledExtensionCount = static_cast<uint32_t>(list_extensions.size());
        ci.ppEnabledExtensionNames = list_extensions.data();

        ci.enabledLayerCount = static_cast<uint32_t>(list_layers.size());
        ci.ppEnabledLayerNames = list_layers.data();

        handle_ = physicalDevice->GetHandle().createDevice(ci);
        VULKAN_HPP_DEFAULT_DISPATCHER.init(handle_);
    }

    Device::~Device()
    {
        OPTICK_SHUTDOWN()

        clearQueues();

        for (auto & s: samplers_) {
            handle_.destroySampler(s.second);
        }

        for (auto & s: pipelineLayouts_) {
            handle_.destroyPipelineLayout(s.second);
        }

        for (auto & s: descriptorSetLayouts_) {
            handle_.destroyDescriptorSetLayout(s.second);
        }

        samplers_.clear();
        pipelineLayouts_.clear();
        descriptorSetLayouts_.clear();

        // swapChain.reset();
        //surface.reset();
        instance->GetHandle().destroySurfaceKHR(surface_);

#if 1
        vmaDestroyAllocator(allocator);
        allocator = nullptr;
#endif
        handle_.waitIdle();
        handle_.destroy();

        physicalDevice.reset();
        instance.reset();
    }

    void Device::clearQueues()
    {
        graphicsQueue_.reset();
        computeQueue_.reset();
        transferQueue_.reset();
        transferCommandPool_.reset();
    }

    void Device::transferBuffer(
        std::shared_ptr<Buffer> src,
        std::shared_ptr<Buffer> dst,
        size_t size,
        size_t srcOffset,
        size_t destOffset) const
    {
        auto cb = transferCommandPool_->createTransferCommandBuffer();

        //        vk::BufferCopy bc{srcOffset, destOffset, size};

        cb->begin();
        cb->copyBuffer(std::move(src), std::move(dst), srcOffset, destOffset, size);
        //      cb->Handle().copyBuffer(src->handle, dst->handle, 1, &bc);
        cb->end();
        cb->submitAndWait();
        // transferQueue_->submitAndWait(cb);
    }

    void Device::transitionImageLayout(
        const std::shared_ptr<Image> & image,
        vk::ImageLayout dstLayout) const
    {
        auto cb = transferCommandPool_->createTransferCommandBuffer();

        cb->begin();

        vk::PipelineStageFlags src_stage, dest_stage;

        vk::ImageMemoryBarrier imb{};
        imb.image = image->handle();
        imb.subresourceRange.aspectMask = vk::ImageAspectFlagBits::eColor;
        imb.subresourceRange.baseMipLevel = 0;
        imb.subresourceRange.levelCount = 1;
        imb.subresourceRange.baseArrayLayer = 0;
        imb.subresourceRange.layerCount = 1;
        imb.oldLayout = image->currentLayout_;
        imb.newLayout = dstLayout;
        imb.setDstQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);
        imb.setSrcQueueFamilyIndex(VK_QUEUE_FAMILY_IGNORED);

        if (image->currentLayout_ == vk::ImageLayout::eUndefined &&
            dstLayout == vk::ImageLayout::eTransferDstOptimal) {
            imb.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
            src_stage = vk::PipelineStageFlagBits::eTopOfPipe;
            dest_stage = vk::PipelineStageFlagBits::eTransfer;
        } else if (image->currentLayout_ == vk::ImageLayout::eTransferDstOptimal &&
            dstLayout == vk::ImageLayout::eShaderReadOnlyOptimal) {
            imb.setDstAccessMask(vk::AccessFlagBits::eTransferWrite);
            imb.setSrcAccessMask(vk::AccessFlagBits::eTransferRead);
            src_stage = vk::PipelineStageFlagBits::eTransfer;
            dest_stage = vk::PipelineStageFlagBits::eTransfer;
        } else {
            throw std::invalid_argument("unsupported layout transition!");
        }

        // cb->imageTransition()

        cb->Handle().pipelineBarrier(src_stage, dest_stage, {}, 0, nullptr, 0, nullptr, 1, &imb);
        cb->end();
        cb->submitAndWait();
        // transferQueue_->submitAndWait(cb);
        //        cb->SubmitIdle();
        image->currentLayout_ = dstLayout;
    }

    void Device::transferBufferToImage(
        std::shared_ptr<Buffer> src,
        std::shared_ptr<Image> dst,
        vk::Extent3D extent,
        vk::ImageLayout destLayout,
        uint32_t layerCount,
        uint32_t baseArrayLayer,
        uint32_t mipLevel) const
    {
        assert(src);
        assert(dst);

        auto cb = transferCommandPool_->createTransferCommandBuffer();

        cb->begin();

        // if (dst->currentLayout_ != vk::ImageLayout::eTransferDstOptimal) {
        cb->imageTransition(dst, vk::ImageLayout::eTransferDstOptimal, mipLevel);
        dst->currentLayout_ = vk::ImageLayout::eTransferDstOptimal;
        // TransitionImageLayout(dst, vk::ImageLayout::eTransferDstOptimal);
        //  }

        cb->copyBufferToImage(
            src, dst, extent, layerCount, baseArrayLayer,
            mipLevel);
        cb->imageTransition(dst, destLayout, mipLevel);
        cb->end();
        cb->submitAndWait();
    }

    vk::DeviceSize Device::getUniformBufferAlignment(vk::DeviceSize size) const
    {
        return physicalDevice->GetUniformBufferAlignment(size);
    }

    vk::DeviceSize Device::getStorageBufferAlignment(vk::DeviceSize size) const
    {
        return physicalDevice->GetStorageBufferAlignment(size);
    }

    std::shared_ptr<Buffer> Device::createBuffer(
        const vk::BufferUsageFlags & flags,
        VmaMemoryUsage memType,
        const vk::DeviceSize size,
        void * data)
    {
        OPTICK_EVENT()

        VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        buffer_info.size = size;
        buffer_info.usage = static_cast<VkBufferUsageFlags>(flags);
        if (data) {
            buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        VmaAllocationCreateInfo allocation_create_info = {};
        allocation_create_info.usage = memType;

        VkBuffer buffer;
        VmaAllocation allocation;
        VmaAllocationInfo alloc_info;
        vmaCreateBuffer(
            allocator, &buffer_info, &allocation_create_info, &buffer, &allocation,
            &alloc_info);

        //assert(res == VK_SUCCESS );

        auto alloc = std::make_shared<Allocation>(allocator, allocation);
        auto mb = std::make_shared<Buffer>(this, buffer, alloc, size);

        if (data) {
            VkMemoryPropertyFlags memFlags;
            vmaGetMemoryTypeProperties(allocator, alloc_info.memoryType, &memFlags);
            if ((memFlags & VK_MEMORY_PROPERTY_HOST_VISIBLE_BIT) != 0) {
                // Allocation ended up in mappable memory. You can map it and access it directly.
                void * mapped_data;

                vmaMapMemory(allocator, allocation, &mapped_data);
                memcpy(mapped_data, data, size);
                vmaUnmapMemory(allocator, allocation);
            } else {
                auto stage_buf = createStagingBuffer(size, data);
                transferBuffer(stage_buf, mb, size);
                // Allocation ended up in non-mappable memory.
                // You need to create CPU-side buffer in VMA_MEMORY_USAGE_CPU_ONLY and make a
                // transfer.
            }
        }

        return mb;
    }

    std::shared_ptr<VertexBuffer> Device::createVertexBuffer(
        VmaMemoryUsage memType,
        uint32_t vertexCount,
        uint32_t vertexSize,
        void * data)
    {
        auto s = vertexCount * vertexSize;

        VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        buffer_info.size = s;
        buffer_info.usage = VK_BUFFER_USAGE_VERTEX_BUFFER_BIT;
        if (data) {
            buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        VmaAllocationCreateInfo alloc_info = {};
        alloc_info.usage = memType;

        VkBuffer buffer;
        VmaAllocation allocation;
        vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &buffer, &allocation, nullptr);
        auto alloc = std::make_shared<Allocation>(allocator, allocation);

        auto mb = std::make_shared<VertexBuffer>(this, buffer, alloc, vertexCount, vertexSize);
        if (data) {
            auto stage_buf = createStagingBuffer(s, data);
            transferBuffer(stage_buf, mb, s);
        }
        /// auto buf = createStagingBuffer(s, data);
        // auto mem = allocateMemory(vk::MemoryPropertyFlagBits::eDeviceLocal,
        // b->getMemoryRequirements()); b->bindMemory(mem); transferBuffer(buf, b, s);

        // auto b = CreateBuffer(vk::BufferUsageFlagBits::eVertexBuffer,
        // vk::MemoryPropertyFlagBits::eDeviceLocal, vertex_size * vertex_count, data); auto b =
        // std::make_shared<VertexBuffer>(vk::MemoryPropertyFlagBits::eDeviceLocal, vertex_count,
        // vertex_size); auto buf = getStagingBuffer(vertex_size * vertex_count, data);
        // transferBuffer(buf, b, vertex_size * vertex_count);
        return mb;
    }

    std::shared_ptr<IndexBuffer> Device::createIndexBuffer(
        VmaMemoryUsage memType,
        uint32_t indexCount,
        bool is16,
        void * data)
    {
        auto s = (is16 ? sizeof(uint16_t) : sizeof(uint32_t)) * indexCount;

        VkBufferCreateInfo buffer_info = {VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO};
        buffer_info.size = s;
        buffer_info.usage = VK_BUFFER_USAGE_INDEX_BUFFER_BIT | VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        if (data) {
            buffer_info.usage |= VK_BUFFER_USAGE_TRANSFER_DST_BIT;
        }

        VmaAllocationCreateInfo alloc_info = {};
        alloc_info.usage = memType;

        VkBuffer buffer;
        VmaAllocation allocation;
        vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &buffer, &allocation, nullptr);
        auto alloc = std::make_shared<Allocation>(allocator, allocation);
        auto mb = std::make_shared<IndexBuffer>(this, buffer, alloc, indexCount, is16);

        if (data) {
            auto buf = createStagingBuffer(s, data);
            transferBuffer(buf, mb, s);
        }
        return mb;
    }

    std::shared_ptr<Image> Device::createImage(
        const vk::Format format,
        const vk::Extent3D extent,
        uint32_t mipLevels,
        uint32_t layers,
        vk::ImageUsageFlags usage,
        vk::ImageType type)
    {
        vk::ImageCreateInfo ici{
            {},
            type,
            format,
            extent,
            mipLevels,
            layers,
            vk::SampleCountFlagBits::e1,
            vk::ImageTiling::eOptimal,
            usage,
            vk::SharingMode::eExclusive
        };
        ici.setInitialLayout(vk::ImageLayout::eUndefined);
        VkImage image;
        VmaAllocation allocation;
        VmaAllocationCreateInfo alloc_info = {};
        alloc_info.usage = VMA_MEMORY_USAGE_GPU_ONLY;

        vmaCreateImage(
            allocator, reinterpret_cast<VkImageCreateInfo *>(&ici), &alloc_info, &image,
            &allocation, nullptr);

        auto i = std::make_shared<Image>(
            this, image, format, std::make_shared<Allocation>(allocator, allocation), extent);

        return i;
    }

    std::shared_ptr<Shader> Device::createShader(const std::vector<uint32_t> & bytes) const
    {
        auto sm = handle_.createShaderModule(
            {
                {}, static_cast<uint32_t>(bytes.size() * 4),
                bytes.data()
            });
        return std::make_shared<Shader>(sm);
    }

#if 0
    std::shared_ptr<Image> VulkanContext::Create2DImage(const vk::Format format,
                                                        const vk::Extent2D extent,
                                                        uint32_t layers,
                                                        const vk::ImageUsageFlags usage) const
    {
        const vk::Extent3D ex(extent.width, extent.height, 1);

        return CreateImage(format, ex, layers, usage, vk::ImageType::e2D);
    }

    std::shared_ptr<Image> VulkanContext::CreateImage(const vk::Format format,
                                                      const vk::Extent3D extent,
                                                      const uint32_t layers,
                                                      const vk::ImageUsageFlags usage,
                                                      const vk::ImageType type) const
    {
        auto image = createImageWithoutMemory(format, extent, layers, usage, type);

        vk::MemoryPropertyFlags search{};
        search = vk::MemoryPropertyFlagBits::eDeviceLocal;

        const auto memory_requirements = image->getMemoryRequirements();

        auto mem = allocateMemory(search, memory_requirements);
        image->bindMemory(mem);

        return image;
    }

    std::shared_ptr<Image> VulkanContext::createImageWithoutMemory(vk::Format format,
                                                                   vk::Extent3D extent,
                                                                   uint32_t layers,
                                                                   vk::ImageUsageFlags usage,
                                                                   vk::ImageType type,
                                                                   uint32_t mipLevels) const
    {
        vk::ImageCreateInfo ici{{},
                                type,
                                format,
                                extent,
                                mipLevels,
                                layers,
                                vk::SampleCountFlagBits::e1,
                                vk::ImageTiling::eOptimal,
                                usage,
                                vk::SharingMode::eExclusive};

        auto im = getDevice().createImage(ici);

        auto image = std::make_shared<Image>(handle_, im);
        image->format_ = format;
        image->imageType_ = type;
        image->extent_ = extent;

        return image;
    }

    std::shared_ptr<Image> VulkanContext::Create2DImage(vk::Format format,
                                                        uint32_t width,
                                                        uint32_t height) const
    {
        vk::Extent2D e(width, height);

        auto i =
            Create2DImage(static_cast<vk::Format>(format), e, 1,
                          vk::ImageUsageFlagBits::eSampled | vk::ImageUsageFlagBits::eTransferDst);
        return i;
    }

    std::shared_ptr<Image> VulkanContext::Create2DImage(vk::Format format,
                                                        uint32_t width,
                                                        uint32_t height,
                                                        uint8_t * pixels,
                                                        uint32_t size) const
    {
        auto i = Create2DImage(format, width, height);
        auto b = createStagingBuffer(size, pixels);

        transferBufferToImage(b, i, vk::Extent3D(width, height, 1), 1, 0);
        return i;
    }
#endif
#if 0
    vk::ShaderModule VulkanContext::LoadShader(const char * path)
    {
        FILE * file = fopen(path, "rb");
        fseek(file, 0, SEEK_END);

        long length = ftell(file);
        assert(length >= 0);
        fseek(file, 0, SEEK_SET);

        std::byte * b = new std::byte[length];

        auto rc = fread(b, 1, length, file);
        assert(rc == length);

        fclose(file);

        auto sm = getDevice().createShaderModule({{}, uint32_t(length), (uint32_t *) b});
        delete[] b;
        return sm;
    }
#endif

    std::shared_ptr<DescriptorPool> Device::CreateDescriptorPool(
        std::vector<vk::DescriptorPoolSize> poolSizes,
        uint32_t max)
    {
        vk::DescriptorPoolCreateInfo dpci;
        dpci.setPoolSizeCount(static_cast<uint32_t>(poolSizes.size()))
            .setFlags(
                vk::DescriptorPoolCreateFlagBits::eFreeDescriptorSet |
                vk::DescriptorPoolCreateFlagBits::eUpdateAfterBind)
            .setPPoolSizes(poolSizes.data())
            .setMaxSets(max);

        auto h = getDevice().createDescriptorPool(dpci);

        return std::make_shared<DescriptorPool>(this, h);
    }

    void Device::WaitIdle() const
    {
        getDevice().waitIdle();
    }

    std::shared_ptr<CommandPool> Device::CreateGraphicsCommandPool()
    {
        return createCommandPool({{}, physicalDevice->GetGraphicsQueueFamily()});
    }

    vk::Format Device::GetDepthFormat(bool checkSamplingSupport) const
    {
        return physicalDevice->getSupportedDepthFormat(checkSamplingSupport);
    }

    void Device::getMemBudget(std::vector<MemHeapStatus> & heaps) const
    {
        vk::PhysicalDeviceMemoryProperties2 pdmp2;
        vk::PhysicalDeviceMemoryBudgetPropertiesEXT ex;

        pdmp2.pNext = &ex;

        physicalDevice->GetHandle().getMemoryProperties2(&pdmp2);

        heaps.clear();

        uint32_t hc = pdmp2.memoryProperties.memoryHeapCount;
        for (uint32_t i = 0; i < hc; i++) {
            (void) heaps.emplace_back(MemHeapStatus{ex.heapBudget[i], ex.heapUsage[i]});
        }
    }

    std::shared_ptr<Buffer> Device::createStagingBuffer(size_t size, const void * data)
    {
        VkBufferCreateInfo buffer_info {
            VK_STRUCTURE_TYPE_BUFFER_CREATE_INFO, nullptr, 0, size,
            VK_BUFFER_USAGE_TRANSFER_SRC_BIT
        };

        VmaAllocationCreateInfo alloc_info{0, VMA_MEMORY_USAGE_CPU_ONLY};

        VkBuffer buffer_handle;
        VmaAllocation allocation;

        vmaCreateBuffer(allocator, &buffer_info, &alloc_info, &buffer_handle, &allocation, nullptr);

        auto all = std::make_shared<Allocation>(allocator, allocation);
        auto buf = std::make_shared<Buffer>(this, buffer_handle, all, size);
        all->map();
        all->update(data, size);
        all->unmap();

        return buf;
    }
#if 0
    std::shared_ptr<Memory> VulkanContext::allocateMemory(
        const vk::MemoryPropertyFlags memFlags,
        const vk::MemoryRequirements & memReq) const
    {
        vk::MemoryAllocateInfo mai{memReq.size,
                                   physicalDevice->GetMemoryIndex(memReq.memoryTypeBits, memFlags)};
        auto h = handle_.allocateMemory(mai);
        return std::make_shared<Memory>(handle_, h);
    }
#endif
    RxUtil::Hash Device::getHash(const vk::SamplerCreateInfo & sci) const
    {
        RxUtil::Hasher h;

        h.f32(sci.maxAnisotropy);
        h.f32(sci.mipLodBias);
        h.f32(sci.minLod);
        h.f32(sci.maxLod);
        h.u32(static_cast<uint32_t>(sci.minFilter));
        h.u32(static_cast<uint32_t>(sci.magFilter));
        h.u32(static_cast<uint32_t>(sci.mipmapMode));
        h.u32(sci.compareEnable);
        h.u32(static_cast<uint32_t>(sci.compareOp));
        h.u32(sci.anisotropyEnable);
        h.u32(static_cast<uint32_t>(sci.addressModeU));
        h.u32(static_cast<uint32_t>(sci.addressModeV));
        h.u32(static_cast<uint32_t>(sci.addressModeW));
        h.u32(static_cast<uint32_t>(sci.borderColor));
        h.u32(sci.unnormalizedCoordinates);

        return h.get();
    }

    vk::Sampler Device::createSampler(const vk::SamplerCreateInfo & sci)
    {
        auto h = getHash(sci);

        if (samplers_.contains(h)) {
            return samplers_[h];
        }

        auto sampler = handle_.createSampler(sci);
        //std::make_shared<RXCore::Sampler>(handle_, handle_.createSampler(sci));
        samplers_.emplace(h, sampler);

        return sampler;
    }

    std::shared_ptr<CommandPool> Device::createCommandPool(
        const vk::CommandPoolCreateInfo & cci)
    {
        auto h = handle_.createCommandPool(cci);
        return std::make_shared<CommandPool>(this, h, cci.queueFamilyIndex);
    }

    vk::PipelineLayout Device::createPipelineLayout(const vk::PipelineLayoutCreateInfo & plci)
    {
        auto h = getHash(plci);
        if (pipelineLayouts_.contains(h)) {
            return pipelineLayouts_[h];
        }

        std::vector<vk::DescriptorSetLayout> dsls(plci.setLayoutCount);
        //std::copy(plci.pSetLayouts, plci.pSetLayouts + (plci.setLayoutCount-1), dsls);

        auto layout = handle_.createPipelineLayout(plci);
        //std::make_shared<RXCore::PipelineLayout>(handle_, );
        pipelineLayouts_.emplace(h, layout);

        return layout;
    }

    vk::DescriptorSetLayout Device::createDescriptorSetLayout(
        const vk::DescriptorSetLayoutCreateInfo & dslci)
    {
        auto h = getHash(dslci);
        if (descriptorSetLayouts_.contains(h)) {
            return descriptorSetLayouts_[h];
        }

        auto layout = handle_.createDescriptorSetLayout(dslci);
        //std::make_shared<RXCore::DescriptorSetLayout>(handle_, handle_.createDescriptorSetLayout(dslci));
        descriptorSetLayouts_.emplace(h, layout);

        return layout;
    }

    RxUtil::Hash Device::getHash(const vk::DescriptorSetLayoutCreateInfo & dslci) const
    {
        RxUtil::Hasher h;

        h.u32(dslci.bindingCount);
        h.u32(static_cast<uint32_t>(dslci.flags));
        for (uint32_t i = 0; i < dslci.bindingCount; i++) {
            auto & binding = dslci.pBindings[i];
            h.u32(binding.binding);
            h.u32(binding.descriptorCount);
            h.u32(static_cast<uint32_t>(binding.descriptorType));
            h.u32(static_cast<uint32_t>(binding.stageFlags));

            if (binding.pImmutableSamplers &&
                (binding.descriptorType == vk::DescriptorType::eCombinedImageSampler ||
                    binding.descriptorType == vk::DescriptorType::eSampler)) {
                for (uint32_t j = 0; j < binding.descriptorCount; j++) {
                    //RxUtil::Hasher hash;
                    auto hh = getHashForSampler(binding.pImmutableSamplers[j]);
                    h.u64(hh);
                }
            }
        }

        if (dslci.pNext) {
            auto h2 = getPNextHash(dslci.pNext);
            h.u64(h2);
        }
        return h.get();
    }

    RxUtil::Hash Device::getHashForSampler(vk::Sampler sampler) const
    {
        for (auto & s: samplers_) {
            if (s.second == sampler) {
                return s.first;
            }
        }
        assert(false);
        return 0;
    }

    RxUtil::Hash Device::getHashForDescriptorSetLayout(vk::DescriptorSetLayout dsl) const
    {
        for (auto & s: descriptorSetLayouts_) {
            if (s.second == dsl) {
                return s.first;
            }
        }
        assert(false);
        return 0;
    }

    RxUtil::Hash Device::getHash(const vk::PipelineLayoutCreateInfo & plci) const
    {
        RxUtil::Hasher h;

        h.u32(plci.setLayoutCount);
        for (uint32_t i = 0; i < plci.setLayoutCount; i++) {
            if (plci.pSetLayouts[i]) {
                RxUtil::Hash hash = getHashForDescriptorSetLayout(plci.pSetLayouts[i]);
                h.u64(hash);
            } else {
                h.u32(0);
            }
        }

        h.u32(plci.pushConstantRangeCount);
        for (uint32_t i = 0; i < plci.pushConstantRangeCount; i++) {
            auto & push = plci.pPushConstantRanges[i];
            h.u32(static_cast<uint32_t>(push.stageFlags));
            h.u32(push.size);
            h.u32(push.offset);
        }

        h.u32(static_cast<uint32_t>(plci.flags));

        return h.get();
    }

    RxUtil::Hash Device::getHash(
        const vk::DescriptorSetLayoutBindingFlagsCreateInfo & dslbfci) const
    {
        RxUtil::Hasher h;
        h.u32(dslbfci.bindingCount);
        for (uint32_t i = 0; i < dslbfci.bindingCount; i++) {
            h.u32(static_cast<uint32_t>(dslbfci.pBindingFlags[i]));
        }
        return h.get();
    }

    RxUtil::Hash Device::getPNextHash(const void * pNext) const
    {
        RxUtil::Hasher h;
        auto v = reinterpret_cast<const vk::BaseInStructure *>(pNext);

        h.u32(static_cast<uint32_t>(v->sType));
        RxUtil::Hash next_hash;

        switch (v->sType) {
        case vk::StructureType::eDescriptorSetLayoutBindingFlagsCreateInfo:
            next_hash = getHash(
                *static_cast<const vk::DescriptorSetLayoutBindingFlagsCreateInfo *>(pNext));
            h.u64(next_hash);
            break;
        default:
            next_hash = 0;
            break;
        }

        return h.get();
    }

    void Device::destroyDescriptorPool(DescriptorPool * pool)
    {
        handle_.destroyDescriptorPool(pool->handle);
    }

    void Device::freeCommandBuffer(CommandBuffer * buf)
    {
        handle_.freeCommandBuffers(buf->commandPool_->GetHandle(), 1, &buf->handle_);
    }

    void Device::getSurfaceDetails()
    {
        auto result = physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface_, &capabilities_);
        if(result != vk::Result::eSuccess) {
            throw std::exception("Unable to get surface capabilities");
        }

        formats_ = physicalDevice->GetHandle().getSurfaceFormatsKHR(surface_);
        presentationModes_ = physicalDevice->GetHandle().getSurfacePresentModesKHR(surface_);
    }

    void Device::updateSurfaceCapabilities()
    {
        auto result = physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(surface_, &capabilities_);
        if(result != vk::Result::eSuccess) {
            throw std::exception("Unable to get surface capabilities");
        }
    }

    void Device::selectSurfaceFormat()
    {
        vk::SurfaceFormatKHR selected_format = vk::Format::eUndefined;

        for (auto& f : formats_) {
            if (f.colorSpace == vk::ColorSpaceKHR::eSrgbNonlinear && f.format == vk::Format::eB8G8R8A8Unorm) {
                selected_format = f.format;
                break;
            }
        }
        if (selected_format == vk::Format::eUndefined) {
            selected_format = formats_[0];
        }
        selectedFormat_ = selected_format.format;
        selectedColorSpace_ = selected_format.colorSpace;
    }

    void Device::selectPresentationMode()
    {
        /*
              * FIFO by default since it is always support, but Mailbox if it is there
              */
        selectedPresentationMode_ = vk::PresentModeKHR::eFifo;

        for (auto& pm : presentationModes_) {
            if (pm == vk::PresentModeKHR::eMailbox) {
                selectedPresentationMode_ = pm;
                break;
            }
        }
    }

    void Device::selectPresentationQueueFamily()
    {
        auto qfps = physicalDevice->GetHandle().getQueueFamilyProperties();

        /*
         * Find a shared Queue Family first
         */
        for (uint32_t i = 0; i < qfps.size(); i++) {
            vk::Bool32 s = physicalDevice->GetHandle().getSurfaceSupportKHR(i, surface_);
            if (s) {
                presentQueueFamily_ = i;
                auto cqf = physicalDevice->GetGraphicsQueueFamily();

                if (cqf == i) {
                    exclusiveQueueSupport_ = true;
                    presentQueueFamily_ = i;
                    break;
                }
            }
        }

        /*
         * Otherwise just take any presentation queue family
         */
        if (!exclusiveQueueSupport_) {
            for (uint32_t i = 0; i < qfps.size(); i++) {
                const vk::Bool32 s = physicalDevice->GetHandle().getSurfaceSupportKHR(i, surface_);
                if (s) {
                    presentQueueFamily_ = i;
                    break;
                }
            }
        }
    }

    std::unique_ptr<SwapChain> Device::createSwapChain()
    {
        uint32_t image_count = std::max(2u, capabilities_.minImageCount + 1);
        //auto x1 = capabilities_.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eOpaque;

        vk::SwapchainCreateInfoKHR ci = {
            {},
            surface_,
            image_count,
            selectedFormat_,
            selectedColorSpace_,
            capabilities_.currentExtent,
            1,
            vk::ImageUsageFlagBits::eColorAttachment,
            vk::SharingMode::eExclusive,
            uint32_t(0),
            nullptr,
            capabilities_.currentTransform,
            vk::CompositeAlphaFlagBitsKHR::eOpaque,
            selectedPresentationMode_,
            true,
            nullptr
        };

        auto sc = handle_.createSwapchainKHR(ci);
        auto swo = std::make_unique<SwapChain>(
            this,
            image_count,
            sc,
            selectedFormat_,
            capabilities_.currentExtent);

        return swo;
    }

    uint32_t Device::getPresentQueueFamily() const
    {
        return presentQueueFamily_.value();
    }
} // namespace RXCore
