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

//
// Created by shane on 5/06/2021.
//

#include "RxCore.h"
#include "Vulkan/SwapChain.hpp"
#include "Vulkan/Image.hpp"

namespace RxApi
{
    bool SwapChain::swapChainOutOfDate() const
    {
        return pImpl->swapChainOutOfDate();
    }

    void SwapChain::setSwapChainOutOfDate(bool status)
    {
        pImpl->setSwapChainOutOfDate(status);
    }

    Extent SwapChain::getExtent() const
    {
        auto e = pImpl->GetExtent();
        return Extent{e.width, e.height};
    }

    std::tuple<RxApi::ImageViewPtr, RxApi::Semaphore, uint32_t> SwapChain::acquireNextImage()
    {
        auto[i, s, x] = pImpl->AcquireNextImage();
        RxCore::ImageView iv()
        VkSemaphore ss = s;

        return std::tuple{std::make_shared<RxApi::ImageView>(i), {ss}, x};
    }

    ImageView::ImageView(std::shared_ptr<RxCore::ImageView> pImpl) : pImpl(pImpl)
    {

    }
}