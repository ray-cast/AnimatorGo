#include "gl33_shader.h"
#include "gl33_device.h"

#if defined(__WINDOWS__) && defined(OCTOON_FEATURE_HAL_USE_HLSL)
#	define EXCLUDE_PSTDINT
#	include <hlslcc.hpp>

#	include <sstream>
#	include <d3dcompiler.h>
#endif

#include <cstring>

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubClass(GL33Shader, GraphicsShader, "GL33Shader")
		OctoonImplementSubClass(GL33Program, GraphicsProgram, "GL33Program")
		OctoonImplementSubClass(GL33GraphicsAttribute, GraphicsAttribute, "GL33GraphicsAttribute")
		OctoonImplementSubClass(GL33GraphicsUniform, GraphicsUniform, "GL33GraphicsUniform")
		OctoonImplementSubClass(GL33GraphicsUniformBlock, GraphicsUniformBlock, "GL33GraphicsUniformBlock")

		GL33GraphicsAttribute::GL33GraphicsAttribute() noexcept
			: _semanticIndex(0)
			, _bindingPoint(GL_INVALID_INDEX)
			, _type(GraphicsFormat::Undefined)
		{
		}

		GL33GraphicsAttribute::~GL33GraphicsAttribute() noexcept
		{
		}

		void
		GL33GraphicsAttribute::setSemantic(std::string_view semantic) noexcept
		{
			_semantic = semantic;
		}

		const std::string&
		GL33GraphicsAttribute::getSemantic() const noexcept
		{
			return _semantic;
		}

		void
		GL33GraphicsAttribute::setSemanticIndex(std::uint32_t index) noexcept
		{
			_semanticIndex = index;
		}

		std::uint32_t
		GL33GraphicsAttribute::getSemanticIndex() const noexcept
		{
			return _semanticIndex;
		}

		void
		GL33GraphicsAttribute::setType(GraphicsFormat type) noexcept
		{
			_type = type;
		}

		GraphicsFormat
		GL33GraphicsAttribute::getType() const noexcept
		{
			return _type;
		}

		void
		GL33GraphicsAttribute::setBindingPoint(std::uint32_t bindingPoint) noexcept
		{
			_bindingPoint = bindingPoint;
		}

		std::uint32_t
		GL33GraphicsAttribute::getBindingPoint() const noexcept
		{
			return _bindingPoint;
		}

		GL33GraphicsUniform::GL33GraphicsUniform() noexcept
			: _offset(0)
			, _bindingPoint(GL_INVALID_INDEX)
			, _type(GraphicsUniformType::Null)
			, _stageFlags(0)
		{
		}

		GL33GraphicsUniform::~GL33GraphicsUniform() noexcept
		{
		}

		void
		GL33GraphicsUniform::setName(std::string_view name) noexcept
		{
			_name = name;
		}

		const std::string&
		GL33GraphicsUniform::getName() const noexcept
		{
			return _name;
		}

		void
		GL33GraphicsUniform::setSamplerName(std::string_view name) noexcept
		{
			_samplerName = name;
		}

		const std::string&
		GL33GraphicsUniform::getSamplerName() const noexcept
		{
			return _samplerName;
		}

		void
		GL33GraphicsUniform::setType(GraphicsUniformType type) noexcept
		{
			_type = type;
		}

		GraphicsUniformType
		GL33GraphicsUniform::getType() const noexcept
		{
			return _type;
		}

		void
		GL33GraphicsUniform::setOffset(std::uint32_t offset) noexcept
		{
			_offset = offset;
		}

		std::uint32_t
		GL33GraphicsUniform::getOffset() const noexcept
		{
			return _offset;
		}

		void
		GL33GraphicsUniform::setBindingPoint(std::uint32_t bindingPoint) noexcept
		{
			_bindingPoint = bindingPoint;
		}

		std::uint32_t
		GL33GraphicsUniform::getBindingPoint() const noexcept
		{
			return _bindingPoint;
		}

		void
		GL33GraphicsUniform::setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept
		{
			_stageFlags = flags;
		}

		GraphicsShaderStageFlags
		GL33GraphicsUniform::getShaderStageFlags() const noexcept
		{
			return _stageFlags;
		}

		GL33GraphicsUniformBlock::GL33GraphicsUniformBlock() noexcept
			: _size(0)
			, _bindingPoint(GL_INVALID_INDEX)
			, _type(GraphicsUniformType::UniformBuffer)
			, _stageFlags(0)
		{
		}

		GL33GraphicsUniformBlock::~GL33GraphicsUniformBlock() noexcept
		{
		}

		void
		GL33GraphicsUniformBlock::setName(std::string_view name) noexcept
		{
			_name = name;
		}

		const std::string&
		GL33GraphicsUniformBlock::getName() const noexcept
		{
			return _name;
		}

		void
		GL33GraphicsUniformBlock::setType(GraphicsUniformType type) noexcept
		{
			_type = type;
		}

		GraphicsUniformType
		GL33GraphicsUniformBlock::getType() const noexcept
		{
			return _type;
		}

		void
		GL33GraphicsUniformBlock::setBlockSize(std::uint32_t size) noexcept
		{
			_size = size;
		}

		std::uint32_t
		GL33GraphicsUniformBlock::getBlockSize() const noexcept
		{
			return _size;
		}

		void
		GL33GraphicsUniformBlock::setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept
		{
			_stageFlags = flags;
		}

		GraphicsShaderStageFlags
		GL33GraphicsUniformBlock::getShaderStageFlags() const noexcept
		{
			return _stageFlags;
		}

		void
		GL33GraphicsUniformBlock::addGraphicsUniform(GraphicsUniformPtr uniform) noexcept
		{
			_uniforms.push_back(uniform);
		}

		void
		GL33GraphicsUniformBlock::removeGraphicsUniform(GraphicsUniformPtr uniform) noexcept
		{
			auto it = std::find(_uniforms.begin(), _uniforms.end(), uniform);
			if (it != _uniforms.end())
				_uniforms.erase(it);
		}

		const GraphicsUniforms&
		GL33GraphicsUniformBlock::getGraphicsUniforms() const noexcept
		{
			return _uniforms;
		}

		void
		GL33GraphicsUniformBlock::setBindingPoint(std::uint32_t bindingPoint) noexcept
		{
			_bindingPoint = bindingPoint;
		}

		std::uint32_t
		GL33GraphicsUniformBlock::getBindingPoint() const noexcept
		{
			return _bindingPoint;
		}

		GL33Shader::GL33Shader() noexcept
			: _instance(GL_NONE)
		{
		}

		GL33Shader::~GL33Shader() noexcept
		{
			this->close();
		}

		bool
		GL33Shader::setup(const GraphicsShaderDesc& shaderDesc) noexcept
		{
			assert(!_instance);
			assert(!shaderDesc.getByteCodes().empty());

			auto type = GL33Types::asShaderStage(shaderDesc.getStage());
			if (type == GL_INVALID_ENUM)
			{
				this->getDevice()->downcast<GL33Device>()->message("Invalid shader type");
				return false;
			}

			_instance = glCreateShader(type);
			if (_instance == GL_NONE)
			{
				this->getDevice()->downcast<GL33Device>()->message("glCreateShader() fail.");
				return false;
			}

			std::string codes;
			const char* source = shaderDesc.getByteCodes().c_str();

			if (shaderDesc.getLanguage() == GraphicsShaderLang::HLSL)
			{
#if defined(__WINDOWS__) && defined(OCTOON_FEATURE_HAL_USE_HLSL)
				if (!HlslCodes2GLSL(shaderDesc.getStage(), shaderDesc.getByteCodes().data(), shaderDesc.getEntryPoint().data(), codes))
				{
					this->getDevice()->downcast<GL33Device>()->message("Can't conv hlsl to glsl.");
					return false;
				}

				source = codes.data();
#else
				this->getDevice()->downcast<GL33Device>()->message("Can't conv hlslbytecodes to glsl.");
				return false;
#endif
			}
			else if (shaderDesc.getLanguage() == GraphicsShaderLang::HLSLbytecodes)
			{
#if defined(__WINDOWS__) && defined(OCTOON_FEATURE_HAL_USE_HLSL)
				if (!HlslByteCodes2GLSL(shaderDesc.getStage(), shaderDesc.getByteCodes().data(), codes))
				{
					this->getDevice()->downcast<GL33Device>()->message("Can't conv hlslbytecodes to glsl.");
					return false;
				}

				source = codes.data();
#else
				this->getDevice()->downcast<GL33Device>()->message("Can't conv hlslbytecodes to glsl.");
				return false;
#endif
			}

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

				this->getDevice()->downcast<GL33Device>()->message(log.c_str());
				return false;
			}

			_shaderDesc = shaderDesc;
			return true;
		}

		void
		GL33Shader::close() noexcept
		{
			if (_instance != GL_NONE)
			{
				glDeleteShader(_instance);
				_instance = GL_NONE;
			}
		}

		GLuint
		GL33Shader::getInstanceID() const noexcept
		{
			return _instance;
		}

		bool
		GL33Shader::HlslCodes2GLSL(GraphicsShaderStageFlags stage, const std::string& codes, const std::string& main, std::string& out)
		{
		#if defined(OCTOON_BUILD_PLATFORM_WINDOWS) && defined(OCTOON_FEATURE_HAL_USE_HLSL)
			const char* profile = nullptr;
			if (stage == GraphicsShaderStageFlagBits::VertexBit)
				profile = "vs_4_0";
			else if (stage == GraphicsShaderStageFlagBits::FragmentBit)
				profile = "ps_4_0";
			else if (stage == GraphicsShaderStageFlagBits::GeometryBit)
				profile = "gs_4_0";
			else if (stage == GraphicsShaderStageFlagBits::ComputeBit)
				profile = "cs_4_0";
			else
				assert(false);

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

					this->getDevice()->downcast<GL33Device>()->message(ostream.str().c_str());
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
		GL33Shader::HlslByteCodes2GLSL(GraphicsShaderStageFlags stage, const char* codes, std::string& out)
		{
#if defined(OCTOON_BUILD_PLATFORM_WINDOWS) && defined(OCTOON_FEATURE_HAL_USE_HLSL)
			std::uint32_t flags = HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS | HLSLCC_FLAG_INOUT_APPEND_SEMANTIC_NAMES | HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT;
			if (stage == GraphicsShaderStageFlagBits::GeometryBit)
				flags = HLSLCC_FLAG_GS_ENABLED;
			else if (stage == GraphicsShaderStageFlagBits::TessControlBit)
				flags = HLSLCC_FLAG_TESS_ENABLED;
			else if (stage == GraphicsShaderStageFlagBits::TessEvaluationBit)
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
#else
			return false;
#endif
		}

		const GraphicsShaderDesc&
		GL33Shader::getShaderDesc() const noexcept
		{
			return _shaderDesc;
		}

		void
		GL33Shader::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33Shader::getDevice() noexcept
		{
			return _device.lock();
		}

		GL33Program::GL33Program() noexcept
			: _program(GL_NONE)
		{
		}

		GL33Program::~GL33Program() noexcept
		{
			this->close();
		}

		bool
		GL33Program::setup(const GraphicsProgramDesc& programDesc) noexcept
		{
			assert(_program == GL_NONE);

			if (programDesc.getShaders().empty())
				return false;

			_program = glCreateProgram();
			if (_program == GL_NONE)
			{
				this->getDevice()->downcast<GL33Device>()->message("glCreateProgram() fail");
				return false;
			}

			for (auto& shader : programDesc.getShaders())
			{
				auto glshader = shader->downcast<GL33Shader>();
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

				this->getDevice()->downcast<GL33Device>()->message(log.c_str());
				return false;
			}

			_initActiveAttribute();
			_initActiveUniform();
			_initActiveUniformBlock();

			_programDesc = programDesc;
			return true;
		}

		void
		GL33Program::close() noexcept
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
		GL33Program::apply() noexcept
		{
			glUseProgram(_program);
		}

		GLuint
		GL33Program::getInstanceID() const noexcept
		{
			return _program;
		}

		const GraphicsAttributes&
		GL33Program::getActiveAttributes() const noexcept
		{
			return _activeAttributes;
		}

		const GraphicsParams&
		GL33Program::getActiveParams() const noexcept
		{
			return _activeParams;
		}

		void
		GL33Program::_initActiveAttribute() noexcept
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
						semanticIndex = std::stoi(name.substr(name.rend() - it));
					}

					std::size_t off = semantic.find_last_of('_');
					if (off != std::string::npos)
						semantic = semantic.substr(off + 1);
					else
						semantic = semantic;

					auto attrib = std::make_shared<GL33GraphicsAttribute>();
					attrib->setSemantic(semantic);
					attrib->setSemanticIndex(semanticIndex);
					attrib->setBindingPoint(location);
					attrib->setType(toGraphicsFormat(type));

					_activeAttributes.push_back(attrib);
				}
			}
		}

		void
		GL33Program::_initActiveUniform() noexcept
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
				std::memset(nameUniform.data(), 0, maxUniformLength);

				GLint size;
				GLenum type;
				GLsizei length;
				glGetActiveUniform(_program, (GLuint)i, maxUniformLength, &length, &size, &type, (GLchar*)nameUniform.c_str());

				GLuint location = glGetUniformLocation(_program, nameUniform.c_str());
				if (location == GL_INVALID_INDEX)
					continue;

				auto uniform = std::make_shared<GL33GraphicsUniform>();
				uniform->setName(nameUniform.substr(0, length));
				uniform->setBindingPoint(location);
				uniform->setType(toGraphicsUniformType(nameUniform, type));
				uniform->setShaderStageFlags(GraphicsShaderStageFlagBits::All);

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
		GL33Program::_initActiveUniformBlock() noexcept
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

				auto uniformblock = std::make_shared<GL33GraphicsUniformBlock>();
				uniformblock->setName(nameUniformBlock.get());
				uniformblock->setBindingPoint(location);
				uniformblock->setBlockSize(size);
				uniformblock->setType(GraphicsUniformType::UniformBuffer);
				uniformblock->setShaderStageFlags(GraphicsShaderStageFlagBits::All);

				for (GLint j = 0; j < count; j++)
				{
					GLsizei length = 0;
					glGetActiveUniformName(_program, indices[j], maxUniformLength, &length, name.data());

					auto uniform = std::make_shared<GL33GraphicsUniform>();
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
		GL33Program::toGraphicsFormat(GLenum type) noexcept
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
		GL33Program::toGraphicsUniformType(std::string_view name, GLenum type) noexcept
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
				bool isArray = false;
				if (type == GL_BOOL)
				{
					return GraphicsUniformType::Boolean;
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
					return GraphicsUniformType::Null;
				}
			}
		}

		const GraphicsProgramDesc&
		GL33Program::getProgramDesc() const noexcept
		{
			return _programDesc;
		}

		void
		GL33Program::setDevice(const GraphicsDevicePtr& device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		GL33Program::getDevice() noexcept
		{
			return _device.lock();
		}
	}
}