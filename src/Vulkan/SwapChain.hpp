#pragma once

#include "Device.h"

namespace RxCore
{
#if 0
    class SwapChain
    {
        friend class RenderPass;
        friend class Device;

        struct SwapChainState
        {
            //VkSemaphore presentCompleteSemaphore;
            //VkSemaphore renderFinishedSemaphore;
            VkImage image;
            VkImageView imageView;
        };

    public:
        SwapChain( Device * device,
            uint32_t image_count,
            VkSwapchainKHR h,
            VkFormat format,
            VkExtent2D ex);
        ~SwapChain();

        SwapChain(const SwapChain & other) = delete;
        SwapChain & operator=(const SwapChain & other) = delete;

        std::tuple<VkImageView, VkSemaphore, uint32_t> AcquireNextImage();
        void PresentImage(VkImageView imageView, VkSemaphore renderComplete);

        [[nodiscard]] VkExtent2D GetExtent() const
        {
            return extent_;
        }

        const VkSwapchainKHR handle;

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
        VkExtent2D extent_;
        VkFormat imageFormat_;
    public:
        [[nodiscard]] VkFormat imageFormat() const
        {
            return imageFormat_;
        }

    private:
        VkQueue presentQueue_;
        std::vector<SwapChainState> swapChainState_;
        std::vector<VkSemaphore> imageReadySemaphores_;
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
#endif
}
