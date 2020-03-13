#include "file_controller.h"

#include <octoon/octoon.h>
#include "../libs/nativefiledialog/nfd.h"

std::vector<const char*> g_SupportedProject = { "pmm" };
std::vector<const char*> g_SupportedModel = { "pmx" };

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(FileController, GameComponent, "FileController")

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
			constexpr std::size_t PATHLIMIT = 4096;
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileOpenBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
				return;
		}

		void
		FileController::saveProject(const runtime::any& data) noexcept
		{
			constexpr std::size_t PATHLIMIT = 4096;
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
				return;
		}

		void
		FileController::saveAsProject(const runtime::any& data) noexcept
		{
			constexpr std::size_t PATHLIMIT = 4096;
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
				return;
		}

		void
		FileController::openModel(const runtime::any& data) noexcept
		{
			constexpr std::size_t PATHLIMIT = 4096;
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileOpenBrowse(filepath, PATHLIMIT, g_SupportedModel[0]))
				return;

			objects_.push_back(MeshLoader::load(filepath));
		}

		void
		FileController::saveModel(const runtime::any& data) noexcept
		{
			constexpr std::size_t PATHLIMIT = 4096;
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