#ifndef OCTOON_EGL2_PIPELINE_H_
#define OCTOON_EGL2_PIPELINE_H_

#include "egl2_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL2Pipeline final : public GraphicsPipeline
		{
			OctoonDeclareSubClass(EGL2Pipeline, GraphicsPipeline)
		public:
			EGL2Pipeline() noexcept;
			virtual ~EGL2Pipeline() noexcept;

			bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
			void close() noexcept;

			void bindVertexBuffers(EGL2VertexBuffers& vbos, bool forceUpdate) noexcept;

			void apply() noexcept;

			const GraphicsPipelineDesc& getGraphicsPipelineDesc() const noexcept;

		private:
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL2Pipeline(const EGL2Pipeline&) noexcept = delete;
			EGL2Pipeline& operator=(const EGL2Pipeline&) noexcept = delete;

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

			GraphicsProgramPtr _program;
			GraphicsPipelineDesc _pipelineDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif