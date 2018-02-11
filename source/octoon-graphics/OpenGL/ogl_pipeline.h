#ifndef OCTOON_OGL_PIPELINE_H_
#define OCTOON_OGL_PIPELINE_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLPipeline final : public GraphicsPipeline
		{
			OctoonDeclareSubClass(OGLPipeline, GraphicsPipeline)
		public:
			OGLPipeline() noexcept;
			virtual ~OGLPipeline() noexcept;

			bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
			void close() noexcept;

			void bindVertexBuffers(OGLVertexBuffers& vbos, bool forceUpdate) noexcept;
			void apply() noexcept;

			const GraphicsPipelineDesc& getGraphicsPipelineDesc() const noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLPipeline(const OGLPipeline&) noexcept = delete;
			OGLPipeline& operator=(const OGLPipeline&) noexcept = delete;

		private:
			struct VertexAttrib
			{
				GLenum type;
				GLuint index;
				GLuint count;
				GLuint stride;
				GLboolean normalize;
				GLintptr offset;
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