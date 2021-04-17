//
// Copyright (c) 2020 - Shane Hyde (shane@noctonyx.com)
//

#include <vector>
#include <memory>
#include "PipelineBuilder.hpp"
#include "Device.h"

namespace RxCore
{
    PipelineBuilder::PipelineBuilder()
    {
        piasci.setTopology(vk::PrimitiveTopology::eTriangleList)
              .setPrimitiveRestartEnable(false);

        pvsci.setViewportCount(1)
             .setPViewports(nullptr)
             .setScissorCount(1);

        prsci.setLineWidth(1.0f)
             .setPolygonMode(vk::PolygonMode::eFill)
             .setDepthClampEnable(false)
             .setRasterizerDiscardEnable(false)
             .setCullMode(vk::CullModeFlagBits::eBack)
             .setFrontFace(vk::FrontFace::eCounterClockwise);

        pmsci.setRasterizationSamples(vk::SampleCountFlagBits::e1);
        /*
                attachments_.resize(1);
                attachments_[0].setColorWriteMask(
                                   vk::ColorComponentFlagBits::eA |
                                   vk::ColorComponentFlagBits::eR |
                                   vk::ColorComponentFlagBits::eG |
                                   vk::ColorComponentFlagBits::eB
                               )
                               .setBlendEnable(true)
                               .setSrcColorBlendFactor(vk::BlendFactor::eSrcAlpha)
                               .setDstColorBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
                               .setColorBlendOp(vk::BlendOp::eAdd)
                               .setSrcAlphaBlendFactor(vk::BlendFactor::eOneMinusSrcAlpha)
                               .setDstAlphaBlendFactor(vk::BlendFactor::eZero)
                               .setAlphaBlendOp(vk::BlendOp::eAdd);
        */
        //pcbsci.setAttachments(attachments_);

        pdssci.setDepthTestEnable(false)
              .setDepthWriteEnable(false)
              .setDepthCompareOp(vk::CompareOp::eLessOrEqual)
              .setDepthBoundsTestEnable(false)
              .setStencilTestEnable(false)
              .setFront({vk::StencilOp::eKeep, vk::StencilOp::eKeep})
              .setBack({vk::StencilOp::eKeep, vk::StencilOp::eKeep})
              .setMinDepthBounds(0.0f)
              .setMaxDepthBounds(1.0f);

        dynamicStates_.push_back(vk::DynamicState::eViewport);
        dynamicStates_.push_back(vk::DynamicState::eScissor);

        //pdsci.setDynamicStates(dynamicStates_);

        gpci.setPInputAssemblyState(&piasci)
            .setPViewportState(&pvsci)
            .setPRasterizationState(&prsci)
            .setPMultisampleState(&pmsci)
            .setPDepthStencilState(&pdssci)
            .setPColorBlendState(&pcbsci)
            .setPVertexInputState(&pvisci)
            .setPDynamicState(&pdsci)
            .setLayout(nullptr)
            .setRenderPass(nullptr);
    }

    void PipelineBuilder::addShader(
        vk::ShaderModule shader,
        const vk::ShaderStageFlagBits stageFlags,
        const char * main
    )
    {
        vk::PipelineShaderStageCreateInfo & st = shaderStages_.emplace_back();

        st.setPName(main)
          .setStage(stageFlags)
          .setModule(shader);
    }

    void PipelineBuilder::setShader(
        uint32_t n,
        vk::ShaderModule shader,
        const vk::ShaderStageFlagBits stageFlags,
        const char * main
    )
    {
        shaderStages_[n].setPName(main)
                        .setStage(stageFlags)
                        .setModule(shader);
    }

    void PipelineBuilder::setDepthMode(bool read, bool write)
    {
        pdssci.setDepthWriteEnable(write);
        pdssci.setDepthTestEnable(read);
    }

    [[maybe_unused]] void PipelineBuilder::SetCullMode(EPipelineCullMode mode)
    {
        switch (mode) {
            case EPipelineCullMode::None:
                prsci.setCullMode(vk::CullModeFlagBits::eNone);
                break;
            case EPipelineCullMode::Front:
                prsci.setCullMode(vk::CullModeFlagBits::eFront);
                break;
            case EPipelineCullMode::Back:
                prsci.setCullMode(vk::CullModeFlagBits::eBack);
                break;
            case EPipelineCullMode::FrontAndBack:
                prsci.setCullMode(vk::CullModeFlagBits::eFrontAndBack);
                break;
        }
    }

    [[maybe_unused]] void PipelineBuilder::SetFrontFaceMode(EPipelineFrontFaceMode mode)
    {
        switch (mode) {
            case EPipelineFrontFaceMode::CounterClockwise:
                prsci.setFrontFace(vk::FrontFace::eCounterClockwise);
                break;
            case EPipelineFrontFaceMode::Clockwise:
                prsci.setFrontFace(vk::FrontFace::eClockwise);
                break;
        }
    }

    void PipelineBuilder::setVertexInputState(
        const std::vector<vk::VertexInputAttributeDescription> & attributeDescriptions,
        const std::vector<vk::VertexInputBindingDescription> & bindingDescriptions
    )
    {
        pvisci.setVertexBindingDescriptions(bindingDescriptions)
              .setVertexAttributeDescriptions(attributeDescriptions);
    }

    vk::Pipeline PipelineBuilder::build()
    {
        //gpci.setRenderPass(*renderPass)
        //    .setSubpass(subPass);

        pdsci.setDynamicStates(dynamicStates_);
        pcbsci.setAttachments(attachments_);

        gpci.setStages(shaderStages_);

        auto result_value = Device::VkDevice().createGraphicsPipeline(nullptr, gpci);
        assert(result_value.result == vk::Result::eSuccess);

        //for (auto & shader_stage: shaderStages_) { VulkanContext::VkDevice().destroyShaderModule(shader_stage.module); }
        //shaderStages_.clear();

        return result_value.value;
        //return std::make_shared<Pipeline>(result_value.value, gpci.layout);
    }

    void PipelineBuilder::AddAttachmentColorBlending(const vk::PipelineColorBlendAttachmentState attachment)
    {
        attachments_.emplace_back(attachment);
    }

    void PipelineBuilder::AddAttachmentColorBlending(bool blend)
    {
        attachments_.emplace_back(
            blend,
            vk::BlendFactor::eSrcAlpha,
            vk::BlendFactor::eOneMinusSrcAlpha,
            vk::BlendOp::eAdd,
            vk::BlendFactor::eOneMinusSrcAlpha,
            vk::BlendFactor::eZero,
            vk::BlendOp::eAdd,
            vk::ColorComponentFlagBits::eA |
            vk::ColorComponentFlagBits::eR |
            vk::ColorComponentFlagBits::eG |
            vk::ColorComponentFlagBits::eB
        );
    }

    void PipelineBuilder::removeShaders()
    {
        shaderStages_.clear();
    }
}
