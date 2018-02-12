#include "ogl_shader.h"
#include "ogl_device.h"

#define EXCLUDE_PSTDINT
#include <hlslcc.hpp>

#if defined(__WINDOWS__)
#	include <sstream>
#	include <d3dcompiler.h>
#endif

namespace octoon
{
	namespace graphics
	{
		OctoonImplementSubClass(OGLShader, GraphicsShader, "OGLShader")
		OctoonImplementSubClass(OGLProgram, GraphicsProgram, "OGLProgram")
		OctoonImplementSubClass(OGLGraphicsAttribute, GraphicsAttribute, "OGLGraphicsAttribute")
		OctoonImplementSubClass(OGLGraphicsUniform, GraphicsUniform, "OGLGraphicsUniform")
		OctoonImplementSubClass(OGLGraphicsUniformBlock, GraphicsUniformBlock, "OGLGraphicsUniformBlock")

		OGLGraphicsAttribute::OGLGraphicsAttribute() noexcept
			: _semanticIndex(0)
			, _bindingPoint(GL_INVALID_INDEX)
			, _type(GraphicsFormat::Undefined)
		{
		}

		OGLGraphicsAttribute::~OGLGraphicsAttribute() noexcept
		{
		}

		void
		OGLGraphicsAttribute::setSemantic(const std::string& semantic) noexcept
		{
			_semantic = semantic;
		}

		const std::string&
		OGLGraphicsAttribute::getSemantic() const noexcept
		{
			return _semantic;
		}

		void
		OGLGraphicsAttribute::setSemanticIndex(std::uint32_t index) noexcept
		{
			_semanticIndex = index;
		}

		std::uint32_t
		OGLGraphicsAttribute::getSemanticIndex() const noexcept
		{
			return _semanticIndex;
		}

		void
		OGLGraphicsAttribute::setType(GraphicsFormat type) noexcept
		{
			_type = type;
		}

		GraphicsFormat
		OGLGraphicsAttribute::getType() const noexcept
		{
			return _type;
		}

		void
		OGLGraphicsAttribute::setBindingPoint(std::uint32_t bindingPoint) noexcept
		{
			_bindingPoint = bindingPoint;
		}

		std::uint32_t
		OGLGraphicsAttribute::getBindingPoint() const noexcept
		{
			return _bindingPoint;
		}

		OGLGraphicsUniform::OGLGraphicsUniform() noexcept
			: _offset(0)
			, _bindingPoint(GL_INVALID_INDEX)
			, _type(GraphicsUniformType::None)
			, _stageFlags(0)
		{
		}

		OGLGraphicsUniform::~OGLGraphicsUniform() noexcept
		{
		}

		void
		OGLGraphicsUniform::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string&
		OGLGraphicsUniform::getName() const noexcept
		{
			return _name;
		}

		void
		OGLGraphicsUniform::setSamplerName(const std::string& name) noexcept
		{
			_samplerName = name;
		}

		const std::string&
		OGLGraphicsUniform::getSamplerName() const noexcept
		{
			return _samplerName;
		}

		void
		OGLGraphicsUniform::setType(GraphicsUniformType type) noexcept
		{
			_type = type;
		}

		GraphicsUniformType
		OGLGraphicsUniform::getType() const noexcept
		{
			return _type;
		}

		void
		OGLGraphicsUniform::setOffset(std::uint32_t offset) noexcept
		{
			_offset = offset;
		}

		std::uint32_t
		OGLGraphicsUniform::getOffset() const noexcept
		{
			return _offset;
		}

		void
		OGLGraphicsUniform::setBindingPoint(std::uint32_t bindingPoint) noexcept
		{
			_bindingPoint = bindingPoint;
		}

		std::uint32_t
		OGLGraphicsUniform::getBindingPoint() const noexcept
		{
			return _bindingPoint;
		}

		void
		OGLGraphicsUniform::setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept
		{
			_stageFlags = flags;
		}

		GraphicsShaderStageFlags
		OGLGraphicsUniform::getShaderStageFlags() const noexcept
		{
			return _stageFlags;
		}

		OGLGraphicsUniformBlock::OGLGraphicsUniformBlock() noexcept
			: _size(0)
			, _bindingPoint(GL_INVALID_INDEX)
			, _type(GraphicsUniformType::UniformBuffer)
			, _stageFlags(0)
		{
		}

		OGLGraphicsUniformBlock::~OGLGraphicsUniformBlock() noexcept
		{
		}

		void
		OGLGraphicsUniformBlock::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string&
		OGLGraphicsUniformBlock::getName() const noexcept
		{
			return _name;
		}

		void
		OGLGraphicsUniformBlock::setType(GraphicsUniformType type) noexcept
		{
			_type = type;
		}

		GraphicsUniformType
		OGLGraphicsUniformBlock::getType() const noexcept
		{
			return _type;
		}

		void
		OGLGraphicsUniformBlock::setBlockSize(std::uint32_t size) noexcept
		{
			_size = size;
		}

		std::uint32_t
		OGLGraphicsUniformBlock::getBlockSize() const noexcept
		{
			return _size;
		}

		void
		OGLGraphicsUniformBlock::setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept
		{
			_stageFlags = flags;
		}

		GraphicsShaderStageFlags
		OGLGraphicsUniformBlock::getShaderStageFlags() const noexcept
		{
			return _stageFlags;
		}

		void
		OGLGraphicsUniformBlock::addGraphicsUniform(GraphicsUniformPtr uniform) noexcept
		{
			_uniforms.push_back(uniform);
		}

		void
		OGLGraphicsUniformBlock::removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept
		{
			auto it = std::find(_uniforms.begin(), _uniforms.end(), uniform);
			if (it != _uniforms.end())
				_uniforms.erase(it);
		}

		const GraphicsUniforms&
		OGLGraphicsUniformBlock::getGraphicsUniforms() const noexcept
		{
			return _uniforms;
		}

		void
		OGLGraphicsUniformBlock::setBindingPoint(std::uint32_t bindingPoint) noexcept
		{
			_bindingPoint = bindingPoint;
		}

		std::uint32_t
		OGLGraphicsUniformBlock::getBindingPoint() const noexcept
		{
			return _bindingPoint;
		}

		OGLShader::OGLShader() noexcept
			: _instance(GL_NONE)
		{
		}

		OGLShader::~OGLShader() noexcept
		{
			this->close();
		}

		bool
		OGLShader::setup(const GraphicsShaderDesc& shaderDesc) noexcept
		{
			assert(!_instance);
			assert(!shaderDesc.getByteCodes().empty());

			auto type = OGLTypes::asShaderStage(shaderDesc.getStage());
			if (type == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<OGLDevice>()->message("Invalid shader type");
				return false;
			}

			_instance = glCreateShader(type);
			if (_instance == GL_NONE)
			{
				this->getDevice()->downcast<OGLDevice>()->message("glCreateShader() fail.");
				return false;
			}

			std::string codes;
			if (shaderDesc.getLanguage() == GraphicsShaderLang::HLSL)
			{
				if (!HlslCodes2GLSL(shaderDesc.getStage(), shaderDesc.getByteCodes().data(), shaderDesc.getEntryPoint().data(), codes))
				{
					this->getDevice()->downcast<OGLDevice>()->message("Can't conv hlsl to glsl.");
					return false;
				}
			}
			else if (shaderDesc.getLanguage() == GraphicsShaderLang::HLSLbytecodes)
			{
				if (!HlslByteCodes2GLSL(shaderDesc.getStage(), shaderDesc.getByteCodes().data(), codes))
				{
					this->getDevice()->downcast<OGLDevice>()->message("Can't conv hlslbytecodes to glsl.");
					return false;
				}
			}
			else
			{
				codes = shaderDesc.getByteCodes();
			}

			const char* source = codes.data();
			glShaderSource(_instance, 1, &source, 0);
			glCompileShader(_instance);

			GLint result = GL_FALSE;
			glGetShaderiv(_instance, GL_COMPILE_STATUS, &result);
			if (GL_FALSE == result)
			{
				GLint length = 0;
				glGetShaderiv(_instance, GL_INFO_LOG_LENGTH, &length);

				std::string log((std::size_t)length, 0);
				glGetShaderInfoLog(_instance, length, &length, (char*)log.data());

				this->getDevice()->downcast<OGLDevice>()->message(log.c_str());
				return false;
			}

			_shaderDesc = shaderDesc;
			return true;
		}

		void
		OGLShader::close() noexcept
		{
			if (_instance != GL_NONE)
			{
				glDeleteShader(_instance);
				_instance = GL_NONE;
			}
		}

		GLuint
		OGLShader::getInstanceID() const noexcept
		{
			return _instance;
		}

		bool
		OGLShader::HlslCodes2GLSL(GraphicsShaderStageFlags stage, const std::string& codes, const std::string& main, std::string& out)
		{
		#if defined(OCTOON_BUILD_PLATFORM_WINDOWS)
			const char* profile;
			if (stage == GraphicsShaderStageFlagBits::GraphicsShaderStageVertexBit)
				profile = "vs_4_0";
			else if (stage == GraphicsShaderStageFlagBits::GraphicsShaderStageFragmentBit)
				profile = "ps_4_0";
			else if (stage == GraphicsShaderStageFlagBits::GraphicsShaderStageGeometryBit)
				profile = "gs_4_0";
			else if (stage == GraphicsShaderStageFlagBits::GraphicsShaderStageComputeBit)
				profile = "cs_4_0";

			ID3DBlob* binary = nullptr;
			ID3DBlob* error = nullptr;

			D3DCreateBlob(4096, &binary);
			D3DCreateBlob(4096, &error);

			try
			{
				HRESULT hr = D3DCompile(codes.data(), codes.size(), 0, 0, 0, main.c_str(), profile, D3DCOMPILE_SKIP_VALIDATION | D3DCOMPILE_OPTIMIZATION_LEVEL3, 0, &binary, &error);
				if (hr != S_OK)
				{
					std::string line;
					std::size_t index = 1;
					std::ostringstream ostream;
					std::istringstream istream(codes);

					ostream << (const char*)error->GetBufferPointer() << std::endl;
					while (std::getline(istream, line))
					{
						ostream << index << '\t' << line << std::endl;
						index++;
					}

					this->getDevice()->downcast<OGLDevice>()->message(ostream.str().c_str());
				}
				else
				{
					if (!HlslByteCodes2GLSL(stage, (char*)binary->GetBufferPointer(), out))
						hr = S_FALSE;
				}

				if (binary)
					binary->Release();

				if (error)
					error->Release();

				return hr == S_OK ? true : false;
			}
			catch (...)
			{
				if (binary)
					binary->Release();

				if (error)
					error->Release();

				return false;
			}
		#else
			return false;
		#endif
		}

		bool
		OGLShader::HlslByteCodes2GLSL(GraphicsShaderStageFlags stage, const char* codes, std::string& out)
		{
			std::uint32_t flags = HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS | HLSLCC_FLAG_INOUT_APPEND_SEMANTIC_NAMES | HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT;
			if (stage == GraphicsShaderStageFlagBits::GraphicsShaderStageGeometryBit)
				flags = HLSLCC_FLAG_GS_ENABLED;
			else if (stage == GraphicsShaderStageFlagBits::GraphicsShaderStageTessControlBit)
				flags = HLSLCC_FLAG_TESS_ENABLED;
			else if (stage == GraphicsShaderStageFlagBits::GraphicsShaderStageTessEvaluationBit)
				flags = HLSLCC_FLAG_TESS_ENABLED;

			GLSLShader shader;
			GLSLCrossDependencyData dependency;

			try
			{
				if (!TranslateHLSLFromMem(codes, flags, GLLang::LANG_DEFAULT, 0, nullptr, &dependency, &shader))
				{
					FreeGLSLShader(&shader);
					return false;
				}

				out = shader.sourceCode;
				FreeGLSLShader(&shader);
				return true;
			}
			catch (...)
			{
				FreeGLSLShader(&shader);
				return false;
			}
		}

		const GraphicsShaderDesc&
		OGLShader::getGraphicsShaderDesc() const noexcept
		{
			return _shaderDesc;
		}

		void
		OGLShader::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLShader::getDevice() noexcept
		{
			return _device.lock();
		}

		OGLProgram::OGLProgram() noexcept
			: _program(GL_NONE)
		{
		}

		OGLProgram::~OGLProgram() noexcept
		{
			this->close();
		}

		bool
		OGLProgram::setup(const GraphicsProgramDesc& programDesc) noexcept
		{
			assert(_program == GL_NONE);

			if (programDesc.getShaders().empty())
				return false;

			_program = glCreateProgram();
			if (_program == GL_NONE)
			{
				this->getDevice()->downcast<OGLDevice>()->message("glCreateProgram() fail");
				return false;
			}

			for (auto& shader : programDesc.getShaders())
			{
				auto glshader = shader->downcast<OGLShader>();
				if (glshader)
					glAttachShader(_program, glshader->getInstanceID());
			}

			glLinkProgram(_program);

			GLint status = GL_FALSE;
			glGetProgramiv(_program, GL_LINK_STATUS, &status);
			if (!status)
			{
				GLint length = 0;
				glGetProgramiv(_program, GL_INFO_LOG_LENGTH, &length);

				std::string log((std::size_t)length, 0);
				glGetProgramInfoLog(_program, length, &length, (GLchar*)log.data());

				this->getDevice()->downcast<OGLDevice>()->message(log.c_str());
				return false;
			}

			_initActiveAttribute();
			_initActiveUniform();
			_initActiveUniformBlock();

			_programDesc = programDesc;
			return true;
		}

		void
		OGLProgram::close() noexcept
		{
			if (_program != GL_NONE)
			{
				glDeleteProgram(_program);
				_program = GL_NONE;
			}

			_activeAttributes.clear();
			_activeParams.clear();
		}

		void
		OGLProgram::apply() noexcept
		{
			glUseProgram(_program);
		}

		GLuint
		OGLProgram::getInstanceID() const noexcept
		{
			return _program;
		}

		const GraphicsAttributes&
		OGLProgram::getActiveAttributes() const noexcept
		{
			return _activeAttributes;
		}

		const GraphicsParams&
		OGLProgram::getActiveParams() const noexcept
		{
			return _activeParams;
		}

		void
		OGLProgram::_initActiveAttribute() noexcept
		{
			GLint numAttribute = 0;
			GLint maxAttribute = 0;

			glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numAttribute);
			glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribute);

			if (numAttribute > 0)
			{
				auto nameAttribute = std::make_unique<GLchar[]>(maxAttribute + 1);
				nameAttribute[maxAttribute] = 0;

				for (GLint i = 0; i < numAttribute; ++i)
				{
					GLint size;
					GLenum type;

					glGetActiveAttrib(_program, (GLuint)i, maxAttribute, GL_NONE, &size, &type, nameAttribute.get());
					GLint location = glGetAttribLocation(_program, nameAttribute.get());

					std::string name = nameAttribute.get();
					std::string semantic;
					std::uint32_t semanticIndex = 0;

					auto it = std::find_if_not(name.rbegin(), name.rend(), [](char ch) { return ch >= '0' && ch <= '9'; });
					if (it != name.rend())
					{
						semantic = name.substr(0, name.rend() - it);
						semanticIndex = std::atoi(name.substr(name.rend() - it).c_str());
					}

					std::size_t off = semantic.find_last_of('_');
					if (off != std::string::npos)
						semantic = semantic.substr(off + 1);
					else
						semantic = semantic;

					auto attrib = std::make_shared<OGLGraphicsAttribute>();
					attrib->setSemantic(semantic);
					attrib->setSemanticIndex(semanticIndex);
					attrib->setBindingPoint(location);
					attrib->setType(toGraphicsFormat(type));

					_activeAttributes.push_back(attrib);
				}
			}
		}

		void
		OGLProgram::_initActiveUniform() noexcept
		{
			GLint numUniform = 0;
			GLint maxUniformLength = 0;

			glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numUniform);
			glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLength);

			if (numUniform == 0)
				return;

			std::string nameUniform(maxUniformLength, 0);

			GLuint textureUnit = 0;
			for (GLint i = 0; i < numUniform; ++i)
			{
				GLint size;
				GLenum type;
				GLsizei length;
				glGetActiveUniform(_program, (GLuint)i, maxUniformLength, &length, &size, &type, (GLchar*)nameUniform.c_str());

				GLuint location = glGetUniformLocation(_program, nameUniform.c_str());
				if (location == GL_INVALID_INDEX)
					continue;

				auto uniform = std::make_shared<OGLGraphicsUniform>();
				uniform->setName(nameUniform.substr(0, std::min((std::size_t)length, nameUniform.find('['))));
				uniform->setBindingPoint(location);
				uniform->setType(toGraphicsUniformType(nameUniform, type));
				uniform->setShaderStageFlags(GraphicsShaderStageFlagBits::GraphicsShaderStageAll);

				if (type == GL_SAMPLER_2D ||
					type == GL_SAMPLER_3D ||
					type == GL_SAMPLER_2D_ARRAY ||
					type == GL_SAMPLER_CUBE ||
					type == GL_SAMPLER_CUBE_MAP_ARRAY)
				{
					auto pos = nameUniform.find_first_of("_X_");
					if (pos != std::string::npos)
					{
						uniform->setName(nameUniform.substr(0, pos));
						uniform->setSamplerName(nameUniform.substr(pos + 3));
					}

					glProgramUniform1i(_program, location, textureUnit);
					uniform->setBindingPoint(textureUnit);
					textureUnit++;
				}

				_activeParams.push_back(uniform);
			}
		}

		void
		OGLProgram::_initActiveUniformBlock() noexcept
		{
			GLint numUniformBlock = 0;
			GLint maxUniformBlockLength = 0;
			GLint maxUniformLength = 0;

			glGetProgramiv(_program, GL_ACTIVE_UNIFORM_BLOCKS, &numUniformBlock);
			glGetProgramiv(_program, GL_ACTIVE_UNIFORM_BLOCK_MAX_NAME_LENGTH, &maxUniformBlockLength);
			glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLength);

			if (numUniformBlock == 0)
				return;

			auto nameUniformBlock = std::make_unique<GLchar[]>(maxUniformBlockLength + 1);
			nameUniformBlock[maxUniformBlockLength] = 0;

			for (GLint i = 0; i < numUniformBlock; ++i)
			{
				GLsizei lengthUniformBlock = 0;
				glGetActiveUniformBlockName(_program, (GLuint)i, maxUniformBlockLength, &lengthUniformBlock, nameUniformBlock.get());
				if (lengthUniformBlock == 0)
					continue;

				GLuint location = glGetUniformBlockIndex(_program, nameUniformBlock.get());
				if (location == GL_INVALID_INDEX)
					continue;

				glUniformBlockBinding(_program, location, i);

				GLint count = 0;
				glGetActiveUniformBlockiv(_program, location, GL_UNIFORM_BLOCK_ACTIVE_UNIFORMS, &count);
				if (count == 0)
					continue;

				GLint size = 0;
				std::vector<GLint> indices(count);
				std::vector<GLint> offset((std::size_t)count);
				std::vector<GLint> type((std::size_t)count);
				std::vector<GLint> datasize((std::size_t)count);
				std::vector<GLchar> name(maxUniformLength);

				glGetActiveUniformBlockiv(_program, location, GL_UNIFORM_BLOCK_DATA_SIZE, &size);
				glGetActiveUniformBlockiv(_program, location, GL_UNIFORM_BLOCK_ACTIVE_UNIFORM_INDICES, indices.data());
				glGetActiveUniformsiv(_program, count, (GLuint*)&indices[0], GL_UNIFORM_OFFSET, &offset[0]);
				glGetActiveUniformsiv(_program, count, (GLuint*)&indices[0], GL_UNIFORM_TYPE, &type[0]);
				glGetActiveUniformsiv(_program, count, (GLuint*)&indices[0], GL_UNIFORM_SIZE, &datasize[0]);

				auto uniformblock = std::make_shared<OGLGraphicsUniformBlock>();
				uniformblock->setName(nameUniformBlock.get());
				uniformblock->setBindingPoint(location);
				uniformblock->setBlockSize(size);
				uniformblock->setType(GraphicsUniformType::UniformBuffer);
				uniformblock->setShaderStageFlags(GraphicsShaderStageFlagBits::GraphicsShaderStageAll);

				for (GLint j = 0; j < count; j++)
				{
					GLsizei length = 0;
					glGetActiveUniformName(_program, indices[j], maxUniformLength, &length, name.data());

					auto uniform = std::make_shared<OGLGraphicsUniform>();
					uniform->setName(std::string(name.data(), length));
					uniform->setBindingPoint(indices[j]);
					uniform->setOffset(offset[j]);
					uniform->setType(toGraphicsUniformType(uniform->getName(), type[j]));

					uniformblock->addGraphicsUniform(uniform);
				}

				_activeParams.push_back(uniformblock);
			}
		}

		GraphicsFormat
		OGLProgram::toGraphicsFormat(GLenum type) noexcept
		{
			if (type == GL_BOOL)
				return GraphicsFormat::R8UInt;
			else if (type == GL_UNSIGNED_INT)
				return GraphicsFormat::R8UInt;
			else if (type == GL_UNSIGNED_INT_VEC2)
				return GraphicsFormat::R8G8UInt;
			else if (type == GL_UNSIGNED_INT_VEC3)
				return GraphicsFormat::R8G8B8UInt;
			else if (type == GL_UNSIGNED_INT_VEC4)
				return GraphicsFormat::R8G8B8A8UInt;
			else if (type == GL_INT)
				return GraphicsFormat::R8SInt;
			else if (type == GL_INT_VEC2)
				return GraphicsFormat::R8G8SInt;
			else if (type == GL_INT_VEC3)
				return GraphicsFormat::R8G8B8SInt;
			else if (type == GL_INT_VEC4)
				return GraphicsFormat::R8G8B8A8SInt;
			else if (type == GL_FLOAT)
				return GraphicsFormat::R32SFloat;
			else if (type == GL_FLOAT_VEC2)
				return GraphicsFormat::R32G32SFloat;
			else if (type == GL_FLOAT_VEC3)
				return GraphicsFormat::R32G32B32SFloat;
			else if (type == GL_FLOAT_VEC4)
				return GraphicsFormat::R32G32B32A32SFloat;
			else if (type == GL_FLOAT_MAT2)
				return GraphicsFormat::R32G32B32A32SFloat;
			else if (type == GL_FLOAT_MAT3)
				return GraphicsFormat::R32G32B32A32SFloat;
			else if (type == GL_FLOAT_MAT4)
				return GraphicsFormat::R32G32B32A32SFloat;
			else
			{
				return GraphicsFormat::Undefined;
			}
		}

		GraphicsUniformType
		OGLProgram::toGraphicsUniformType(const std::string& name, GLenum type) noexcept
		{
			if (type == GL_SAMPLER_2D ||
				type == GL_SAMPLER_3D ||
				type == GL_SAMPLER_2D_ARRAY ||
				type == GL_SAMPLER_CUBE ||
				type == GL_SAMPLER_CUBE_MAP_ARRAY)
			{
				return GraphicsUniformType::SamplerImage;
			}
			else
			{
				bool isArray = strstr(name.c_str(), "[") != nullptr;
				if (type == GL_BOOL)
				{
					return GraphicsUniformType::Bool;
				}
				else if (type == GL_UNSIGNED_INT)
				{
					if (isArray)
						return GraphicsUniformType::UIntArray;
					else
						return GraphicsUniformType::UInt;
				}
				else if (type == GL_UNSIGNED_INT_VEC2)
				{
					if (isArray)
						return GraphicsUniformType::UInt2Array;
					else
						return GraphicsUniformType::UInt2;
				}
				else if (type == GL_UNSIGNED_INT_VEC3)
				{
					if (isArray)
						return GraphicsUniformType::UInt3Array;
					else
						return GraphicsUniformType::UInt3;
				}
				else if (type == GL_UNSIGNED_INT_VEC4)
				{
					if (isArray)
						return GraphicsUniformType::UInt4Array;
					else
						return GraphicsUniformType::UInt4;
				}
				else if (type == GL_INT)
				{
					if (isArray)
						return GraphicsUniformType::IntArray;
					else
						return GraphicsUniformType::Int;
				}
				else if (type == GL_INT_VEC2)
				{
					if (isArray)
						return GraphicsUniformType::Int2Array;
					else
						return GraphicsUniformType::Int2;
				}
				else if (type == GL_INT_VEC3)
				{
					if (isArray)
						return GraphicsUniformType::Int3Array;
					else
						return GraphicsUniformType::Int3;
				}
				else if (type == GL_INT_VEC4)
				{
					if (isArray)
						return GraphicsUniformType::Int4Array;
					else
						return GraphicsUniformType::Int4;
				}
				else if (type == GL_FLOAT)
				{
					if (isArray)
						return GraphicsUniformType::FloatArray;
					else
						return GraphicsUniformType::Float;
				}
				else if (type == GL_FLOAT_VEC2)
				{
					if (isArray)
						return GraphicsUniformType::Float2Array;
					else
						return GraphicsUniformType::Float2;
				}
				else if (type == GL_FLOAT_VEC3)
				{
					if (isArray)
						return GraphicsUniformType::Float3Array;
					else
						return GraphicsUniformType::Float3;
				}
				else if (type == GL_FLOAT_VEC4)
				{
					if (isArray)
						return GraphicsUniformType::Float4Array;
					else
						return GraphicsUniformType::Float4;
				}
				else if (type == GL_FLOAT_MAT2)
				{
					if (isArray)
						return GraphicsUniformType::Float2x2Array;
					else
						return GraphicsUniformType::Float2x2;
				}
				else if (type == GL_FLOAT_MAT3)
				{
					if (isArray)
						return GraphicsUniformType::Float3x3Array;
					else
						return GraphicsUniformType::Float3x3;
				}
				else if (type == GL_FLOAT_MAT4)
				{
					if (isArray)
						return GraphicsUniformType::Float4x4Array;
					else
						return GraphicsUniformType::Float4x4;
				}
				else
				{
					assert(false);
					return GraphicsUniformType::None;
				}
			}
		}

		const GraphicsProgramDesc&
		OGLProgram::getGraphicsProgramDesc() const noexcept
		{
			return _programDesc;
		}

		void
		OGLProgram::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		OGLProgram::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}