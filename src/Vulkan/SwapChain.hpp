#pragma once

#include "Device.h"

namespace RxCore
{
    class SwapChain
    {
        friend class RenderPass;
        friend class Device;

        struct SwapChainState
        {
            //vk::Semaphore presentCompleteSemaphore;
            //vk::Semaphore renderFinishedSemaphore;
            vk::Image image;
            vk::ImageView imageView;
        };

    public:
        SwapChain( Device * device,
            uint32_t image_count,
            vk::SwapchainKHR h,
            vk::Format format,
            vk::Extent2D ex);
        ~SwapChain();

        SwapChain(const SwapChain & other) = delete;
        SwapChain & operator=(const SwapChain & other) = delete;

        std::tuple<vk::ImageView, vk::Semaphore, uint32_t> AcquireNextImage();
        void PresentImage(vk::ImageView imageView, vk::Semaphore renderComplete);

        [[nodiscard]] vk::Extent2D GetExtent() const
        {
            return extent_;
        }

        const vk::SwapchainKHR handle;

    protected:
        void createResources();
    private:
        Device * device_;
        uint32_t imageCount_;
    public:
        [[nodiscard]] uint32_t imageCount() const
        {
            return imageCount_;
        }

    private:
        vk::Extent2D extent_;
        vk::Format imageFormat_;
    public:
        [[nodiscard]] vk::Format imageFormat() const
        {
            return imageFormat_;
        }

    private:
        vk::Queue presentQueue_;
        std::vector<SwapChainState> swapChainState_;
        std::vector<vk::Semaphore> imageReadySemaphores_;
        uint8_t imageReadySemaphoreIndex_;
        uint32_t currentImage_ = 0;
        bool swapChainOutOfDate_ = false;
    public:
        void setSwapChainOutOfDate(bool swapChainOutOfDate)
        {
            swapChainOutOfDate_ = swapChainOutOfDate;
        }

        [[nodiscard]] bool swapChainOutOfDate() const
        {
            return swapChainOutOfDate_;
        }
    };
}
