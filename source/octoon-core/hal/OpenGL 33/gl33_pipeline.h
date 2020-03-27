#ifndef OCTOON_GL33_PIPELINE_H_
#define OCTOON_GL33_PIPELINE_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL33Pipeline final : public GraphicsPipeline
		{
			OctoonDeclareSubClass(GL33Pipeline, GraphicsPipeline)
		public:
			GL33Pipeline() noexcept;
			virtual ~GL33Pipeline() noexcept;

			bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
			void close() noexcept;

			void bindVertexBuffers(GL33VertexBuffers& vbos, bool forceUpdate) noexcept;
			void apply() noexcept;

			const GraphicsPipelineDesc& getPipelineDesc() const noexcept override;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL33Pipeline(const GL33Pipeline&) noexcept = delete;
			GL33Pipeline& operator=(const GL33Pipeline&) noexcept = delete;

		private:
			struct VertexAttrib
			{
				GLenum type;
				GLuint index;
				GLuint count;
				GLuint stride;
				GLboolean normalize;
				GLuint offset;
			};

			struct VertexBinding
			{
				GLuint slot;
				GLuint index;
				GLuint divisor;
			};

			std::vector<std::vector<VertexAttrib>> _attributes;
			std::vector<VertexBinding> _bindings;

			GraphicsPipelineDesc _pipelineDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif