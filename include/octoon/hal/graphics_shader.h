#ifndef OCTOON_GRAPHICS_SHADER_H_
#define OCTOON_GRAPHICS_SHADER_H_

#include <octoon/hal/graphics_resource.h>

namespace octoon
{
	class OCTOON_EXPORT GraphicsShaderDesc final
	{
	public:
		GraphicsShaderDesc() noexcept;
		GraphicsShaderDesc(ShaderStageFlags stage, const std::string& code, const char* entry_point = "main",  ShaderLanguage lang = ShaderLanguage::HLSL, ShaderModel model = ShaderModel::Level4X) noexcept;
		GraphicsShaderDesc(ShaderStageFlags stage, const std::vector<char>& code, const char* entry_point = "main", ShaderLanguage lang = ShaderLanguage::HLSL, ShaderModel model = ShaderModel::Level4X) noexcept;
		~GraphicsShaderDesc() = default;

		void setLanguage(ShaderLanguage lang) noexcept;
		ShaderLanguage getLanguage() const noexcept;

		void setShaderModel(ShaderModel model) noexcept;
		ShaderModel getShaderModel() const noexcept;

		void setStage(ShaderStageFlags type) noexcept;
		ShaderStageFlags getStage() const noexcept;

		void setByteCodes(std::string_view codes) noexcept;
		void setByteCodes(std::string&& codes) noexcept;
		const std::string& getByteCodes() const noexcept;

		void setEntryPoint(std::string_view codes) noexcept;
		void setEntryPoint(std::string&& codes) noexcept;
		const std::string& getEntryPoint() const noexcept;

	private:
		std::string _main;
		std::string _bytecodes;
		ShaderLanguage _lang;
		ShaderModel _model;
		ShaderStageFlags _stage;
	};

	class OCTOON_EXPORT GraphicsProgramDesc final
	{
	public:
		GraphicsProgramDesc() noexcept = default;
		~GraphicsProgramDesc() = default;

		bool addShader(GraphicsShaderPtr shader) noexcept;
		void removeShader(GraphicsShaderPtr shader) noexcept;

		const GraphicsShaders& getShaders() const noexcept;

	private:
		GraphicsShaders _shaders;
	};

	class OCTOON_EXPORT GraphicsAttribute : public Object
	{
		OctoonDeclareSubInterface(GraphicsAttribute, GraphicsParam)
	public:
		GraphicsAttribute() noexcept = default;
		virtual ~GraphicsAttribute() = default;

		virtual GraphicsFormat getType() const noexcept = 0;
		virtual const std::string& getSemantic() const noexcept = 0;
		virtual std::uint32_t getSemanticIndex() const noexcept = 0;

	private:
		GraphicsAttribute(const GraphicsAttribute&) noexcept = delete;
		GraphicsAttribute& operator=(const GraphicsAttribute&) noexcept = delete;
	};

	class OCTOON_EXPORT GraphicsParam : public Object
	{
		OctoonDeclareSubInterface(GraphicsParam, Object)
	public:
		GraphicsParam() noexcept = default;
		virtual ~GraphicsParam() = default;

		virtual const std::string& getName() const noexcept = 0;
		virtual UniformAttributeFormat getType() const noexcept = 0;
		virtual ShaderStageFlags getShaderStageFlags() const noexcept = 0;
		virtual std::uint32_t getBindingPoint() const noexcept = 0;

	private:
		GraphicsParam(const GraphicsParam&) noexcept = delete;
		GraphicsParam& operator=(const GraphicsParam&) noexcept = delete;
	};

	class OCTOON_EXPORT GraphicsUniform : public GraphicsParam
	{
		OctoonDeclareSubInterface(GraphicsUniform, GraphicsParam)
	public:
		GraphicsUniform() noexcept = default;
		virtual ~GraphicsUniform() = default;

		virtual std::uint32_t getOffset() const noexcept = 0;
		virtual const std::string& getSamplerName() const noexcept = 0;

	private:
		GraphicsUniform(const GraphicsUniform&) noexcept = delete;
		GraphicsUniform& operator=(const GraphicsUniform&) noexcept = delete;
	};

	class OCTOON_EXPORT GraphicsUniformBlock : public GraphicsParam
	{
		OctoonDeclareSubInterface(GraphicsUniformBlock, GraphicsParam)
	public:
		GraphicsUniformBlock() noexcept = default;
		virtual ~GraphicsUniformBlock() = default;

		virtual std::uint32_t getBlockSize() const noexcept = 0;
		virtual const GraphicsUniforms& getGraphicsUniforms() const noexcept = 0;

	private:
		GraphicsUniformBlock(const GraphicsUniformBlock&) noexcept = delete;
		GraphicsUniformBlock& operator=(const GraphicsUniformBlock&) noexcept = delete;
	};

	class OCTOON_EXPORT GraphicsShader : public GraphicsResource
	{
		OctoonDeclareSubInterface(GraphicsShader, GraphicsResource)
	public:
		GraphicsShader() noexcept = default;
		virtual ~GraphicsShader() = default;

		virtual const GraphicsShaderDesc& getShaderDesc() const noexcept = 0;

	private:
		GraphicsShader(const GraphicsShader&) noexcept = delete;
		GraphicsShader& operator=(const GraphicsShader&) noexcept = delete;
	};

	class OCTOON_EXPORT GraphicsProgram : public GraphicsResource
	{
		OctoonDeclareSubInterface(GraphicsProgram, GraphicsResource)
	public:
		GraphicsProgram() noexcept = default;
		virtual ~GraphicsProgram() = default;

		virtual const GraphicsParams& getActiveParams() const noexcept = 0;
		virtual const GraphicsAttributes& getActiveAttributes() const noexcept = 0;

		virtual const GraphicsProgramDesc& getProgramDesc() const noexcept = 0;

	private:
		GraphicsProgram(const GraphicsProgram&) noexcept = delete;
		GraphicsProgram& operator=(const GraphicsProgram&) noexcept = delete;
	};
}

#endif