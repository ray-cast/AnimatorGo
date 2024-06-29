#ifndef OCTOON_GL32_SHADER_H_
#define OCTOON_GL32_SHADER_H_

#include "gl32_types.h"

namespace octoon
{
	namespace hal
	{
		class GL32GraphicsAttribute final : public GraphicsAttribute
		{
			OctoonDeclareSubClass(GL32GraphicsAttribute, GraphicsAttribute)
		public:
			GL32GraphicsAttribute() noexcept;
			~GL32GraphicsAttribute() noexcept;

			void setSemantic(std::string_view semantic) noexcept;
			const std::string& getSemantic() const noexcept;

			void setSemanticIndex(std::uint8_t index) noexcept;
			std::uint32_t getSemanticIndex() const noexcept;

			void setType(GraphicsFormat type) noexcept;
			GraphicsFormat getType() const noexcept;

			void setBindingPoint(std::uint32_t bindingPoint) noexcept;
			std::uint32_t getBindingPoint() const noexcept;

		private:
			GL32GraphicsAttribute(const GL32GraphicsAttribute&) noexcept = delete;
			GL32GraphicsAttribute& operator=(const GL32GraphicsAttribute&) noexcept = delete;

		private:
			std::string _semantic;
			std::uint8_t _semanticIndex;
			std::uint32_t _bindingPoint;
			GraphicsFormat _type;
		};

		class GL32GraphicsUniform final : public GraphicsUniform
		{
			OctoonDeclareSubClass(GL32GraphicsUniform, GraphicsUniform)
		public:
			GL32GraphicsUniform() noexcept;
			~GL32GraphicsUniform() noexcept;

			void setName(std::string_view name) noexcept;
			const std::string& getName() const noexcept;

			void setSamplerName(std::string_view name) noexcept;
			const std::string& getSamplerName() const noexcept;

			void setType(UniformAttributeFormat type) noexcept;
			UniformAttributeFormat getType() const noexcept;

			void setOffset(std::uint32_t offset) noexcept;
			std::uint32_t getOffset() const noexcept;

			void setBindingPoint(GLuint bindingPoint) noexcept;
			GLuint getBindingPoint() const noexcept;

			void setShaderStageFlags(ShaderStageFlags flags) noexcept;
			ShaderStageFlags getShaderStageFlags() const noexcept;

		private:
			GL32GraphicsUniform(const GL32GraphicsUniform&) noexcept = delete;
			GL32GraphicsUniform& operator=(const GL32GraphicsUniform&) noexcept = delete;

		private:
			std::string _name;
			std::string _samplerName;
			std::uint32_t _offset;
			GLuint _bindingPoint;
			UniformAttributeFormat _type;
			ShaderStageFlags _stageFlags;
		};

		class GL32GraphicsUniformBlock final : public GraphicsUniformBlock
		{
			OctoonDeclareSubClass(GL32GraphicsUniformBlock, GraphicsUniformBlock)
		public:
			GL32GraphicsUniformBlock() noexcept;
			~GL32GraphicsUniformBlock() noexcept;

			void setName(std::string_view name) noexcept;
			const std::string& getName() const noexcept;

			void setType(UniformAttributeFormat type) noexcept;
			UniformAttributeFormat getType() const noexcept;

			void setBlockSize(std::uint32_t size) noexcept;
			std::uint32_t getBlockSize() const noexcept;

			void setBindingPoint(GLuint bindingPoint) noexcept;
			GLuint getBindingPoint() const noexcept;

			void setShaderStageFlags(ShaderStageFlags flags) noexcept;
			ShaderStageFlags getShaderStageFlags() const noexcept;

			void addGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
			void removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
			const GraphicsUniforms& getGraphicsUniforms() const noexcept;

		private:
			GL32GraphicsUniformBlock(const GL32GraphicsUniformBlock&) noexcept = delete;
			GL32GraphicsUniformBlock& operator=(const GL32GraphicsUniformBlock&) noexcept = delete;

		private:
			std::string _name;
			std::uint32_t _size;
			GLuint _bindingPoint;
			GraphicsUniforms _uniforms;
			UniformAttributeFormat _type;
			ShaderStageFlags _stageFlags;
		};

		class GL32Shader final : public GraphicsShader
		{
			OctoonDeclareSubClass(GL32Shader, GraphicsShader)
		public:
			GL32Shader() noexcept;
			~GL32Shader() noexcept;

			bool setup(const GraphicsShaderDesc& desc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsShaderDesc& getShaderDesc() const noexcept override;

		private:
			static bool HlslCodes2GLSL(ShaderStageFlags stage, const std::string& codes, std::string& out);
			static bool HlslByteCodes2GLSL(ShaderStageFlags stage, const char* codes, std::string& out);

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL32Shader(const GL32Shader&) noexcept = delete;
			GL32Shader& operator=(const GL32Shader&) noexcept = delete;

		private:
			GLuint _instance;
			GraphicsShaderDesc _shaderDesc;
			GraphicsDeviceWeakPtr _device;
		};

		class GL32Program final : public GraphicsProgram
		{
			OctoonDeclareSubClass(GL32Program, GraphicsProgram)
		public:
			GL32Program() noexcept;
			~GL32Program() noexcept;

			bool setup(const GraphicsProgramDesc& desc) noexcept;
			void close() noexcept;

			void apply() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsParams& getActiveParams() const noexcept override;
			const GraphicsAttributes& getActiveAttributes() const noexcept override;

			const GraphicsProgramDesc& getProgramDesc() const noexcept override;

		private:
			void _initActiveAttribute() noexcept;
			void _initActiveUniform() noexcept;
			void _initActiveUniformBlock() noexcept;

		private:
			static GraphicsFormat toGraphicsFormat(GLenum type) noexcept;
			static UniformAttributeFormat toGraphicsUniformType(std::string_view name, GLenum type) noexcept;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL32Program(const GL32Program&) noexcept = delete;
			GL32Program& operator=(const GL32Program&) noexcept = delete;

		private:

			bool _isActive;

			GLuint _program;

			GraphicsParams _activeParams;
			GraphicsAttributes  _activeAttributes;

			GraphicsDeviceWeakPtr _device;
			GraphicsProgramDesc _programDesc;
		};
	}
}

#endif