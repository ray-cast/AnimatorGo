#ifndef OCTOON_GRAPHICS_PIPELINE_H_
#define OCTOON_GRAPHICS_PIPELINE_H_

#include <octoon/hal/graphics_child.h>

namespace octoon
{
	namespace graphics
	{
		class OCTOON_EXPORT GraphicsPipelineLayoutDesc final
		{
		public:
			GraphicsPipelineLayoutDesc() noexcept;
			~GraphicsPipelineLayoutDesc() noexcept;

			void setGraphicsDescriptorSetLayout(const GraphicsDescriptorSetLayouts& descriptorSetLayouts) noexcept;
			const GraphicsDescriptorSetLayouts& getGraphicsDescriptorSetLayouts() const noexcept;

			void addGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept;
			void removeGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept;

		private:
			GraphicsDescriptorSetLayouts _descriptorSetLayouts;
		};

		class OCTOON_EXPORT GraphicsPipelineDesc final
		{
		public:
			GraphicsPipelineDesc() noexcept = default;
			~GraphicsPipelineDesc() = default;

			void setGraphicsProgram(GraphicsProgramPtr program) noexcept;
			GraphicsProgramPtr getGraphicsProgram() const noexcept;

			void setGraphicsInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept;
			GraphicsInputLayoutPtr getInputLayout() const noexcept;

			void setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr descriptorSet) noexcept;
			GraphicsDescriptorSetLayoutPtr getDescriptorSetLayout() const noexcept;

			void setGraphicsFramebufferLayout(GraphicsFramebufferLayoutPtr framebufferLayout) noexcept;
			GraphicsFramebufferLayoutPtr getFramebufferLayout() const noexcept;

			void setGraphicsState(GraphicsStatePtr state) noexcept;
			GraphicsStatePtr getGraphicsState() const noexcept;

		private:
			GraphicsStatePtr _state;
			GraphicsProgramPtr _program;
			GraphicsInputLayoutPtr _inputLayout;
			GraphicsFramebufferLayoutPtr _framebufferLayout;
			GraphicsDescriptorSetLayoutPtr _descriptorSetLayout;
		};

		class OCTOON_EXPORT GraphicsPipelineLayout : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsPipelineLayout, GraphicsChild)
		public:
			GraphicsPipelineLayout() noexcept = default;
			virtual ~GraphicsPipelineLayout() = default;

			virtual const GraphicsPipelineLayoutDesc& getPipelineLayoutDesc() const noexcept = 0;

		private:
			GraphicsPipelineLayout(const GraphicsPipelineLayout&) noexcept = delete;
			GraphicsPipelineLayout& operator=(const GraphicsPipelineLayout&) noexcept = delete;
		};

		class OCTOON_EXPORT GraphicsPipeline : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsPipeline, GraphicsChild)
		public:
			GraphicsPipeline() noexcept = default;
			virtual ~GraphicsPipeline() = default;

			virtual const GraphicsPipelineDesc& getPipelineDesc() const noexcept = 0;

		private:
			GraphicsPipeline(const GraphicsPipeline&) noexcept = delete;
			GraphicsPipeline& operator=(const GraphicsPipeline&) noexcept = delete;
		};
	}
}

#endif