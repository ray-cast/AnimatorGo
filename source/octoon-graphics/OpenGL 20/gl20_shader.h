#ifndef OCTOON_GL20_SHADER_H_
#define OCTOON_GL20_SHADER_H_

#include "gl20_types.h"

namespace octoon 
{
	namespace graphics
	{
		class GL20GraphicsAttribute final : public GraphicsAttribute
		{
			OctoonDeclareSubClass(GL20GraphicsAttribute, GraphicsAttribute)
		public:
			GL20GraphicsAttribute() noexcept;
			~GL20GraphicsAttribute() noexcept;

			void setSemantic(const std::string& semantic) noexcept;
			const std::string& getSemantic() const noexcept;

			void setSemanticIndex(std::uint32_t index) noexcept;
			std::uint32_t getSemanticIndex() const noexcept;

			void setType(GraphicsFormat type) noexcept;
			GraphicsFormat getType() const noexcept;

			void setBindingPoint(std::uint32_t bindingPoint) noexcept;
			std::uint32_t getBindingPoint() const noexcept;

		private:
			GL20GraphicsAttribute(const GL20GraphicsAttribute&) noexcept = delete;
			GL20GraphicsAttribute& operator=(const GL20GraphicsAttribute&) noexcept = delete;

		private:
			std::string _semantic;
			std::uint8_t _semanticIndex;
			std::uint32_t _bindingPoint;
			GraphicsFormat _type;
		};

		class GL20GraphicsUniform final : public GraphicsUniform
		{
			OctoonDeclareSubClass(GL20GraphicsUniform, GraphicsUniform)
		public:
			GL20GraphicsUniform() noexcept;
			~GL20GraphicsUniform() noexcept;

			void setName(const std::string& name) noexcept;
			const std::string& getName() const noexcept;

			void setSamplerName(const std::string& name) noexcept;
			const std::string& getSamplerName() const noexcept;

			void setType(GraphicsUniformType type) noexcept;
			GraphicsUniformType getType() const noexcept;

			void setOffset(std::uint32_t offset) noexcept;
			std::uint32_t getOffset() const noexcept;

			void setBindingPoint(GLuint bindingPoint) noexcept;
			GLuint getBindingPoint() const noexcept;

			void setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept;
			GraphicsShaderStageFlags getShaderStageFlags() const noexcept;

		private:
			GL20GraphicsUniform(const GL20GraphicsUniform&) noexcept = delete;
			GL20GraphicsUniform& operator=(const GL20GraphicsUniform&) noexcept = delete;

		private:
			std::string _name;
			std::string _samplerName;
			std::uint32_t _offset;
			GLuint _bindingPoint;
			GraphicsUniformType _type;
			GraphicsShaderStageFlags _stageFlags;
		};

		class GL20Shader final : public GraphicsShader
		{
			OctoonDeclareSubClass(GL20Shader, GraphicsShader)
		public:
			GL20Shader() noexcept;
			~GL20Shader() noexcept;

			bool setup(const GraphicsShaderDesc& desc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsShaderDesc& getGraphicsShaderDesc() const noexcept;

		private:
			static bool HlslCodes2GLSL(GraphicsShaderStageFlags stage, const std::string& codes, std::string& out);
			static bool HlslByteCodes2GLSL(GraphicsShaderStageFlags stage, const char* codes, std::string& out);

		private:
			friend class GL20Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GL20Shader(const GL20Shader&) noexcept = delete;
			GL20Shader& operator=(const GL20Shader&) noexcept = delete;

		private:
			GLuint _instance;
			GraphicsDeviceWeakPtr _device;
			GraphicsShaderDesc _shaderDesc;
		};

		class GL20Program final : public GraphicsProgram
		{
			OctoonDeclareSubClass(GL20Program, GraphicsProgram)
		public:
			GL20Program() noexcept;
			~GL20Program() noexcept;

			bool setup(const GraphicsProgramDesc& desc) noexcept;
			void close() noexcept;

			void apply() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsParams& getActiveParams() const noexcept;
			const GraphicsAttributes& getActiveAttributes() const noexcept;

			const GraphicsProgramDesc& getGraphicsProgramDesc() const noexcept;

		private:
			void _initActiveAttribute() noexcept;
			void _initActiveUniform() noexcept;

		private:
			static GraphicsFormat toGraphicsFormat(GLenum type) noexcept;
			static GraphicsUniformType toGraphicsUniformType(const std::string& name, GLenum type) noexcept;

		private:
			friend class GL20Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			GL20Program(const GL20Program&) noexcept = delete;
			GL20Program& operator=(const GL20Program&) noexcept = delete;

		private:

			GLuint _program;

			GraphicsParams _activeParams;
			GraphicsAttributes  _activeAttributes;

			GraphicsDeviceWeakPtr _device;
			GraphicsProgramDesc _programDesc;
		};
	}
}

#endif