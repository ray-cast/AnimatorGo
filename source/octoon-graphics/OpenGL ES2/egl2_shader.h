#ifndef OCTOON_EGL2_SHADER_H_
#define OCTOON_EGL2_SHADER_H_

#include "egl2_types.h"

namespace octoon 
{
	namespace graphics
	{
		class EGL2GraphicsAttribute final : public GraphicsAttribute
		{
			OctoonDeclareSubClass(EGL2GraphicsAttribute, GraphicsAttribute)
		public:
			EGL2GraphicsAttribute() noexcept;
			~EGL2GraphicsAttribute() noexcept;

			void setSemantic(const std::string& semantic) noexcept;
			const std::string& getSemantic() const noexcept;

			void setSemanticIndex(std::uint32_t index) noexcept;
			std::uint32_t getSemanticIndex() const noexcept;

			void setType(GraphicsFormat type) noexcept;
			GraphicsFormat getType() const noexcept;

			void setBindingPoint(std::uint32_t bindingPoint) noexcept;
			std::uint32_t getBindingPoint() const noexcept;

		private:
			EGL2GraphicsAttribute(const EGL2GraphicsAttribute&) noexcept = delete;
			EGL2GraphicsAttribute& operator=(const EGL2GraphicsAttribute&) noexcept = delete;

		private:
			std::string _semantic;
			std::uint8_t _semanticIndex;
			std::uint32_t _bindingPoint;
			GraphicsFormat _type;
		};

		class EGL2GraphicsUniform final : public GraphicsUniform
		{
			OctoonDeclareSubClass(EGL2GraphicsUniform, GraphicsUniform)
		public:
			EGL2GraphicsUniform() noexcept;
			~EGL2GraphicsUniform() noexcept;

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
			EGL2GraphicsUniform(const EGL2GraphicsUniform&) noexcept = delete;
			EGL2GraphicsUniform& operator=(const EGL2GraphicsUniform&) noexcept = delete;

		private:
			std::string _name;
			std::string _samplerName;
			std::uint32_t _offset;
			GLuint _bindingPoint;
			GraphicsUniformType _type;
			GraphicsShaderStageFlags _stageFlags;
		};

		class EGL2Shader final : public GraphicsShader
		{
			OctoonDeclareSubClass(EGL2Shader, GraphicsShader)
		public:
			EGL2Shader() noexcept;
			~EGL2Shader() noexcept;

			bool setup(const GraphicsShaderDesc& desc) noexcept;
			void close() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsShaderDesc& getGraphicsShaderDesc() const noexcept;

		private:
			static bool HlslCodes2GLSL(GraphicsShaderStageFlags stage, const std::string& codes, std::string& out);
			static bool HlslByteCodes2GLSL(GraphicsShaderStageFlags stage, const char* codes, std::string& out);

		private:
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL2Shader(const EGL2Shader&) noexcept = delete;
			EGL2Shader& operator=(const EGL2Shader&) noexcept = delete;

		private:
			GLuint _instance;
			GraphicsDeviceWeakPtr _device;
			GraphicsShaderDesc _shaderDesc;
		};

		class EGL2Program final : public GraphicsProgram
		{
			OctoonDeclareSubClass(EGL2Program, GraphicsProgram)
		public:
			EGL2Program() noexcept;
			~EGL2Program() noexcept;

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
			friend class EGL2Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			EGL2Program(const EGL2Program&) noexcept = delete;
			EGL2Program& operator=(const EGL2Program&) noexcept = delete;

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