//
// Copyright (c) 2020 - Shane Hyde (shane@noctonyx.com)
//

#ifndef AMX_PIPELINEBUILDER_HPP
#define AMX_PIPELINEBUILDER_HPP

#include <vector>
#include <memory>
#include "Vulk.hpp"
//#include "PipelineLayout.hpp"

namespace RxCore
{
    //class PipelineLayout;

    class Device;

    class RenderPass;
    class Shader;

    enum class EPipelineCullMode
    {
        None,
        Front,
        Back,
        FrontAndBack
    };

    enum class EPipelineFrontFaceMode
    {
        CounterClockwise,
        Clockwise
    };

    class Pipeline;

    class PipelineBuilder
    {
    public:
        explicit PipelineBuilder();

        void setLayout(vk::PipelineLayout layout)
        {
            gpci.setLayout(layout);
            //pipelineLayout_ = layout;
        }

        void setRenderPass(vk::RenderPass rp)
        {
            gpci.setRenderPass(rp);
        }

        void setSubPass(uint32_t subpass)
        {
            gpci.setSubpass(subpass);
        }

        void addShader(
            vk::ShaderModule shader,
            const vk::ShaderStageFlagBits stageFlags,
            const char * main = "main"
        );

        void removeShaders();

        void setShader(
            uint32_t n,
            vk::ShaderModule shader,
            const vk::ShaderStageFlagBits stageFlags,
            const char * main = "main"
        );
        void AddAttachmentColorBlending(vk::PipelineColorBlendAttachmentState attachment);
        void AddAttachmentColorBlending(bool blend);

        void setDepthMode(bool read, bool write);
        [[maybe_unused]] void SetCullMode(EPipelineCullMode mode);
        [[maybe_unused]] void SetFrontFaceMode(EPipelineFrontFaceMode mode);
        void setDepthClampEnable(bool enable) {
            prsci.setDepthClampEnable(enable);
        }
        vk::Pipeline build();
        //void SetLayout(const std::shared_ptr<PipelineLayout>& layout);

        void setLineMode()
        {
            prsci.setPolygonMode(vk::PolygonMode::eLine);
        }

        void setVertexInputState(
            const std::vector<vk::VertexInputAttributeDescription> & attributeDescriptions,
            const std::vector<vk::VertexInputBindingDescription> & bindingDescriptions
        );

        void clearAttachments() {
            attachments_.clear();
        }

    private:
        vk::GraphicsPipelineCreateInfo gpci;
        vk::PipelineDynamicStateCreateInfo pdsci;
        vk::PipelineColorBlendStateCreateInfo pcbsci;
        vk::PipelineDepthStencilStateCreateInfo pdssci;
        vk::PipelineMultisampleStateCreateInfo pmsci;
        vk::PipelineRasterizationStateCreateInfo prsci;
        vk::PipelineViewportStateCreateInfo pvsci;
        vk::PipelineInputAssemblyStateCreateInfo piasci;
        vk::PipelineVertexInputStateCreateInfo pvisci;
        //std::shared_ptr<PipelineLayout> pipelineLayout_;
        std::vector<vk::PipelineShaderStageCreateInfo> shaderStages_;
        std::vector<vk::PipelineColorBlendAttachmentState> attachments_;
        std::vector<vk::DynamicState> dynamicStates_;
    };
}
#endif //AMX_PIPELINEBUILDER_HPP
