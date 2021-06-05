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
    SwapChain::SwapChain(
        Device * device,
        uint32_t image_count,
        vk::SwapchainKHR h,
        vk::Format format,
        vk::Extent2D ex)
        : device_(device)
        , handle(h)
        , imageCount_(image_count)
        , extent_(ex)
        , imageFormat_(format) { createResources(); }

    SwapChain::~SwapChain()
    {
        //const auto device = Device::VkDevice();

        for (uint32_t i = 0; i < imageCount_; i++) {
            device_->getDevice().destroySemaphore(imageReadySemaphores_[i]);
            //device.destroySemaphore(swapChainState_[i].renderFinishedSemaphore);
            device_->getDevice().destroyImageView(swapChainState_[i].imageView);
            //m_Device.GetVK().destroyImage(m_SwapChainState[i].m_Image);
        }

        device_->getDevice().destroySwapchainKHR(handle);
    }

    void SwapChain::createResources()
    {
        //const auto device = Device::VkDevice();

        swapChainState_.resize(imageCount_);
        auto is = device_->getDevice().getSwapchainImagesKHR(handle);
        for (uint32_t x = 0; x < is.size(); x++) {
            swapChainState_[x].image = is[x];
            swapChainState_[x].imageView = device_->getDevice().createImageView(
                {
                    {},
                    swapChainState_[x].image,
                    vk::ImageViewType::e2D,
                    imageFormat_,
                    {
                        vk::ComponentSwizzle::eIdentity,
                        vk::ComponentSwizzle::eIdentity,
                        vk::ComponentSwizzle::eIdentity,
                        vk::ComponentSwizzle::eIdentity,
                    },
                    {
                        vk::ImageAspectFlagBits::eColor,
                        0,
                        1,
                        0,
                        1
                    }
                }
            );
        }
        imageReadySemaphores_.resize(imageCount_);
        imageReadySemaphoreIndex_ = 0;

        for (uint32_t i = 0; i < imageCount_; i++) {
            imageReadySemaphores_[i] = device_->getDevice().createSemaphore({});
            //swapChainState_[i].renderFinishedSemaphore = device.createSemaphore({});
            //m_SwapChainState[i].m_InFlightFence = m_Device.GetVK().createFence({vk::FenceCreateFlagBits::eSignaled});
        }

        device_->getDevice().getQueue(
            device_->getPresentQueueFamily(),
            0,
            &presentQueue_);
    }

    std::tuple<vk::ImageView, vk::Semaphore, uint32_t> SwapChain::AcquireNextImage()
    {
        OPTICK_EVENT()
        std::tuple<vk::ImageView, vk::Semaphore, uint32_t> result;

        vk::Semaphore sem = imageReadySemaphores_[imageReadySemaphoreIndex_];
        std::get<1>(result) = sem;
        imageReadySemaphoreIndex_ = (imageReadySemaphoreIndex_ + 1) % imageCount_;

        // *semaphore = m_SwapChainState[m_CurrentImage].m_PresentCompleteSemaphore;

        auto r =  device_->getDevice().acquireNextImageKHR(
            handle,
            std::numeric_limits<uint64_t>::max(),
            sem,
            nullptr,
            &currentImage_
        );
        //        m_Device.GetVK().resetFences(1, &(m_SwapChainState[m_CurrentImage].m_InFlightFence));
        //std::get<2>(result) = swapChainState_[currentImage_].renderFinishedSemaphore;
        std::get<2>(result) = currentImage_;

        if (r == vk::Result::eSuboptimalKHR) {
            spdlog::info("Swapchain out of date");
            swapChainOutOfDate_ = true;
        }
        if (r == vk::Result::eErrorOutOfDateKHR) {
            spdlog::info("Swapchain out of date");
            swapChainOutOfDate_ = true;
            std::get<0>(result) = nullptr;
            return result;
        }
        std::get<0>(result) = swapChainState_[currentImage_].imageView;
        return result;
    }

    void SwapChain::PresentImage(vk::ImageView imageView, vk::Semaphore renderComplete)
    {
        try {
            for (uint32_t i = 0; i < swapChainState_.size(); ++i) {
                if (swapChainState_[i].imageView == imageView) {
                    auto r = presentQueue_.presentKHR(
                        {
                            1,
                            &renderComplete,
                            1,
                            &handle,
                            &i
                        });
                    assert(r == vk::Result::eSuccess);
                    if (r != vk::Result::eSuccess) {
                        throw std::exception("Unable to present");
                    }

                    break;
                }
            }
        }
        catch (vk::OutOfDateKHRError &) { swapChainOutOfDate_ = true; }
    }
}
