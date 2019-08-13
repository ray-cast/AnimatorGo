#ifndef OCTOON_EDITOR_PROJECT_CONTROLLER_H_
#define OCTOON_EDITOR_PROJECT_CONTROLLER_H_

#include <octoon/game_component.h>
#include <octoon/animation/animation_clip.h>
#include "..//models/pmm.h"

namespace octoon
{
	namespace editor
	{
		class ProjectController final : public GameComponent
		{
			OctoonDeclareSubClass(ProjectController, GameComponent)
		public:
			ProjectController() noexcept;
			~ProjectController() noexcept;

			virtual GameComponentPtr clone() const noexcept override;

		private:
			void openProject(const runtime::any& data) noexcept;
			void saveProject(const runtime::any& data) noexcept;
			void saveAsProject(const runtime::any& data) noexcept;

			void openModel(const runtime::any& data) noexcept;
			void saveModel(const runtime::any& data) noexcept;

			void exit(const runtime::any& data) noexcept;

			void play(const runtime::any& data) noexcept;
			void offlineMode(const runtime::any& data) noexcept;

			void renderVideo(const runtime::any& data) noexcept;
			void renderPicture(const runtime::any& data) noexcept;

			GameObjectPtr createCamera(const PMMFile& camera) noexcept;

			void setupCameraAnimation(const std::vector<PmmKeyframeCamera>& camera, animation::AnimationClip<float>& clip) noexcept;
			void setupModelAnimation(const PmmModel& model, animation::AnimationClips<float>& clips) noexcept;

			void setupRenderScene() noexcept;
			void setupOfflineRenderScene() noexcept;

		private:
			bool showFileOpenBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept;
			bool showFileSaveBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept;

		private:
			void onActivate() noexcept override;
			void onDeactivate() noexcept override;

		private:
			GameObjects objects_;
			GameObjectPtr camera_;
		};
	}
}

#endif