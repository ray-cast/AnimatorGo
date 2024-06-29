#include <octoon/hal/graphics_pipeline.h>

namespace octoon
{
	OctoonImplementSubInterface(GraphicsPipeline, GraphicsResource, "GraphicsPipeline")
	OctoonImplementSubInterface(GraphicsPipelineLayout, GraphicsResource, "GraphicsPipelineLayout")

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
	GraphicsPipelineDesc::setProgram(GraphicsProgramPtr program) noexcept
	{
		_program = program;
	}

	GraphicsProgramPtr
	GraphicsPipelineDesc::getProgram() const noexcept
	{
		return _program;
	}

	void
	GraphicsPipelineDesc::setInputLayout(GraphicsInputLayoutPtr layout) noexcept
	{
		_inputLayout = layout;
	}

	GraphicsInputLayoutPtr
	GraphicsPipelineDesc::getInputLayout() const noexcept
	{
		return _inputLayout;
	}

	void
	GraphicsPipelineDesc::setDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr descriptorSetLayout) noexcept
	{
		_descriptorSetLayout = descriptorSetLayout;
	}

	GraphicsDescriptorSetLayoutPtr
	GraphicsPipelineDesc::getDescriptorSetLayout() const noexcept
	{
		return _descriptorSetLayout;
	}

	void
	GraphicsPipelineDesc::setFramebufferLayout(GraphicsFramebufferLayoutPtr framebufferLayout) noexcept
	{
		_framebufferLayout = framebufferLayout;
	}

	GraphicsFramebufferLayoutPtr
	GraphicsPipelineDesc::getFramebufferLayout() const noexcept
	{
		return _framebufferLayout;
	}

	void
	GraphicsPipelineDesc::setRenderState(std::shared_ptr<RenderState> state) noexcept
	{
		_state = state;
	}

	std::shared_ptr<RenderState>
	GraphicsPipelineDesc::getGraphicsState() const noexcept
	{
		return _state;
	}
}