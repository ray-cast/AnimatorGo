#include <octoon/mdl_loader.h>
#include <mi/mdl_sdk.h>

#ifdef MI_PLATFORM_WINDOWS
#ifdef UNICODE
#define FMT_LPTSTR "%ls"
#else // UNICODE
#define FMT_LPTSTR "%s"
#endif // UNICODE
#endif // MI_PLATFORM_WINDOWS

// Pointer to the DSO handle. Cached here for unload().
static void* g_dso_handle = 0;

inline mi::neuraylib::INeuray* load_and_get_ineuray(const char* filename = 0)
{
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

class Default_logger : public mi::base::Interface_implement<mi::base::ILogger>
{
public:
    void message(
        mi::base::Message_severity level,
        const char* /*module_category*/,
        const mi::base::Message_details& /*details*/,
        const char* message)
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

namespace octoon
{
    class MDLContext
    {
    public:
        mi::base::Handle<mi::neuraylib::INeuray> m_neuray;
        mi::base::Handle<mi::neuraylib::IDatabase> m_database;
        mi::base::Handle<mi::neuraylib::IMdl_compiler> m_mdl_compiler;
        mi::base::Handle<mi::neuraylib::IImage_api> m_image_api;
        mi::base::Handle<mi::neuraylib::IMdl_factory> m_mdl_factory;
        mi::base::Handle<mi::neuraylib::IMdl_backend> m_hlsl_backend;
    };

	MDLLoader::MDLLoader() noexcept
	{
        auto context = std::make_unique<MDLContext>();

        context->m_neuray = load_and_get_ineuray();
        if (!context->m_neuray.is_valid_interface())
            return;

        mi::base::Handle<mi::base::ILogger> logger(new Default_logger());

        context->m_mdl_compiler = mi::base::Handle<mi::neuraylib::IMdl_compiler>(context->m_neuray->get_api_component<mi::neuraylib::IMdl_compiler>());
        context->m_mdl_compiler->set_logger(logger.get());
	}

	MDLLoader::~MDLLoader() noexcept
	{
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
	MDLLoader::load(io::istream& stream) noexcept(false)
	{
		return nullptr;
	}

	void
	MDLLoader::save(io::ostream& stream, const material::Material& animation) noexcept(false)
	{
	}
}