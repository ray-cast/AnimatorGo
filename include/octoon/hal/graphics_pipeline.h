#ifndef OCTOON_GRAPHICS_PIPELINE_H_
#define OCTOON_GRAPHICS_PIPELINE_H_

#include <octoon/hal/render_state.h>

namespace octoon
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

		void setProgram(GraphicsProgramPtr program) noexcept;
		GraphicsProgramPtr getProgram() const noexcept;

		void setInputLayout(GraphicsInputLayoutPtr inputLayout) noexcept;
		GraphicsInputLayoutPtr getInputLayout() const noexcept;

		void setDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr descriptorSet) noexcept;
		GraphicsDescriptorSetLayoutPtr getDescriptorSetLayout() const noexcept;

		void setFramebufferLayout(GraphicsFramebufferLayoutPtr framebufferLayout) noexcept;
		GraphicsFramebufferLayoutPtr getFramebufferLayout() const noexcept;

		void setRenderState(std::shared_ptr<RenderState> state) noexcept;
		std::shared_ptr<RenderState> getGraphicsState() const noexcept;

	private:
		std::shared_ptr<RenderState> _state;
		GraphicsProgramPtr _program;
		GraphicsInputLayoutPtr _inputLayout;
		GraphicsFramebufferLayoutPtr _framebufferLayout;
		GraphicsDescriptorSetLayoutPtr _descriptorSetLayout;
	};

	class OCTOON_EXPORT GraphicsPipelineLayout : public GraphicsResource
	{
		OctoonDeclareSubInterface(GraphicsPipelineLayout, GraphicsResource)
	public:
		GraphicsPipelineLayout() noexcept = default;
		virtual ~GraphicsPipelineLayout() = default;

		virtual const GraphicsPipelineLayoutDesc& getPipelineLayoutDesc() const noexcept = 0;

	private:
		GraphicsPipelineLayout(const GraphicsPipelineLayout&) noexcept = delete;
		GraphicsPipelineLayout& operator=(const GraphicsPipelineLayout&) noexcept = delete;
	};

	class OCTOON_EXPORT GraphicsPipeline : public GraphicsResource
	{
		OctoonDeclareSubInterface(GraphicsPipeline, GraphicsResource)
	public:
		GraphicsPipeline() noexcept = default;
		virtual ~GraphicsPipeline() = default;

		virtual const GraphicsPipelineDesc& getPipelineDesc() const noexcept = 0;

	private:
		GraphicsPipeline(const GraphicsPipeline&) noexcept = delete;
		GraphicsPipeline& operator=(const GraphicsPipeline&) noexcept = delete;
	};
}

#endif