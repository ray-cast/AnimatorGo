#ifndef OCTOON_GL32_RENDER_PIPELINE_H_
#define OCTOON_GL32_RENDER_PIPELINE_H_

#include "gl32_types.h"

namespace octoon
{
	namespace hal
	{
		class GL32Pipeline final : public GraphicsPipeline
		{
			OctoonDeclareSubClass(GL32Pipeline, GraphicsPipeline)
		public:
			GL32Pipeline() noexcept;
			virtual ~GL32Pipeline() noexcept;

			bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
			void close() noexcept;

			void bindVertexBuffers(GL32VertexBuffers& vbos, bool forceUpdate) noexcept;

			void apply() noexcept;

			const GraphicsPipelineDesc& getPipelineDesc() const noexcept override;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL32Pipeline(const GL32Pipeline&) noexcept = delete;
			GL32Pipeline& operator=(const GL32Pipeline&) noexcept = delete;

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