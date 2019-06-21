#include "file_controller.h"

#include <octoon/octoon.h>
#include <octoon/io/fstream.h>

#include "..//models/pmm.h"
#include "../libs/nativefiledialog/nfd.h"

constexpr std::size_t PATHLIMIT = 4096;

std::vector<const char*> g_SupportedProject = { "pmm" };
std::vector<const char*> g_SupportedModel = { "pmx" };

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(FileController, GameComponent, "FileController")

		std::string utf82acp(const std::string& sjis)
		{
			std::string utf8_string;

			LPCCH pSJIS = (LPCCH)sjis.c_str();
			int utf16size = ::MultiByteToWideChar(CP_UTF8, MB_ERR_INVALID_CHARS, pSJIS, -1, 0, 0);
			if (utf16size != 0)
			{
				auto pUTF16 = std::make_unique<WCHAR[]>(utf16size);
				if (::MultiByteToWideChar(CP_UTF8, 0, (LPCCH)pSJIS, -1, pUTF16.get(), utf16size) != 0)
				{
					int utf8size = ::WideCharToMultiByte(CP_ACP, 0, pUTF16.get(), -1, 0, 0, 0, 0);
					if (utf8size != 0)
					{
						auto pUTF8 = std::make_unique<TCHAR[]>(utf8size + 16);
						std::memset(pUTF8.get(), 0, utf8size + 16);
						if (::WideCharToMultiByte(CP_ACP, 0, pUTF16.get(), -1, pUTF8.get(), utf8size, 0, 0) != 0)
							utf8_string = std::string(pUTF8.get());
					}
				}
			}

			return utf8_string;
		}

		FileController::FileController() noexcept
		{
		}

		FileController::~FileController() noexcept
		{
		}

		bool 
		FileController::showFileOpenBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept
		{
			assert(buffer && max_length > 0 && ext_name);

			nfdchar_t* outpath = nullptr;

			try
			{
				nfdresult_t  result = NFD_OpenDialog(ext_name, nullptr, &outpath);
				if (result != NFD_OKAY)
					return false;

				if (outpath)
				{
					std::strncpy(buffer, outpath, max_length);
					free(outpath);

					return true;
				}

				return false;
			}
			catch (...)
			{
				if (outpath) free(outpath);
				return false;
			}
		}

		bool 
		FileController::showFileSaveBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept
		{
			assert(buffer && max_length > 0 && ext_name);

			nfdchar_t* outpath = nullptr;

			try
			{
				nfdresult_t  result = NFD_SaveDialog(ext_name, nullptr, &outpath);
				if (result != NFD_OKAY)
					return false;

				if (outpath)
				{
					std::strncpy(buffer, outpath, max_length);
					free(outpath);

					return true;
				}

				return false;
			}
			catch (...)
			{
				if (outpath) free(outpath);

				return false;
			}
		}

		void
		FileController::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
			this->addMessageListener("editor:menu:file:open", std::bind(&FileController::openProject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:file:save", std::bind(&FileController::saveProject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:file:saveAs", std::bind(&FileController::saveAsProject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:file:import", std::bind(&FileController::openModel, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:file:export", std::bind(&FileController::saveModel, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:file:exit", std::bind(&FileController::exit, this, std::placeholders::_1));
		}

		void
		FileController::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
			this->removeMessageListener("editor:menu:file:open", std::bind(&FileController::openProject, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:file:save", std::bind(&FileController::saveProject, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:file:saveAs", std::bind(&FileController::saveAsProject, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:file:import", std::bind(&FileController::openModel, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:file:export", std::bind(&FileController::saveModel, this, std::placeholders::_1));
		}

		void
		FileController::openProject(const runtime::any& data) noexcept
		{			
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileOpenBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
				return;

			auto pmm = PMMFile::load(io::ifstream(utf82acp(filepath))).value();
			for (auto& it : pmm.model)
			{
				auto model = GamePrefabs::instance()->createModel(it.path);
				if (model)
				{
					model->setName(it.name);

					objects_.push_back(model);
				}
			}
		}

		void
		FileController::saveProject(const runtime::any& data) noexcept
		{
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
				return;
		}

		void
		FileController::saveAsProject(const runtime::any& data) noexcept
		{
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
				return;
		}

		void
		FileController::openModel(const runtime::any& data) noexcept
		{
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileOpenBrowse(filepath, PATHLIMIT, g_SupportedModel[0]))
				return;

			objects_.push_back(GamePrefabs::instance()->createModel(filepath));
		}

		void
		FileController::saveModel(const runtime::any& data) noexcept
		{
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedModel[0]))
				return;
		}

		void 
		FileController::exit(const runtime::any& data) noexcept
		{
			std::exit(0);
		}

		GameComponentPtr 
		FileController::clone() const noexcept
		{
			return std::make_shared<FileController>();
		}
	}
}