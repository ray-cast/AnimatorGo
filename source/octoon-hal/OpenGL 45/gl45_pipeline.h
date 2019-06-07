#ifndef OCTOON_GL33_CORE_PIPELINE_H_
#define OCTOON_GL33_CORE_PIPELINE_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL45Pipeline final : public GraphicsPipeline
		{
			OctoonDeclareSubClass(GL45Pipeline, GraphicsPipeline)
		public:
			GL45Pipeline() noexcept;
			virtual ~GL45Pipeline() noexcept;

			bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
			void close() noexcept;

			void bindVertexBuffers(GL45VertexBuffers& vbos, bool forceUpdate) noexcept;

			void apply() noexcept;

			const GraphicsPipelineDesc& getPipelineDesc() const noexcept override;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL45Pipeline(const GL45Pipeline&) noexcept = delete;
			GL45Pipeline& operator=(const GL45Pipeline&) noexcept = delete;

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