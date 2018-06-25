#ifndef OCTOON_GRAPHICS_SHADER_H_
#define OCTOON_GRAPHICS_SHADER_H_

#include <octoon/graphics/graphics_child.h>

namespace octoon
{
	namespace graphics
	{
		class OCTOON_EXPORT GraphicsShaderDesc final
		{
		public:
			GraphicsShaderDesc() noexcept;
			GraphicsShaderDesc(GraphicsShaderStageFlags stage, const std::string& code, const char* entry_point = "main",  GraphicsShaderLang lang = GraphicsShaderLang::HLSL, GraphicsShaderModel model = GraphicsShaderModel::Level4X) noexcept;
			GraphicsShaderDesc(GraphicsShaderStageFlags stage, const std::vector<char>& code, const char* entry_point = "main", GraphicsShaderLang lang = GraphicsShaderLang::HLSL, GraphicsShaderModel model = GraphicsShaderModel::Level4X) noexcept;
			~GraphicsShaderDesc() = default;

			void setLanguage(GraphicsShaderLang lang) noexcept;
			GraphicsShaderLang getLanguage() const noexcept;

			void setShaderModel(GraphicsShaderModel model) noexcept;
			GraphicsShaderModel getShaderModel() const noexcept;

			void setStage(GraphicsShaderStageFlags type) noexcept;
			GraphicsShaderStageFlags getStage() const noexcept;

			void setByteCodes(const std::string& codes) noexcept;
			void setByteCodes(std::string&& codes) noexcept;
			const std::string& getByteCodes() const noexcept;

			void setEntryPoint(const std::string& codes) noexcept;
			void setEntryPoint(std::string&& codes) noexcept;
			const std::string& getEntryPoint() const noexcept;

		private:
			std::string _main;
			std::string _bytecodes;
			GraphicsShaderLang _lang;
			GraphicsShaderModel _model;
			GraphicsShaderStageFlags _stage;
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

		class OCTOON_EXPORT GraphicsAttribute : public runtime::RttiInterface
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

		class OCTOON_EXPORT GraphicsParam : public runtime::RttiInterface
		{
			OctoonDeclareSubInterface(GraphicsParam, runtime::RttiInterface)
		public:
			GraphicsParam() noexcept = default;
			virtual ~GraphicsParam() = default;

			virtual const std::string& getName() const noexcept = 0;
			virtual GraphicsUniformType getType() const noexcept = 0;
			virtual GraphicsShaderStageFlags getShaderStageFlags() const noexcept = 0;
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

		class OCTOON_EXPORT GraphicsShader : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsShader, GraphicsChild)
		public:
			GraphicsShader() noexcept = default;
			virtual ~GraphicsShader() = default;

			virtual const GraphicsShaderDesc& getShaderDesc() const noexcept = 0;

		private:
			GraphicsShader(const GraphicsShader&) noexcept = delete;
			GraphicsShader& operator=(const GraphicsShader&) noexcept = delete;
		};

		class OCTOON_EXPORT GraphicsProgram : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsProgram, GraphicsChild)
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
}

#endif