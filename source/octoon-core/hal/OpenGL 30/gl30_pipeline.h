#ifndef OCTOON_GL30_RENDER_PIPELINE_H_
#define OCTOON_GL30_RENDER_PIPELINE_H_

#include "gl30_types.h"

namespace octoon
{
	namespace hal
	{
		class GL30Pipeline final : public GraphicsPipeline
		{
			OctoonDeclareSubClass(GL30Pipeline, GraphicsPipeline)
		public:
			GL30Pipeline() noexcept;
			virtual ~GL30Pipeline() noexcept;

			bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
			void close() noexcept;

			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

			void bindVertexBuffers(GL30VertexBuffers& vbos, bool forceUpdate, std::uint32_t startVertices = 0) noexcept;

			void apply() noexcept;

			const GraphicsPipelineDesc& getPipelineDesc() const noexcept override;

		private:
			GL30Pipeline(const GL30Pipeline&) noexcept = delete;
			GL30Pipeline& operator=(const GL30Pipeline&) noexcept = delete;

		private:
			struct VertexAttrib
			{
				GLenum type;
				GLuint index;
				GLuint count;
				GLuint stride;
				GLboolean normalize;
				GLuint size;
				GLuint offset;
			};

			struct VertexBinding
			{
				GLuint slot;
				GLuint index;
				GLuint divisor;
			};

			std::vector<VertexBinding> _bindings;
			std::vector<std::vector<VertexAttrib>> _attributes;

			GraphicsPipelineDesc _pipelineDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif