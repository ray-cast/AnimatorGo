#ifndef OCTOON_OGL_CORE_PIPELINE_H_
#define OCTOON_OGL_CORE_PIPELINE_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLCorePipeline final : public GraphicsPipeline
		{
			OctoonDeclareSubClass(OGLCorePipeline, GraphicsPipeline)
		public:
			OGLCorePipeline() noexcept;
			virtual ~OGLCorePipeline() noexcept;

			bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
			void close() noexcept;

			void bindVertexBuffers(OGLCoreVertexBuffers& vbos, bool forceUpdate) noexcept;

			void apply() noexcept;

			const GraphicsPipelineDesc& getGraphicsPipelineDesc() const noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLCorePipeline(const OGLCorePipeline&) noexcept = delete;
			OGLCorePipeline& operator=(const OGLCorePipeline&) noexcept = delete;

		private:
			struct VertexAttrib
			{
				GLenum type;
				GLuint index;
				GLuint slot;
				GLuint count;
				GLuint offset;
				GLboolean normalize;
			};

			struct VertexBinding
			{
				GLuint slot;
				GLuint divisor;
				GLuint stride;
			};

			std::vector<VertexAttrib> _attributes;
			std::vector<VertexBinding> _bindings;

			GraphicsPipelineDesc _pipelineDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif