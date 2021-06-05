#include "Surface.hpp"
#include "PhysicalDevice.hpp"
#include <optional>
#include "SwapChain.hpp"

namespace RxCore
{
    Surface::Surface(Device * device, vk::SurfaceKHR handle)
        : device_(device)
        , handle(handle)
    {
        getSurfaceDetails();

        selectPresentationQueueFamily();
        selectFormat();
        selectPresentationMode();
    }

    Surface::~Surface()
    {
        device_->destroySurface(this);
    }

    void Surface::getSurfaceDetails()
    {
        auto result = Device::Context()->physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(handle, &capabilities_);
        if(result != vk::Result::eSuccess) {
            throw std::exception("Unable to get surface capabilities");
        }

        formats_ = Device::Context()->physicalDevice->GetHandle().getSurfaceFormatsKHR(handle);
        presentationModes_ = Device::Context()->physicalDevice->GetHandle().getSurfacePresentModesKHR(handle);
    }

    void Surface::updateSurfaceCapabilities()
    {
        auto result = Device::Context()->physicalDevice->GetHandle().getSurfaceCapabilitiesKHR(handle, &capabilities_);
        if(result != vk::Result::eSuccess) {
            throw std::exception("Unable to get surface capabilities");
        }
    }

    void Surface::selectFormat()
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

    void Surface::selectPresentationMode()
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

    void Surface::selectPresentationQueueFamily()
    {
        auto qfps = Device::Context()->physicalDevice->GetHandle().getQueueFamilyProperties();

        /*
         * Find a shared Queue Family first
         */
        for (uint32_t i = 0; i < qfps.size(); i++) {
            vk::Bool32 s = Device::Context()->physicalDevice->GetHandle().getSurfaceSupportKHR(i, handle);
            if (s) {
                presentQueueFamily_ = i;
                auto cqf = Device::Context()->physicalDevice->GetGraphicsQueueFamily();

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
                const vk::Bool32 s = Device::Context()->physicalDevice->GetHandle().getSurfaceSupportKHR(i, handle);
                if (s) {
                    presentQueueFamily_ = i;
                    break;
                }
            }
        }
    }

    std::unique_ptr<SwapChain> Surface::CreateSwapChain()
    {
        uint32_t image_count = std::max(2u, capabilities_.minImageCount + 1);
        //auto x1 = capabilities_.supportedCompositeAlpha & vk::CompositeAlphaFlagBitsKHR::eOpaque;

        vk::SwapchainCreateInfoKHR ci = {
            {},
            handle,
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

        auto sc = Device::VkDevice().createSwapchainKHR(ci);
        auto swo = std::make_unique<SwapChain>(
            image_count,
            sc,
            Device::Context()->surface->selectedFormat_,
            Device::Context()->surface->capabilities_.currentExtent);

        return swo;
    }
}
