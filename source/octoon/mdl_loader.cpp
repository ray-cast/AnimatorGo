#include <octoon/mdl_loader.h>
#include <mi/mdl_sdk.h>
#include <regex>

#ifdef MI_PLATFORM_WINDOWS
#	ifdef UNICODE
#		define FMT_LPTSTR "%ls"
#	else
#		define FMT_LPTSTR "%s"
#	endif
#endif

#define MDL_MAX_TEXTURES 32
const std::string MDL_USE_TEXTURE_PREFIX = "VisRTX_use_";

namespace std
{
	template<class BidirIt, class Traits, class CharT, class UnaryFunction>
	std::basic_string<CharT> regex_replace(BidirIt first, BidirIt last, const std::basic_regex<CharT, Traits>& re, UnaryFunction f)
	{
		std::basic_string<CharT> s;

		typename std::match_results<BidirIt>::difference_type positionOfLastMatch = 0;
		auto endOfLastMatch = first;

		auto callback = [&](const std::match_results<BidirIt>& match)
		{
			auto positionOfThisMatch = match.position(0);
			auto diff = positionOfThisMatch - positionOfLastMatch;

			auto startOfThisMatch = endOfLastMatch;
			std::advance(startOfThisMatch, diff);

			s.append(endOfLastMatch, startOfThisMatch);
			s.append(f(match));

			auto lengthOfMatch = match.length(0);

			positionOfLastMatch = positionOfThisMatch + lengthOfMatch;

			endOfLastMatch = startOfThisMatch;
			std::advance(endOfLastMatch, lengthOfMatch);
		};

		std::regex_iterator<BidirIt> begin(first, last, re), end;
		std::for_each(begin, end, callback);

		s.append(endOfLastMatch, last);

		return s;
	}

	template<class Traits, class CharT, class UnaryFunction>
	std::string regex_replace(const std::string& s, const std::basic_regex<CharT, Traits>& re, UnaryFunction f)
	{
		return regex_replace(s.cbegin(), s.cend(), re, f);
	}
}

namespace octoon
{
	class MDLContext
	{
	public:
		mi::base::Handle<mi::neuraylib::INeuray> neuray;
		mi::base::Handle<mi::neuraylib::IDatabase> database;
		mi::base::Handle<mi::neuraylib::IScope> globalScope;
		mi::base::Handle<mi::neuraylib::IMdl_compiler> compiler;
		mi::base::Handle<mi::neuraylib::IImage_api> imageAPI;
		mi::base::Handle<mi::neuraylib::IMdl_factory> factory;
		mi::base::Handle<mi::neuraylib::IMdl_execution_context> executionContext;
		mi::base::Handle<mi::neuraylib::IMdl_backend> backend;

		static inline mi::neuraylib::INeuray* load_and_get_ineuray(const char* filename = 0)
		{
			static void* g_dso_handle = 0;

			if (!filename)
				filename = "libmdl_sdk" MI_BASE_DLL_FILE_EXT;

#ifdef MI_PLATFORM_WINDOWS
			void* handle = LoadLibraryA((LPSTR)filename);
			if (!handle) {
				LPTSTR buffer = 0;
				LPCTSTR message = TEXT("unknown failure");
				DWORD error_code = GetLastError();
				if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS, 0, error_code,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buffer, 0, 0))
					message = buffer;
				fprintf(stderr, "Failed to load library (%u): " FMT_LPTSTR, error_code, message);
				if (buffer)
					LocalFree(buffer);
				return 0;
			}
			void* symbol = GetProcAddress((HMODULE)handle, "mi_factory");
			if (!symbol) {
				LPTSTR buffer = 0;
				LPCTSTR message = TEXT("unknown failure");
				DWORD error_code = GetLastError();
				if (FormatMessage(FORMAT_MESSAGE_ALLOCATE_BUFFER | FORMAT_MESSAGE_FROM_SYSTEM |
					FORMAT_MESSAGE_IGNORE_INSERTS, 0, error_code,
					MAKELANGID(LANG_NEUTRAL, SUBLANG_DEFAULT), (LPTSTR)&buffer, 0, 0))
					message = buffer;
				fprintf(stderr, "GetProcAddress error (%u): " FMT_LPTSTR, error_code, message);
				if (buffer)
					LocalFree(buffer);
				return 0;
			}
#else // MI_PLATFORM_WINDOWS
			void* handle = dlopen(filename, RTLD_LAZY);
			if (!handle) {
				fprintf(stderr, "%s\n", dlerror());
				return 0;
			}
			void* symbol = dlsym(handle, "mi_factory");
			if (!symbol) {
				fprintf(stderr, "%s\n", dlerror());
				return 0;
			}
#endif // MI_PLATFORM_WINDOWS
			g_dso_handle = handle;

			mi::neuraylib::INeuray* neuray = mi::neuraylib::mi_factory<mi::neuraylib::INeuray>(symbol);
			if (!neuray)
			{
				mi::base::Handle<mi::neuraylib::IVersion> version(
					mi::neuraylib::mi_factory<mi::neuraylib::IVersion>(symbol));
				if (!version)
					fprintf(stderr, "Error: Incompatible library.\n");
				else
					fprintf(stderr, "Error: Library version %s does not match header version %s.\n",
						version->get_product_version(), MI_NEURAYLIB_PRODUCT_VERSION_STRING);
				return 0;
			}
			return neuray;
		}
	};

	class DefaultLogger : public mi::base::Interface_implement<mi::base::ILogger>
	{
	public:
		void message(mi::base::Message_severity level, const char* /*module_category*/, const mi::base::Message_details& /*details*/, const char* message)
		{
			const char* severity = 0;
			switch (level) {
			case mi::base::MESSAGE_SEVERITY_FATAL:        severity = "fatal: "; break;
			case mi::base::MESSAGE_SEVERITY_ERROR:        severity = "error: "; break;
			case mi::base::MESSAGE_SEVERITY_WARNING:      severity = "warn:  "; break;
			case mi::base::MESSAGE_SEVERITY_INFO:         severity = "info:  "; break;
			case mi::base::MESSAGE_SEVERITY_VERBOSE:      return;
			case mi::base::MESSAGE_SEVERITY_DEBUG:        return;
			case mi::base::MESSAGE_SEVERITY_FORCE_32_BIT: return;
			}

			fprintf(stderr, "%s", severity);
			fprintf(stderr, "%s", message);
			putc('\n', stderr);

#ifdef MI_PLATFORM_WINDOWS
			fflush(stderr);
#endif
		}
	};

	void check_success(bool expr, const char* errMsg)
	{
		if (expr) return;

		throw std::runtime_error(errMsg);
	}

	void check_success(bool expr, const std::string& errMsg)
	{
		if (expr) return;

		throw std::runtime_error(errMsg);
	}

	void check_success(mi::Sint32 errVal, const char* errMsg)
	{
		if (errVal >= 0) return;

		throw std::runtime_error(std::string(errMsg) + " (" + std::to_string(errVal) + ")");
	}

	void check_success(mi::Sint32 errVal, const std::string& errMsg)
	{
		if (errVal >= 0) return;

		throw std::runtime_error(errMsg + " (" + std::to_string(errVal) + ")");
	}

	MDLLoader::MDLLoader() noexcept(false)
		: context_(std::make_unique<MDLContext>())
		, verboseLogging_(false)
	{
#ifdef MI_PLATFORM_WINDOWS
		const std::string lib_mdl_sdk = "libmdl_sdk.dll";
		const std::string lib_nv_freeimage = "nv_freeimage.dll";
		const std::string lib_dds = "dds.dll";
#else
		const std::string lib_mdl_sdk = "libmdl_sdk.so";
		const std::string lib_nv_freeimage = "nv_freeimage.so";
		const std::string lib_dds = "dds.so";
#endif

		this->context_->neuray = MDLContext::load_and_get_ineuray();
		if (!this->context_->neuray.is_valid_interface())
			throw std::runtime_error("Starting MDL neuray failed");

		mi::base::Handle<mi::base::ILogger> logger(new DefaultLogger());

		this->context_->compiler = this->context_->neuray->get_api_component<mi::neuraylib::IMdl_compiler>();
		this->context_->compiler->set_logger(logger.get());

		this->addModulePath(".");

		if (this->context_->compiler->load_plugin_library(lib_nv_freeimage.c_str()) < 0)
			throw std::runtime_error("Failed to load plugin " + lib_nv_freeimage);

		if (this->context_->compiler->load_plugin_library(lib_dds.c_str()) < 0)
			throw std::runtime_error("Failed to load plugin " + lib_dds);

		if (this->context_->neuray->start() < 0)
			throw std::runtime_error("Starting MDL SDK failed");

		this->context_->database = this->context_->neuray->get_api_component<mi::neuraylib::IDatabase>();
		this->context_->globalScope = this->context_->database->get_global_scope();
		this->context_->factory = this->context_->neuray->get_api_component<mi::neuraylib::IMdl_factory>();

		this->context_->executionContext = this->context_->factory->create_execution_context();
		this->context_->executionContext->set_option("internal_space", "coordinate_world");
		this->context_->executionContext->set_option("bundle_resources", false);
		this->context_->executionContext->set_option("mdl_meters_per_scene_unit", 1.0f);
		this->context_->executionContext->set_option("mdl_wavelength_min", 380.0f);
		this->context_->executionContext->set_option("mdl_wavelength_max", 780.0f);
		this->context_->executionContext->set_option("include_geometry_normal", true);

		this->context_->backend = this->context_->compiler->get_backend(mi::neuraylib::IMdl_compiler::MB_CUDA_PTX);

		if (this->context_->backend->set_option("num_texture_spaces", "1") < 0)
			throw std::runtime_error("Setting PTX option num_texture_spaces failed");

		if (this->context_->backend->set_option("num_texture_results", std::to_string(MDL_MAX_TEXTURES).c_str()) < 0)
			throw std::runtime_error("Setting PTX option num_texture_results failed");

		if (this->context_->backend->set_option("sm_version", "30") < 0)
			throw std::runtime_error("Setting PTX option sm_version failed");

		if (this->context_->backend->set_option("tex_lookup_call_mode", "optix_cp") < 0)
			throw std::runtime_error("Setting PTX option tex_lookup_call_mode failed");

		this->context_->imageAPI = this->context_->neuray->get_api_component<mi::neuraylib::IImage_api>();

		mi::base::Handle<mi::neuraylib::ITransaction> transaction = mi::base::make_handle(this->context_->globalScope->create_transaction());
		{
			mi::base::Handle<mi::neuraylib::IImage> image(transaction->create<mi::neuraylib::IImage>("Image"));
			transaction->store(image.get(), "dummyImage");
		}

		transaction->commit();
	}

	MDLLoader::~MDLLoader() noexcept
	{
		this->context_->imageAPI.reset();
		this->context_->backend.reset();
		this->context_->executionContext.reset();
		this->context_->factory.reset();
		this->context_->globalScope.reset();
		this->context_->database.reset();
		this->context_->compiler.reset();
		this->context_->neuray->shutdown();
	}

	bool
	MDLLoader::doCanRead(io::istream& stream) noexcept
	{
		return false;
	}

	bool
	MDLLoader::doCanRead(const char* type) noexcept
	{
		return false;
	}

	std::unique_ptr<material::Material>
	MDLLoader::load(io::istream& stream, std::string_view material) noexcept(false)
	{
		auto streamsize = stream.size();
		if (streamsize > 0)
		{
			const std::string mdlSource(streamsize, '0');
			stream.read((char*)mdlSource.data(), mdlSource.size());

			const std::size_t p = material.rfind("::");

			std::string_view moduleName = material.substr(0, p);
			std::string_view materialName = (p == std::string::npos) ? material : material.substr(p + 2, material.size() - p);

			return this->load(materialName, moduleName, mdlSource);
		}
		else
		{
			return nullptr;
		}

	}

	std::unique_ptr<material::Material>
	MDLLoader::load(std::string_view name, std::string_view moduleName, std::string_view source) noexcept(false)
	{
		auto material = std::make_unique<material::Material>(name);
		auto mdlModuleName = moduleName.find("::") == 0 ? std::string(moduleName) : "::" + std::string(moduleName);
		auto mdlMaterialName = "mdl" + mdlModuleName + "::" + material->getName();

		auto transaction = mi::base::make_handle(this->context_->globalScope->create_transaction());
		{
			auto materialDefinition = mi::base::make_handle(transaction->access<mi::neuraylib::IMaterial_definition>(mdlMaterialName.c_str()));
			if (!materialDefinition)
			{
#if _VISUAL_STUDIO_2017_
				if (this->verboseLogging_)
					std::cout << "[MDL] Loading material definition for \"" << material << "\" from module \"" << std::string(moduleName) << "\"" << std::endl;
#else
				if (this->verboseLogging_)
					std::cout << "[MDL] Loading material definition for \"" << material << "\" from module \"" << moduleName << "\"" << std::endl;
#endif

				if (source.empty())
				{
					if (this->context_->compiler->load_module(transaction.get(), mdlModuleName.c_str()) < 0)
						throw std::runtime_error("Loading \"" + std::string(moduleName) + ".mdl\" failed");
				}
				else
				{
					if (this->context_->compiler->load_module_from_string(transaction.get(), mdlModuleName.c_str(), std::string(source).c_str()) < 0)
						throw std::runtime_error("Loading \"" + std::string(moduleName) + "\" from memory failed");
				}

				materialDefinition = mi::base::make_handle(transaction->access<mi::neuraylib::IMaterial_definition>(mdlMaterialName.c_str()));
			}
			else
			{
				if (this->verboseLogging_)
					std::cout << "[MDL] Loaded material definition for \"" << material << "\" from database" << std::endl;
			}

			check_success(materialDefinition, "Material " + mdlMaterialName + " not found");

			mi::Sint32 errors;
			mi::base::Handle<const mi::neuraylib::IExpression_list> arguments;
			mi::base::Handle<mi::neuraylib::IMaterial_instance> materialInstance = mi::base::make_handle(materialDefinition->create_material_instance(arguments.get(), &errors));
			check_success(errors, "Creating material instance failed");

			transaction->store(materialInstance.get(), material->getName().c_str());
		}

		transaction->commit();

		transaction = mi::base::make_handle(this->context_->globalScope->create_transaction());
		{
			mi::neuraylib::Argument_editor editor(transaction.get(), material->getName().c_str(), this->context_->factory.get());

			if (this->verboseLogging_)
				std::cout << "[MDL]  - Parameters: " << editor.get_parameter_count() << std::endl;

			for (mi::Size i = 0; i < editor.get_parameter_count(); ++i)
			{
				const char* name = editor.get_parameter_name(i);
				auto types = editor.get_parameter_types();
				auto type = types->get_type(name);
				auto resolvedType = type->skip_all_type_aliases();
				auto kind = resolvedType->get_kind();

				switch (kind)
				{
				case mi::neuraylib::IType::Kind::TK_BOOL:
				{
					bool value;
					editor.get_value(name, value);
					material->set(name, value);
				}
				break;
				case mi::neuraylib::IType::Kind::TK_FLOAT:
				{
					float value;
					editor.get_value(name, value);
					material->set(name, value);
				}
				break;
				case mi::neuraylib::IType::Kind::TK_DOUBLE:
				{
					double value;
					editor.get_value(name, value);
					material->set(name, (float)value);
				}
				break;
				case mi::neuraylib::IType::Kind::TK_INT:
				{
					int value;
					editor.get_value(name, value);
					material->set(name, value);
				}
				break;
				case mi::neuraylib::IType::Kind::TK_TEXTURE:
				{
					int value;
					editor.get_value(name, value);
					material->set(name, value);
				}
				break;
				case mi::neuraylib::IType::Kind::TK_COLOR:
				{
					mi::math::Color value;
					editor.get_value(name, value);
					material->set(name, math::float4(value.r, value.g, value.b, value.a));
				}
				break;
				default:
					if (this->verboseLogging_)
						std::cout << "[MDL] Warning: Unhandled parameter type: " << (int)kind << std::endl;
					break;
				}

				if (this->verboseLogging_)
				{
					auto TypeToString = [](mi::neuraylib::IType::Kind kind) -> std::string
					{
						switch (kind)
						{
						case mi::neuraylib::IType::Kind::TK_BOOL:
							return "bool";
						case mi::neuraylib::IType::Kind::TK_COLOR:
							return "color";
						case mi::neuraylib::IType::Kind::TK_DOUBLE:
							return "double";
						case mi::neuraylib::IType::Kind::TK_FLOAT:
							return "float";
						case mi::neuraylib::IType::Kind::TK_INT:
							return "int";
						case mi::neuraylib::IType::Kind::TK_TEXTURE:
							return "texture";
						default:
							return "other";
						}
					};

					std::cout << "[MDL]      " << name << " (type: " << TypeToString(kind) << ")" << std::endl;
				}

				resolvedType->release();
				type->release();
				types->release();
			}
		}

		transaction->commit();

		return std::move(material);
	}

	void
	MDLLoader::save(io::ostream& stream, const material::Material& animation) noexcept(false)
	{
	}

	void
	MDLLoader::addModulePath(std::string_view modulePath)
	{
		if (modulePath.empty())
			return;

		std::string path(modulePath);

		if (this->modulePaths_.find(path) == this->modulePaths_.end())
		{
			this->context_->compiler->add_module_path(path.c_str());
			this->modulePaths_.insert(path);
		}
	}

	std::string
	MDLLoader::preprocessSource(std::string_view source)
	{
		std::set<std::string> textureVariables;
		std::regex textureDeclareRegex("uniform.*?texture_2d.*?((?:[a-zA-Z][a-zA-Z0-9_]*)).*?=.*?texture_2d.*?\\(\\).*?(\\[\\[anno::unused\\(\\)\\]\\])?", std::regex::icase);

		std::string phase1 = std::regex_replace(std::string(source), textureDeclareRegex, [&](const std::smatch& match)
		{
			if (match.size() >= 2)
			{
				const std::string var = match[1];
				textureVariables.insert(var);

				bool unused = false;
				if (match.size() == 3)
				{
					const std::string anno = match[2];
					if (anno.find("anno::unused") != std::string::npos)
						unused = true;
				}

				if (unused)
					return "uniform bool " + MDL_USE_TEXTURE_PREFIX + var + " = false [[anno::unused()]], " + match.str();
				else
					return "uniform bool " + MDL_USE_TEXTURE_PREFIX + var + " = false, " + match.str();
			}

			return match.str();
		});

		std::regex textureIsValidRegex("tex::texture_isvalid.*?\\(.*?((?:[a-zA-Z][a-zA-Z0-9_]*)).*?\\)", std::regex::icase);

		std::string phase2 = std::regex_replace(phase1, textureIsValidRegex, [&](const std::smatch& match)
		{
			if (match.size() == 2)
			{
				const std::string var = match[1];

				if (textureVariables.find(var) != textureVariables.end())
					return MDL_USE_TEXTURE_PREFIX + var;
			}

			return match.str();
		});

		return phase2;
	}
}