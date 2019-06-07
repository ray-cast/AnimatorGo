#include <octoon/hal/graphics_pipeline.h>

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubInterface(GraphicsPipeline, GraphicsChild, "GraphicsPipeline")
		OctoonImplementSubInterface(GraphicsPipelineLayout, GraphicsChild, "GraphicsPipelineLayout")

		GraphicsPipelineLayoutDesc::GraphicsPipelineLayoutDesc() noexcept
		{
		}

		GraphicsPipelineLayoutDesc::~GraphicsPipelineLayoutDesc() noexcept
		{
		}

		void
		GraphicsPipelineLayoutDesc::setGraphicsDescriptorSetLayout(const GraphicsDescriptorSetLayouts& descriptorSetLayout) noexcept
		{
			_descriptorSetLayouts = descriptorSetLayout;
		}

		const GraphicsDescriptorSetLayouts&
		GraphicsPipelineLayoutDesc::getGraphicsDescriptorSetLayouts() const noexcept
		{
			return _descriptorSetLayouts;
		}

		void
		GraphicsPipelineLayoutDesc::addGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept
		{
			auto it = std::find(_descriptorSetLayouts.begin(), _descriptorSetLayouts.end(), descriptorSetLayout);
			if (it == _descriptorSetLayouts.end())
				_descriptorSetLayouts.push_back(descriptorSetLayout);
		}

		void
		GraphicsPipelineLayoutDesc::removeGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr& descriptorSetLayout) noexcept
		{
			auto it = std::find(_descriptorSetLayouts.begin(), _descriptorSetLayouts.end(), descriptorSetLayout);
			if (it != _descriptorSetLayouts.end())
				_descriptorSetLayouts.erase(it);
		}

		void
		GraphicsPipelineDesc::setGraphicsProgram(GraphicsProgramPtr program) noexcept
		{
			_program = program;
		}

		GraphicsProgramPtr
		GraphicsPipelineDesc::getGraphicsProgram() const noexcept
		{
			return _program;
		}

		void
		GraphicsPipelineDesc::setGraphicsInputLayout(GraphicsInputLayoutPtr layout) noexcept
		{
			_inputLayout = layout;
		}

		GraphicsInputLayoutPtr
		GraphicsPipelineDesc::getInputLayout() const noexcept
		{
			return _inputLayout;
		}

		void
		GraphicsPipelineDesc::setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr descriptorSetLayout) noexcept
		{
			_descriptorSetLayout = descriptorSetLayout;
		}

		GraphicsDescriptorSetLayoutPtr
		GraphicsPipelineDesc::getDescriptorSetLayout() const noexcept
		{
			return _descriptorSetLayout;
		}

		void
		GraphicsPipelineDesc::setGraphicsFramebufferLayout(GraphicsFramebufferLayoutPtr framebufferLayout) noexcept
		{
			_framebufferLayout = framebufferLayout;
		}

		GraphicsFramebufferLayoutPtr
		GraphicsPipelineDesc::getFramebufferLayout() const noexcept
		{
			return _framebufferLayout;
		}

		void
		GraphicsPipelineDesc::setGraphicsState(GraphicsStatePtr state) noexcept
		{
			_state = state;
		}

		GraphicsStatePtr
		GraphicsPipelineDesc::getGraphicsState() const noexcept
		{
			return _state;
		}
	}
}