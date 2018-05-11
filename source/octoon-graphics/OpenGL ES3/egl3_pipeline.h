#ifndef OCTOON_EGL3_RENDER_PIPELINE_H_
#define OCTOON_EGL3_RENDER_PIPELINE_H_

#include "egl3_types.h"

namespace octoon
{
	namespace graphics
	{
		class EGL3Pipeline final : public GraphicsPipeline
		{
			OctoonDeclareSubClass(EGL3Pipeline, GraphicsPipeline)
		public:
			EGL3Pipeline() noexcept;
			virtual ~EGL3Pipeline() noexcept;

			bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
			void close() noexcept;

			void bindVertexBuffers(EGL3VertexBuffers& vbos, bool forceUpdate) noexcept;

			void apply() noexcept;

			const GraphicsPipelineDesc& getGraphicsPipelineDesc() const noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL3Pipeline(const EGL3Pipeline&) noexcept = delete;
			EGL3Pipeline& operator=(const EGL3Pipeline&) noexcept = delete;

		private:
			struct VertexAttrib
			{
				GLenum type;
				GLuint index;
				GLuint slot;
				GLuint count;
				GLboolean normalize;
				GLintptr offset;
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