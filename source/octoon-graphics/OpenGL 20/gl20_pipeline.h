#ifndef OCTOON_GL20_PIPELINE_H_
#define OCTOON_GL20_PIPELINE_H_

#include "gl20_types.h"

namespace octoon 
{
	namespace graphics
	{
		class GL20Pipeline final : public GraphicsPipeline
		{
			OctoonDeclareSubClass(GL20Pipeline, GraphicsPipeline)
		public:
			GL20Pipeline() noexcept;
			virtual ~GL20Pipeline() noexcept;

			bool setup(const GraphicsPipelineDesc& pipelineDesc) noexcept;
			void close() noexcept;

			void bindVertexBuffers(GL20VertexBuffers& vbos, bool forceUpdate) noexcept;

			void apply() noexcept;

			const GraphicsPipelineDesc& getGraphicsPipelineDesc() const noexcept;

		private:
			friend class GL20Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GL20Pipeline(const GL20Pipeline&) noexcept = delete;
			GL20Pipeline& operator=(const GL20Pipeline&) noexcept = delete;

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