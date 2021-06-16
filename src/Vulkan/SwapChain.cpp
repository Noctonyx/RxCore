////////////////////////////////////////////////////////////////////////////////
// MIT License
//
// Copyright (c) 2021.  Shane Hyde (shane@noctonyx.com)
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

#include "Vulk.hpp"
#include "SwapChain.hpp"
#include <tuple>
#include <vector>

#include "Log.h"
#include "spdlog/spdlog.h"
#include <optick/optick.h>
#include "Device.h"

namespace RxCore
{
#if 0
    SwapChain::SwapChain(
        Device * device,
        uint32_t image_count,
        VkSwapchainKHR h,
        VkFormat format,
        VkExtent2D ex)
        : handle(h)
        , device_(device)
        , imageCount_(image_count)
        , extent_(ex)
        , imageFormat_(format)
    {
        createResources();
    }

    SwapChain::~SwapChain()
    {
        for (uint32_t i = 0; i < imageCount_; i++) {
            vkDestroySemaphore(device_->getDevice(), imageReadySemaphores_[i], nullptr);
            vkDestroyImageView(device_->getDevice(), swapChainState_[i].imageView, nullptr);
        }

        vkDestroySwapchainKHR(device_->getDevice(), handle, nullptr);
    }

    void SwapChain::createResources()
    {
        //const auto device = Device::VkDevice();

        swapChainState_.resize(imageCount_);
        uint32_t count;
        vkGetSwapchainImagesKHR(device_->getDevice(), handle, &count, nullptr);
        std::vector<VkImage> images;

        images.resize(count);
        vkGetSwapchainImagesKHR(device_->getDevice(), handle, &count, images.data());

        //auto is = device_->getDevice().getSwapchainImagesKHR(handle);
        for (uint32_t x = 0; x < images.size(); x++) {
            swapChainState_[x].image = images[x];

            VkImageViewCreateInfo ivci{};
            ivci.sType = VK_STRUCTURE_TYPE_IMAGE_VIEW_CREATE_INFO;
            ivci.image = swapChainState_[x].image;
            ivci.viewType = VK_IMAGE_VIEW_TYPE_2D;
            ivci.format = imageFormat_;
            ivci.components = { VK_COMPONENT_SWIZZLE_IDENTITY ,VK_COMPONENT_SWIZZLE_IDENTITY ,VK_COMPONENT_SWIZZLE_IDENTITY ,VK_COMPONENT_SWIZZLE_IDENTITY };
            ivci.subresourceRange = { VK_IMAGE_ASPECT_COLOR_BIT, 0, 1, 0, 1 };

            vkCreateImageView(device_->getDevice(), &ivci, nullptr, &(swapChainState_[x].imageView));
        }
        imageReadySemaphores_.resize(imageCount_);
        imageReadySemaphoreIndex_ = 0;

        for (uint32_t i = 0; i < imageCount_; i++) {
            imageReadySemaphores_[i] = device_->createSemaphore();
        }
        vkGetDeviceQueue(device_->getDevice(), device_->getPresentQueueFamily(), 0, &presentQueue_);
    }

    std::tuple<VkImageView, VkSemaphore, uint32_t> SwapChain::AcquireNextImage()
    {
        OPTICK_EVENT()
        std::tuple<VkImageView, VkSemaphore, uint32_t> result;

        VkSemaphore sem = imageReadySemaphores_[imageReadySemaphoreIndex_];
        std::get<1>(result) = sem;
        imageReadySemaphoreIndex_ = (imageReadySemaphoreIndex_ + 1) % imageCount_;

        // *semaphore = m_SwapChainState[m_CurrentImage].m_PresentCompleteSemaphore;

        VkResult r = vkAcquireNextImageKHR(device_->getDevice(), handle,
                                           std::numeric_limits<uint64_t>::max(),
                                           sem,
                                           nullptr,
                                           &currentImage_);

        //        m_Device.GetVK().resetFences(1, &(m_SwapChainState[m_CurrentImage].m_InFlightFence));
        //std::get<2>(result) = swapChainState_[currentImage_].renderFinishedSemaphore;
        std::get<2>(result) = currentImage_;

        if (r == VK_SUBOPTIMAL_KHR) {
            spdlog::info("Swapchain out of date");
            swapChainOutOfDate_ = true;
        }
        if (r == VK_ERROR_OUT_OF_DATE_KHR) {
            spdlog::info("Swapchain out of date");
            swapChainOutOfDate_ = true;
            std::get<0>(result) = nullptr;
            return result;
        }
        std::get<0>(result) = swapChainState_[currentImage_].imageView;
        return result;
    }

    void SwapChain::PresentImage(VkImageView imageView, VkSemaphore renderComplete)
    {
        for (uint32_t i = 0; i < swapChainState_.size(); ++i) {
            if (swapChainState_[i].imageView == imageView) {

                VkPresentInfoKHR pi{};
                pi.sType = VK_STRUCTURE_TYPE_DEVICE_GROUP_PRESENT_INFO_KHR;
                pi.waitSemaphoreCount = 1;
                pi.pWaitSemaphores = &renderComplete;
                pi.swapchainCount = 1;
                pi.pSwapchains = &handle;
                pi.pImageIndices = &i;

                auto r = vkQueuePresentKHR(presentQueue_, &pi);

                if (r == VK_ERROR_OUT_OF_DATE_KHR) {
                    swapChainOutOfDate_ = true;
                    return;
                }
                assert(r == VK_SUCCESS);

                if (r != VK_SUCCESS) {
                    throw std::exception("Unable to present");
                }

                break;
            }
        }
    }
#endif
}
