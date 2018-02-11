#ifndef OCTOON_OGL_SHADER_H_
#define OCTOON_OGL_SHADER_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLGraphicsAttribute final : public GraphicsAttribute
		{
			OctoonDeclareSubClass(OGLGraphicsAttribute, GraphicsAttribute)
		public:
			OGLGraphicsAttribute() noexcept;
			~OGLGraphicsAttribute() noexcept;

			void setSemantic(const std::string& semantic) noexcept;
			const std::string& getSemantic() const noexcept;

			void setSemanticIndex(std::uint32_t index) noexcept;
			std::uint32_t getSemanticIndex() const noexcept;

			void setType(GraphicsFormat type) noexcept;
			GraphicsFormat getType() const noexcept;

			void setBindingPoint(std::uint32_t bindingPoint) noexcept;
			std::uint32_t getBindingPoint() const noexcept;

		private:
			OGLGraphicsAttribute(const OGLGraphicsAttribute&) noexcept = delete;
			OGLGraphicsAttribute& operator=(const OGLGraphicsAttribute&) noexcept = delete;

		private:
			std::string _semantic;
			std::uint32_t _semanticIndex;
			std::uint32_t _bindingPoint;
			GraphicsFormat _type;
		};

		class OGLGraphicsUniform final : public GraphicsUniform
		{
			OctoonDeclareSubClass(OGLGraphicsUniform, GraphicsUniform)
		public:
			OGLGraphicsUniform() noexcept;
			~OGLGraphicsUniform() noexcept;

			void setName(const std::string& name) noexcept;
			const std::string& getName() const noexcept;

			void setSamplerName(const std::string& name) noexcept;
			const std::string& getSamplerName() const noexcept;

			void setType(GraphicsUniformType type) noexcept;
			GraphicsUniformType getType() const noexcept;

			void setOffset(std::uint32_t offset) noexcept;
			std::uint32_t getOffset() const noexcept;

			void setBindingPoint(std::uint32_t bindingPoint) noexcept;
			std::uint32_t getBindingPoint() const noexcept;

			void setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept;
			GraphicsShaderStageFlags getShaderStageFlags() const noexcept;

		private:
			OGLGraphicsUniform(const OGLGraphicsUniform&) noexcept = delete;
			OGLGraphicsUniform& operator=(const OGLGraphicsUniform&) noexcept = delete;

		private:
			std::string _name;
			std::string _samplerName;
			std::uint32_t _offset;
			std::uint32_t _bindingPoint;
			GraphicsUniformType _type;
			GraphicsShaderStageFlags _stageFlags;
		};

		class OGLGraphicsUniformBlock final : public GraphicsUniformBlock
		{
			OctoonDeclareSubClass(OGLGraphicsUniformBlock, GraphicsUniformBlock)
		public:
			OGLGraphicsUniformBlock() noexcept;
			~OGLGraphicsUniformBlock() noexcept;

			void setName(const std::string& name) noexcept;
			const std::string& getName() const noexcept;

			void setType(GraphicsUniformType type) noexcept;
			GraphicsUniformType getType() const noexcept;

			void setBlockSize(std::uint32_t size) noexcept;
			std::uint32_t getBlockSize() const noexcept;

			void setBindingPoint(std::uint32_t bindingPoint) noexcept;
			std::uint32_t getBindingPoint() const noexcept;

			void setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept;
			GraphicsShaderStageFlags getShaderStageFlags() const noexcept;

			void addGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
			void removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
			const GraphicsUniforms& getGraphicsUniforms() const noexcept;

		private:
			OGLGraphicsUniformBlock(const OGLGraphicsUniformBlock&) noexcept = delete;
			OGLGraphicsUniformBlock& operator=(const OGLGraphicsUniformBlock&) noexcept = delete;

		private:
			std::string _name;
			std::uint32_t _size;
			std::uint32_t _bindingPoint;
			GraphicsUniforms _uniforms;
			GraphicsUniformType _type;
			GraphicsShaderStageFlags _stageFlags;
		};

		class OGLShader final : public GraphicsShader
		{
			OctoonDeclareSubClass(OGLShader, GraphicsShader)
		public:
			OGLShader() noexcept;
			~OGLShader() noexcept;

			bool setup(const GraphicsShaderDesc& shader) noexcept;
			void close() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsShaderDesc& getGraphicsShaderDesc() const noexcept;

		private:
			bool HlslCodes2GLSL(GraphicsShaderStageFlags stage, const std::string& codes, const std::string& main, std::string& out);
			bool HlslByteCodes2GLSL(GraphicsShaderStageFlags stage, const char* codes, std::string& out);

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLShader(const OGLShader&) noexcept = delete;
			OGLShader& operator=(const OGLShader&) noexcept = delete;

		private:
			GLuint _instance;

			GraphicsShaderDesc _shaderDesc;
			GraphicsDeviceWeakPtr _device;
		};

		class OGLProgram final : public GraphicsProgram
		{
			OctoonDeclareSubClass(OGLProgram, GraphicsProgram)
		public:
			OGLProgram() noexcept;
			~OGLProgram() noexcept;

			bool setup(const GraphicsProgramDesc& program) noexcept;
			void close() noexcept;

			void apply() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsParams& getActiveParams() const noexcept;
			const GraphicsAttributes& getActiveAttributes() const noexcept;

			const GraphicsProgramDesc& getGraphicsProgramDesc() const noexcept;

		private:
			void _initActiveAttribute() noexcept;
			void _initActiveUniform() noexcept;
			void _initActiveUniformBlock() noexcept;

		private:
			static GraphicsFormat toGraphicsFormat(GLenum type) noexcept;
			static GraphicsUniformType toGraphicsUniformType(const std::string& name, GLenum type) noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLProgram(const OGLProgram&) noexcept = delete;
			OGLProgram& operator=(const OGLProgram&) noexcept = delete;

		private:
			GLuint _program;
			GraphicsParams _activeParams;
			GraphicsAttributes  _activeAttributes;
			GraphicsProgramDesc _programDesc;
			GraphicsDeviceWeakPtr _device;
		};
	}
}

#endif