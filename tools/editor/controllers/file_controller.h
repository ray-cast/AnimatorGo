#ifndef OCTOON_EDITOR_FILE_CONTROLLER_H_
#define OCTOON_EDITOR_FILE_CONTROLLER_H_

#include <octoon/game_component.h>

namespace octoon
{
	namespace editor
	{
		class FileController : public GameComponent
		{
			OctoonDeclareSubClass(FileController, GameComponent)
		public:
			FileController() noexcept;
			~FileController() noexcept;

			virtual void openProject(const std::any& data) noexcept;
			virtual void saveProject(const std::any& data) noexcept;
			virtual void saveAsProject(const std::any& data) noexcept;

			virtual void openModel(const std::any& data) noexcept;
			virtual void saveModel(const std::any& data) noexcept;

			virtual void exit(const std::any& data) noexcept;

			virtual GameComponentPtr clone() const noexcept override;

		private:
			bool showFileOpenBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept;
			bool showFileSaveBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

		private:
			GameObjects objects_;
		};
	}
}

#endif