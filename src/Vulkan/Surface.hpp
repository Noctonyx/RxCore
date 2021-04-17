//
// Copyright (c) 2020 - Shane Hyde (shane@noctonyx.com)
//

#pragma once

#include "Vulk.hpp"
#include "Instance.hpp"
//#include "SwapChain.hpp"
//#include "../UI/Window.hpp"
#include <optional>

namespace RxCore
{
    class PhysicalDevice;
    class Device;

    class Surface
    {
        friend class SwapChain;
        friend class Renderer;
        friend class Device;

    public:
        Surface(vk::SurfaceKHR handle);
        ~Surface();

        std::unique_ptr<SwapChain> CreateSwapChain();

        void updateSurfaceCapabilities();

    protected:
        void getSurfaceDetails();
        void selectFormat();
        void selectPresentationMode();
        void selectPresentationQueueFamily();

    public:
        vk::SurfaceKHR handle;

    private:
        std::optional<uint32_t> presentQueueFamily_;
        bool exclusiveQueueSupport_{};

        vk::SurfaceCapabilitiesKHR capabilities_;
        std::vector<vk::SurfaceFormatKHR> formats_;
        std::vector<vk::PresentModeKHR> presentationModes_;
        vk::Format selectedFormat_;
        vk::ColorSpaceKHR selectedColorSpace_;
        vk::PresentModeKHR selectedPresentationMode_;
    };
}
