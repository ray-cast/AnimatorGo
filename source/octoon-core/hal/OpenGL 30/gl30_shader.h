#ifndef OCTOON_GL30_SHADER_H_
#define OCTOON_GL30_SHADER_H_

#include "gl30_types.h"

namespace octoon
{
	namespace hal
	{
		class GL30GraphicsAttribute final : public GraphicsAttribute
		{
			OctoonDeclareSubClass(GL30GraphicsAttribute, GraphicsAttribute)
		public:
			GL30GraphicsAttribute() noexcept;
			~GL30GraphicsAttribute() noexcept;

			void setSemantic(std::string_view semantic) noexcept;
			const std::string& getSemantic() const noexcept override;

			void setSemanticIndex(std::uint8_t index) noexcept;
			std::uint32_t getSemanticIndex() const noexcept override;

			void setType(GraphicsFormat type) noexcept;
			GraphicsFormat getType() const noexcept override;

			void setBindingPoint(std::uint32_t bindingPoint) noexcept;
			std::uint32_t getBindingPoint() const noexcept;

		private:
			GL30GraphicsAttribute(const GL30GraphicsAttribute&) noexcept = delete;
			GL30GraphicsAttribute& operator=(const GL30GraphicsAttribute&) noexcept = delete;

		private:
			std::string _semantic;
			std::uint8_t _semanticIndex;
			std::uint32_t _bindingPoint;
			GraphicsFormat _type;
		};

		class GL30GraphicsUniform final : public GraphicsUniform
		{
			OctoonDeclareSubClass(GL30GraphicsUniform, GraphicsUniform)
		public:
			GL30GraphicsUniform() noexcept;
			~GL30GraphicsUniform() noexcept;

			void setName(std::string_view name) noexcept;
			const std::string& getName() const noexcept override;

			void setSamplerName(std::string_view name) noexcept;
			const std::string& getSamplerName() const noexcept override;

			void setType(GraphicsUniformType type) noexcept;
			GraphicsUniformType getType() const noexcept override;

			void setOffset(std::uint32_t offset) noexcept;
			std::uint32_t getOffset() const noexcept override;

			void setBindingPoint(GLuint bindingPoint) noexcept;
			GLuint getBindingPoint() const noexcept override;

			void setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept;
			GraphicsShaderStageFlags getShaderStageFlags() const noexcept override;

		private:
			GL30GraphicsUniform(const GL30GraphicsUniform&) noexcept = delete;
			GL30GraphicsUniform& operator=(const GL30GraphicsUniform&) noexcept = delete;

		private:
			std::string _name;
			std::string _samplerName;
			std::uint32_t _offset;
			GLuint _bindingPoint;
			GraphicsUniformType _type;
			GraphicsShaderStageFlags _stageFlags;
		};

		class GL30GraphicsUniformBlock final : public GraphicsUniformBlock
		{
			OctoonDeclareSubClass(GL30GraphicsUniformBlock, GraphicsUniformBlock)
		public:
			GL30GraphicsUniformBlock() noexcept;
			~GL30GraphicsUniformBlock() noexcept;

			void setName(std::string_view name) noexcept;
			const std::string& getName() const noexcept override;

			void setType(GraphicsUniformType type) noexcept;
			GraphicsUniformType getType() const noexcept override;

			void setBlockSize(std::uint32_t size) noexcept;
			std::uint32_t getBlockSize() const noexcept override;

			void setBindingPoint(GLuint bindingPoint) noexcept;
			GLuint getBindingPoint() const noexcept override;

			void setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept;
			GraphicsShaderStageFlags getShaderStageFlags() const noexcept override;

			void addGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
			void removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
			const GraphicsUniforms& getGraphicsUniforms() const noexcept override;

		private:
			GL30GraphicsUniformBlock(const GL30GraphicsUniformBlock&) noexcept = delete;
			GL30GraphicsUniformBlock& operator=(const GL30GraphicsUniformBlock&) noexcept = delete;

		private:
			std::string _name;
			std::uint32_t _size;
			GLuint _bindingPoint;
			GraphicsUniforms _uniforms;
			GraphicsUniformType _type;
			GraphicsShaderStageFlags _stageFlags;
		};

		class GL30Shader final : public GraphicsShader
		{
			OctoonDeclareSubClass(GL30Shader, GraphicsShader)
		public:
			GL30Shader() noexcept;
			~GL30Shader() noexcept;

			bool setup(const GraphicsShaderDesc& desc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsShaderDesc& getShaderDesc() const noexcept override;

		private:
			static bool HlslCodes2GLSL(GraphicsShaderStageFlags stage, const std::string& codes, std::string& out);
			static bool HlslByteCodes2GLSL(GraphicsShaderStageFlags stage, const char* codes, std::string& out);

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL30Shader(const GL30Shader&) noexcept = delete;
			GL30Shader& operator=(const GL30Shader&) noexcept = delete;

		private:
			GLuint _instance;
			GraphicsShaderDesc _shaderDesc;
			GraphicsDeviceWeakPtr _device;
		};

		class GL30Program final : public GraphicsProgram
		{
			OctoonDeclareSubClass(GL30Program, GraphicsProgram)
		public:
			GL30Program() noexcept;
			~GL30Program() noexcept;

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
			static GraphicsUniformType toGraphicsUniformType(std::string_view name, GLenum type) noexcept;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL30Program(const GL30Program&) noexcept = delete;
			GL30Program& operator=(const GL30Program&) noexcept = delete;

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