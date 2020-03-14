#include <octoon/hal/graphics_shader.h>

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubInterface(GraphicsParam, runtime::RttiInterface, "GraphicsParam")
		OctoonImplementSubInterface(GraphicsAttribute, runtime::RttiInterface, "GraphicsAttribute")
		OctoonImplementSubInterface(GraphicsUniform, GraphicsParam, "GraphicsUniform")
		OctoonImplementSubInterface(GraphicsUniformBlock, GraphicsParam, "GraphicsUniformBlock")
		OctoonImplementSubInterface(GraphicsShader, GraphicsChild, "GraphicsShader")
		OctoonImplementSubInterface(GraphicsProgram, GraphicsChild, "GraphicsProgram")

		GraphicsShaderDesc::GraphicsShaderDesc() noexcept
			: _lang(GraphicsShaderLang::None)
			, _model(GraphicsShaderModel::Level4X)
			, _stage(GraphicsShaderStageFlagBits::VertexBit)
			, _main("main")
		{
		}

		GraphicsShaderDesc::GraphicsShaderDesc(GraphicsShaderStageFlags stage, const std::string& code, const char* entry_point, GraphicsShaderLang lang, GraphicsShaderModel model) noexcept
			: _bytecodes(code)
			, _lang(lang)
			, _model(model)
			, _stage(stage)
			, _main(entry_point)
		{
		}

		GraphicsShaderDesc::GraphicsShaderDesc(GraphicsShaderStageFlags stage, const std::vector<char>& code, const char* entry_point, GraphicsShaderLang lang, GraphicsShaderModel model) noexcept
			: _lang(lang)
			, _model(model)
			, _stage(stage)
			, _main(entry_point)
		{
			_bytecodes.insert(_bytecodes.begin(), code.begin(), code.end());
		}

		void
		GraphicsShaderDesc::setLanguage(GraphicsShaderLang lang) noexcept
		{
			_lang = lang;
		}

		GraphicsShaderLang
		GraphicsShaderDesc::getLanguage() const noexcept
		{
			return _lang;
		}

		void
		GraphicsShaderDesc::setShaderModel(GraphicsShaderModel model) noexcept
		{
			_model = model;
		}

		GraphicsShaderModel
		GraphicsShaderDesc::getShaderModel() const noexcept
		{
			return _model;
		}

		void
		GraphicsShaderDesc::setStage(GraphicsShaderStageFlags stage) noexcept
		{
			_stage = stage;
		}

		GraphicsShaderStageFlags
		GraphicsShaderDesc::getStage()const noexcept
		{
			return _stage;
		}

		void
		GraphicsShaderDesc::setByteCodes(std::string_view codes) noexcept
		{
			_bytecodes = codes;
		}

		void
		GraphicsShaderDesc::setByteCodes(std::string&& codes) noexcept
		{
			_bytecodes = std::move(codes);
		}

		const std::string&
		GraphicsShaderDesc::getByteCodes() const noexcept
		{
			return _bytecodes;
		}

		void
		GraphicsShaderDesc::setEntryPoint(std::string_view main) noexcept
		{
			_main = main;
		}

		void
		GraphicsShaderDesc::setEntryPoint(std::string&& main) noexcept
		{
			_main = std::move(main);
		}

		const std::string&
		GraphicsShaderDesc::getEntryPoint() const noexcept
		{
			return _main;
		}

		bool
		GraphicsProgramDesc::addShader(GraphicsShaderPtr shader) noexcept
		{
			assert(shader);

			auto comp = [&](const GraphicsShaderPtr& it)
			{
				return it->getShaderDesc().getStage() == shader->getShaderDesc().getStage();
			};

			auto it = std::find_if(_shaders.begin(), _shaders.end(), comp);
			if (it != _shaders.end())
				return false;

			_shaders.push_back(shader);
			return true;
		}

		void
		GraphicsProgramDesc::removeShader(GraphicsShaderPtr shader) noexcept
		{
			auto it = std::find(_shaders.begin(), _shaders.end(), shader);
			if (it != _shaders.end())
				_shaders.erase(it);
		}

		const GraphicsShaders&
		GraphicsProgramDesc::getShaders() const noexcept
		{
			return _shaders;
		}
	}
}