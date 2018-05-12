#ifndef OCTOON_EGL3_SHADER_H_
#define OCTOON_EGL3_SHADER_H_

#include "egl3_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL3GraphicsAttribute final : public GraphicsAttribute
		{
			OctoonDeclareSubClass(EGL3GraphicsAttribute, GraphicsAttribute)
		public:
			EGL3GraphicsAttribute() noexcept;
			~EGL3GraphicsAttribute() noexcept;

			void setSemantic(const std::string& semantic) noexcept;
			const std::string& getSemantic() const noexcept;

			void setSemanticIndex(std::uint32_t index) noexcept;
			std::uint32_t getSemanticIndex() const noexcept;

			void setType(GraphicsFormat type) noexcept;
			GraphicsFormat getType() const noexcept;

			void setBindingPoint(std::uint32_t bindingPoint) noexcept;
			std::uint32_t getBindingPoint() const noexcept;

		private:
			EGL3GraphicsAttribute(const EGL3GraphicsAttribute&) noexcept = delete;
			EGL3GraphicsAttribute& operator=(const EGL3GraphicsAttribute&) noexcept = delete;

		private:
			std::string _semantic;
			std::uint8_t _semanticIndex;
			std::uint32_t _bindingPoint;
			GraphicsFormat _type;
		};

		class EGL3GraphicsUniform final : public GraphicsUniform
		{
			OctoonDeclareSubClass(EGL3GraphicsUniform, GraphicsUniform)
		public:
			EGL3GraphicsUniform() noexcept;
			~EGL3GraphicsUniform() noexcept;

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
			EGL3GraphicsUniform(const EGL3GraphicsUniform&) noexcept = delete;
			EGL3GraphicsUniform& operator=(const EGL3GraphicsUniform&) noexcept = delete;

		private:
			std::string _name;
			std::string _samplerName;
			std::uint32_t _offset;
			GLuint _bindingPoint;
			GraphicsUniformType _type;
			GraphicsShaderStageFlags _stageFlags;
		};

		class EGL3GraphicsUniformBlock final : public GraphicsUniformBlock
		{
			OctoonDeclareSubClass(EGL3GraphicsUniformBlock, GraphicsUniformBlock)
		public:
			EGL3GraphicsUniformBlock() noexcept;
			~EGL3GraphicsUniformBlock() noexcept;

			void setName(const std::string& name) noexcept;
			const std::string& getName() const noexcept;

			void setType(GraphicsUniformType type) noexcept;
			GraphicsUniformType getType() const noexcept;

			void setBlockSize(std::uint32_t size) noexcept;
			std::uint32_t getBlockSize() const noexcept;

			void setBindingPoint(GLuint bindingPoint) noexcept;
			GLuint getBindingPoint() const noexcept;

			void setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept;
			GraphicsShaderStageFlags getShaderStageFlags() const noexcept;

			void addGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
			void removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept;
			const GraphicsUniforms& getGraphicsUniforms() const noexcept;

		private:
			EGL3GraphicsUniformBlock(const EGL3GraphicsUniformBlock&) noexcept = delete;
			EGL3GraphicsUniformBlock& operator=(const EGL3GraphicsUniformBlock&) noexcept = delete;

		private:
			std::string _name;
			std::uint32_t _size;
			GLuint _bindingPoint;
			GraphicsUniforms _uniforms;
			GraphicsUniformType _type;
			GraphicsShaderStageFlags _stageFlags;
		};

		class EGL3Shader final : public GraphicsShader
		{
			OctoonDeclareSubClass(EGL3Shader, GraphicsShader)
		public:
			EGL3Shader() noexcept;
			~EGL3Shader() noexcept;

			bool setup(const GraphicsShaderDesc& desc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsShaderDesc& getGraphicsShaderDesc() const noexcept;

		private:
			static bool HlslCodes2GLSL(GraphicsShaderStageFlags stage, const std::string& codes, std::string& out);
			static bool HlslByteCodes2GLSL(GraphicsShaderStageFlags stage, const char* codes, std::string& out);

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL3Shader(const EGL3Shader&) noexcept = delete;
			EGL3Shader& operator=(const EGL3Shader&) noexcept = delete;

		private:
			GLuint _instance;
			GraphicsShaderDesc _shaderDesc;
			GraphicsDeviceWeakPtr _device;
		};

		class EGL3Program final : public GraphicsProgram
		{
			OctoonDeclareSubClass(EGL3Program, GraphicsProgram)
		public:
			EGL3Program() noexcept;
			~EGL3Program() noexcept;

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
			void _initActiveUniformBlock() noexcept;

		private:
			static GraphicsFormat toGraphicsFormat(GLenum type) noexcept;
			static GraphicsUniformType toGraphicsUniformType(const std::string& name, GLenum type) noexcept;

		private:
			friend class EGL3Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL3Program(const EGL3Program&) noexcept = delete;
			EGL3Program& operator=(const EGL3Program&) noexcept = delete;

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