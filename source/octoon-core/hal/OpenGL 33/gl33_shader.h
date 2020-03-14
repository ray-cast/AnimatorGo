#ifndef OCTOON_GL33_SHADER_H_
#define OCTOON_GL33_SHADER_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL33GraphicsAttribute final : public GraphicsAttribute
		{
			OctoonDeclareSubClass(GL33GraphicsAttribute, GraphicsAttribute)
		public:
			GL33GraphicsAttribute() noexcept;
			~GL33GraphicsAttribute() noexcept;

			void setSemantic(std::string_view semantic) noexcept;
			const std::string& getSemantic() const noexcept;

			void setSemanticIndex(std::uint32_t index) noexcept;
			std::uint32_t getSemanticIndex() const noexcept;

			void setType(GraphicsFormat type) noexcept;
			GraphicsFormat getType() const noexcept;

			void setBindingPoint(std::uint32_t bindingPoint) noexcept;
			std::uint32_t getBindingPoint() const noexcept;

		private:
			GL33GraphicsAttribute(const GL33GraphicsAttribute&) noexcept = delete;
			GL33GraphicsAttribute& operator=(const GL33GraphicsAttribute&) noexcept = delete;

		private:
			std::string _semantic;
			std::uint32_t _semanticIndex;
			std::uint32_t _bindingPoint;
			GraphicsFormat _type;
		};

		class GL33GraphicsUniform final : public GraphicsUniform
		{
			OctoonDeclareSubClass(GL33GraphicsUniform, GraphicsUniform)
		public:
			GL33GraphicsUniform() noexcept;
			~GL33GraphicsUniform() noexcept;

			void setName(std::string_view name) noexcept;
			const std::string& getName() const noexcept;

			void setSamplerName(std::string_view name) noexcept;
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
			GL33GraphicsUniform(const GL33GraphicsUniform&) noexcept = delete;
			GL33GraphicsUniform& operator=(const GL33GraphicsUniform&) noexcept = delete;

		private:
			std::string _name;
			std::string _samplerName;
			std::uint32_t _offset;
			std::uint32_t _bindingPoint;
			GraphicsUniformType _type;
			GraphicsShaderStageFlags _stageFlags;
		};

		class GL33GraphicsUniformBlock final : public GraphicsUniformBlock
		{
			OctoonDeclareSubClass(GL33GraphicsUniformBlock, GraphicsUniformBlock)
		public:
			GL33GraphicsUniformBlock() noexcept;
			~GL33GraphicsUniformBlock() noexcept;

			void setName(std::string_view name) noexcept;
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
			GL33GraphicsUniformBlock(const GL33GraphicsUniformBlock&) noexcept = delete;
			GL33GraphicsUniformBlock& operator=(const GL33GraphicsUniformBlock&) noexcept = delete;

		private:
			std::string _name;
			std::uint32_t _size;
			std::uint32_t _bindingPoint;
			GraphicsUniforms _uniforms;
			GraphicsUniformType _type;
			GraphicsShaderStageFlags _stageFlags;
		};

		class GL33Shader final : public GraphicsShader
		{
			OctoonDeclareSubClass(GL33Shader, GraphicsShader)
		public:
			GL33Shader() noexcept;
			~GL33Shader() noexcept;

			bool setup(const GraphicsShaderDesc& shader) noexcept;
			void close() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsShaderDesc& getShaderDesc() const noexcept override;

		private:
			bool HlslCodes2GLSL(GraphicsShaderStageFlags stage, const std::string& codes, const std::string& main, std::string& out);
			bool HlslByteCodes2GLSL(GraphicsShaderStageFlags stage, const char* codes, std::string& out);

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL33Shader(const GL33Shader&) noexcept = delete;
			GL33Shader& operator=(const GL33Shader&) noexcept = delete;

		private:
			GLuint _instance;

			GraphicsShaderDesc _shaderDesc;
			GraphicsDeviceWeakPtr _device;
		};

		class GL33Program final : public GraphicsProgram
		{
			OctoonDeclareSubClass(GL33Program, GraphicsProgram)
		public:
			GL33Program() noexcept;
			~GL33Program() noexcept;

			bool setup(const GraphicsProgramDesc& program) noexcept;
			void close() noexcept;

			void apply() noexcept;

			GLuint getInstanceID() const noexcept;

			const GraphicsParams& getActiveParams() const noexcept;
			const GraphicsAttributes& getActiveAttributes() const noexcept;

			const GraphicsProgramDesc& getProgramDesc() const noexcept override;

		private:
			void _initActiveAttribute() noexcept;
			void _initActiveUniform() noexcept;
			void _initActiveUniformBlock() noexcept;

		private:
			static GraphicsFormat toGraphicsFormat(GLenum type) noexcept;
			static GraphicsUniformType toGraphicsUniformType(std::string_view name, GLenum type) noexcept;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL33Program(const GL33Program&) noexcept = delete;
			GL33Program& operator=(const GL33Program&) noexcept = delete;

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