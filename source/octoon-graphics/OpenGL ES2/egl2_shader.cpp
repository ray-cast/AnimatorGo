#include "egl2_shader.h"
#include "egl2_device.h"

#if defined(__WINDOWS__) && defined(OCTOON_FEATURE_GRAPHICS_USE_HLSL)
#	define EXCLUDE_PSTDINT
#	include <hlslcc.hpp>

#	include <sstream>
#	include <d3dcompiler.h>
#endif

namespace octoon 
{
	namespace graphics
	{
		OctoonImplementSubClass(EGL2Shader, GraphicsShader, "EGL2Shader")
		OctoonImplementSubClass(EGL2Program, GraphicsProgram, "EGL2Program")
		OctoonImplementSubClass(EGL2GraphicsAttribute, GraphicsAttribute, "EGL2GraphicsAttribute")
		OctoonImplementSubClass(EGL2GraphicsUniform, GraphicsUniform, "EGL2GraphicsUniform")

		EGL2GraphicsAttribute::EGL2GraphicsAttribute() noexcept
			: _semanticIndex(0)
			, _bindingPoint(GL_INVALID_INDEX)
			, _type(GraphicsFormat::Undefined)
		{
		}

		EGL2GraphicsAttribute::~EGL2GraphicsAttribute() noexcept
		{
		}

		void
		EGL2GraphicsAttribute::setSemantic(const std::string& semantic) noexcept
		{
			_semantic = semantic;
		}

		const std::string&
		EGL2GraphicsAttribute::getSemantic() const noexcept
		{
			return _semantic;
		}

		void
		EGL2GraphicsAttribute::setSemanticIndex(std::uint32_t index) noexcept
		{
			_semanticIndex = index;
		}

		std::uint32_t
		EGL2GraphicsAttribute::getSemanticIndex() const noexcept
		{
			return _semanticIndex;
		}

		void
		EGL2GraphicsAttribute::setType(GraphicsFormat type) noexcept
		{
			_type = type;
		}

		GraphicsFormat
		EGL2GraphicsAttribute::getType() const noexcept
		{
			return _type;
		}

		void
		EGL2GraphicsAttribute::setBindingPoint(std::uint32_t bindingPoint) noexcept
		{
			_bindingPoint = bindingPoint;
		}

		std::uint32_t
		EGL2GraphicsAttribute::getBindingPoint() const noexcept
		{
			return _bindingPoint;
		}

		EGL2GraphicsUniform::EGL2GraphicsUniform() noexcept
			: _offset(0)
			, _bindingPoint(GL_INVALID_INDEX)
			, _type(GraphicsUniformType::Null)
			, _stageFlags(0)
		{
		}

		EGL2GraphicsUniform::~EGL2GraphicsUniform() noexcept
		{
		}

		void
		EGL2GraphicsUniform::setName(const std::string& name) noexcept
		{
			_name = name;
		}

		const std::string&
		EGL2GraphicsUniform::getName() const noexcept
		{
			return _name;
		}

		void
		EGL2GraphicsUniform::setSamplerName(const std::string& name) noexcept
		{
			_samplerName = name;
		}

		const std::string&
		EGL2GraphicsUniform::getSamplerName() const noexcept
		{
			return _samplerName;
		}

		void
		EGL2GraphicsUniform::setType(GraphicsUniformType type) noexcept
		{
			_type = type;
		}

		GraphicsUniformType
		EGL2GraphicsUniform::getType() const noexcept
		{
			return _type;
		}

		void
		EGL2GraphicsUniform::setOffset(std::uint32_t offset) noexcept
		{
			_offset = offset;
		}

		std::uint32_t
		EGL2GraphicsUniform::getOffset() const noexcept
		{
			return _offset;
		}

		void
		EGL2GraphicsUniform::setBindingPoint(GLuint bindingPoint) noexcept
		{
			_bindingPoint = bindingPoint;
		}

		GLuint
		EGL2GraphicsUniform::getBindingPoint() const noexcept
		{
			return _bindingPoint;
		}

		void
		EGL2GraphicsUniform::setShaderStageFlags(GraphicsShaderStageFlags flags) noexcept
		{
			_stageFlags = flags;
		}

		GraphicsShaderStageFlags
		EGL2GraphicsUniform::getShaderStageFlags() const noexcept
		{
			return _stageFlags;
		}

		EGL2Shader::EGL2Shader() noexcept
			: _instance(GL_NONE)
		{
		}

		EGL2Shader::~EGL2Shader() noexcept
		{
			this->close();
		}

		bool
		EGL2Shader::setup(const GraphicsShaderDesc& shaderDesc) noexcept
		{
			assert(_instance == GL_NONE);
			assert(shaderDesc.getByteCodes().size() > 0);
			assert(EGL2Types::asShaderStage(shaderDesc.getStage()) != GL_INVALID_ENUM);

			_instance = glCreateShader(EGL2Types::asShaderStage(shaderDesc.getStage()));
			if (_instance == GL_NONE)
			{
				GL_PLATFORM_LOG("glCreateShader() fail.");
				return false;
			}

			std::string codes = shaderDesc.getByteCodes().data();
			if (shaderDesc.getLanguage() == GraphicsShaderLang::HLSL)
			{
#if defined(__WINDOWS__) && defined(OCTOON_FEATURE_GRAPHICS_USE_HLSL)
				if (!HlslCodes2GLSL(shaderDesc.getStage(), shaderDesc.getByteCodes().data(), codes))
				{
					GL_PLATFORM_LOG("Can't conv hlsl to glsl.");
					return false;
				}
#else
				return false;
#endif
			}
			else if (shaderDesc.getLanguage() == GraphicsShaderLang::HLSLbytecodes)
			{
#if defined(__WINDOWS__) && defined(OCTOON_FEATURE_GRAPHICS_USE_HLSL)
				if (!HlslByteCodes2GLSL(shaderDesc.getStage(), shaderDesc.getByteCodes().data(), codes))
				{
					GL_PLATFORM_LOG("Can't conv hlslbytecodes to glsl.");
					return false;
				}
#else
				return false;
#endif
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

				GL_PLATFORM_LOG(log.c_str());
				return false;
			}

			_shaderDesc = shaderDesc;
			return true;
		}

		void
		EGL2Shader::close() noexcept
		{
			if (_instance != GL_NONE)
			{
				glDeleteShader(_instance);
				_instance = GL_NONE;
			}
		}

		GLuint
		EGL2Shader::getInstanceID() const noexcept
		{
			return _instance;
		}

		bool
		EGL2Shader::HlslCodes2GLSL(GraphicsShaderStageFlags stage, const std::string& codes, std::string& out)
		{
#if defined(OCTOON_BUILD_PLATFORM_WINDOWS) && defined(OCTOON_FEATURE_GRAPHICS_USE_HLSL)
			std::string profile;
			if (stage == GraphicsShaderStageFlagBits::VertexBit)
				profile = "vs_4_0";
			else if (stage == GraphicsShaderStageFlagBits::FragmentBit)
				profile = "ps_4_0";

			ID3DBlob* binary = nullptr;
			ID3DBlob* error = nullptr;

			D3DCreateBlob(4096, &binary);
			D3DCreateBlob(4096, &error);

			HRESULT hr = D3DCompile(
				codes.data(),
				codes.size(),
				nullptr,
				nullptr,
				nullptr,
				"main",
				profile.c_str(),
				D3DCOMPILE_OPTIMIZATION_LEVEL3,
				0,
				&binary,
				&error
			);

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

				GL_PLATFORM_LOG(ostream.str().c_str());
			}

			return HlslByteCodes2GLSL(stage, (char*)binary->GetBufferPointer(), out);
		#else
			return false;
		#endif
		}

		bool
		EGL2Shader::HlslByteCodes2GLSL(GraphicsShaderStageFlags stage, const char* codes, std::string& out)
		{
#if defined(OCTOON_BUILD_PLATFORM_WINDOWS) && defined(OCTOON_FEATURE_GRAPHICS_USE_HLSL)
			std::uint32_t flags = HLSLCC_FLAG_COMBINE_TEXTURE_SAMPLERS | HLSLCC_FLAG_INOUT_APPEND_SEMANTIC_NAMES | HLSLCC_FLAG_DISABLE_GLOBALS_STRUCT;
			if (stage == GraphicsShaderStageFlagBits::GeometryBit)
				flags = HLSLCC_FLAG_GS_ENABLED;
			else if (stage == GraphicsShaderStageFlagBits::TessControlBit)
				flags = HLSLCC_FLAG_TESS_ENABLED;
			else if (stage == GraphicsShaderStageFlagBits::TessEvaluationBit)
				flags = HLSLCC_FLAG_TESS_ENABLED;

			GLSLShader shader;
			GLSLCrossDependencyData dependency;
			if (!TranslateHLSLFromMem(codes, flags, GLLang::LANG_ES_300, 0, nullptr, &dependency, &shader))
			{
				FreeGLSLShader(&shader);
				return false;
			}

			if (stage == GraphicsShaderStageFlagBits::VertexBit)
			{
				glslopt_shader_type glslopt_type = glslopt_shader_type::kGlslOptShaderVertex;
				if (stage == GraphicsShaderStageFlagBits::FragmentBit)
					glslopt_type = glslopt_shader_type::kGlslOptShaderFragment;

				auto ctx = glslopt_initialize(glslopt_target::kGlslTargetOpenGLES30);
				if (ctx)
				{
					glslopt_shader* glslopt_shader = glslopt_optimize(ctx, glslopt_type, shader.sourceCode, 0);
					bool optimizeOk = glslopt_get_status(glslopt_shader);
					if (!optimizeOk)
					{
						glslopt_cleanup(ctx);
						FreeGLSLShader(&shader);
						return false;
					}

					out = glslopt_get_output(glslopt_shader);
					glslopt_cleanup(ctx);			
				}
			}
			else
			{
				out = shader.sourceCode;
			}

			out = shader.sourceCode;
	
			FreeGLSLShader(&shader);
			return true;
#else
			return false;
#endif
		}

		const GraphicsShaderDesc&
		EGL2Shader::getGraphicsShaderDesc() const noexcept
		{
			return _shaderDesc;
		}

		void
		EGL2Shader::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2Shader::getDevice() noexcept
		{
			return _device.lock();
		}

		EGL2Program::EGL2Program() noexcept
			: _program(GL_NONE)
		{
		}

		EGL2Program::~EGL2Program() noexcept
		{
			this->close();
		}

		bool
		EGL2Program::setup(const GraphicsProgramDesc& programDesc) noexcept
		{
			assert(_program == GL_NONE);

			_program = glCreateProgram();

			for (auto& shader : programDesc.getShaders())
			{
				assert(shader->isInstanceOf<EGL2Shader>());

				auto glshader = shader->downcast<EGL2Shader>();
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

				GL_PLATFORM_LOG(log.c_str());
				return false;
			}

			glUseProgram(_program);

			_initActiveAttribute();
			_initActiveUniform();

			glUseProgram(GL_NONE);

			_programDesc = programDesc;
			return true;
		}

		void
		EGL2Program::close() noexcept
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
		EGL2Program::apply() noexcept
		{
			glUseProgram(_program);
		}

		GLuint
		EGL2Program::getInstanceID() const noexcept
		{
			return _program;
		}

		const GraphicsParams&
		EGL2Program::getActiveParams() const noexcept
		{
			return _activeParams;
		}

		const GraphicsAttributes&
		EGL2Program::getActiveAttributes() const noexcept
		{
			return _activeAttributes;
		}

		void
		EGL2Program::_initActiveAttribute() noexcept
		{
			GLint numAttribute = 0;
			GLint maxAttribute = 0;

			glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTES, &numAttribute);
			glGetProgramiv(_program, GL_ACTIVE_ATTRIBUTE_MAX_LENGTH, &maxAttribute);

			if (numAttribute)
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

					auto attrib = std::make_shared<EGL2GraphicsAttribute>();
					attrib->setSemantic(semantic);
					attrib->setSemanticIndex(semanticIndex);
					attrib->setBindingPoint(location);
					attrib->setType(toGraphicsFormat(type));

					_activeAttributes.push_back(attrib);
				}
			}
		}

		void
		EGL2Program::_initActiveUniform() noexcept
		{
			GLint numUniform = 0;
			GLint maxUniformLength = 0;

			glGetProgramiv(_program, GL_ACTIVE_UNIFORMS, &numUniform);
			glGetProgramiv(_program, GL_ACTIVE_UNIFORM_MAX_LENGTH, &maxUniformLength);

			if (numUniform == 0)
				return;

			std::string nameUniform(maxUniformLength + 1, 0);

			GLint textureUnit = 0;
			for (GLint i = 0; i < numUniform; ++i)
			{
				GLint size;
				GLenum type;
				GLsizei length;
				glGetActiveUniform(_program, (GLuint)i, maxUniformLength, &length, &size, &type, (GLchar*)nameUniform.c_str());

				GLint location = glGetUniformLocation(_program, nameUniform.c_str());
				if (location == GL_INVALID_INDEX)
					continue;

				auto uniform = std::make_shared<EGL2GraphicsUniform>();
				uniform->setName(nameUniform.substr(0, std::min((std::size_t)length, nameUniform.find('['))));
				uniform->setBindingPoint(location);
				uniform->setType(toGraphicsUniformType(nameUniform, type));
				uniform->setShaderStageFlags(GraphicsShaderStageFlagBits::All);

				if (type == GL_SAMPLER_2D ||
					type == GL_SAMPLER_3D_OES ||
					type == GL_SAMPLER_CUBE ||
					type == GL_SAMPLER_CUBE_MAP_ARRAY_EXT)
				{
					auto pos = nameUniform.find_first_of("_X_");
					if (pos != std::string::npos)
					{
						uniform->setName(nameUniform.substr(0, pos));
						uniform->setSamplerName(nameUniform.substr(pos + 3));
					}

					glUniform1i(location, textureUnit);
					uniform->setBindingPoint(textureUnit);
					uniform->setShaderStageFlags(GraphicsShaderStageFlagBits::All);

					textureUnit++;
				}

				_activeParams.push_back(uniform);
			}
		}

		GraphicsFormat
		EGL2Program::toGraphicsFormat(GLenum type) noexcept
		{
			if (type == GL_BOOL)
				return GraphicsFormat::R8UInt;
			else if (type == GL_UNSIGNED_INT)
				return GraphicsFormat::R8UInt;
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
				GL_PLATFORM_ASSERT(false, "Invlid uniform type");
				return GraphicsFormat::Undefined;
			}
		}

		GraphicsUniformType
		EGL2Program::toGraphicsUniformType(const std::string& name, GLenum type) noexcept
		{
			if (type == GL_SAMPLER_2D || type == GL_SAMPLER_CUBE)
			{
				return GraphicsUniformType::SamplerImage;
			}
			else
			{
				bool isArray = name.find("[0]") != std::string::npos;

				if (type == GL_BOOL)
				{
					return GraphicsUniformType::Boolean;
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
					GL_PLATFORM_ASSERT(false, "Invlid uniform type");
					return GraphicsUniformType::Null;
				}
			}
		}

		const GraphicsProgramDesc&
		EGL2Program::getGraphicsProgramDesc() const noexcept
		{
			return _programDesc;
		}

		void
		EGL2Program::setDevice(GraphicsDevicePtr device) noexcept
		{
			_device = device;
		}

		GraphicsDevicePtr
		EGL2Program::getDevice() noexcept
		{
			return _device.lock();
		}

	}
}
