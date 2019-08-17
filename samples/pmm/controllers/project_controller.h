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

			bool open(const std::string& path) noexcept;
			void close() noexcept;

			bool importModel(const std::string& path) noexcept;
			bool exportModel(const std::string& path) noexcept;

			virtual GameComponentPtr clone() const noexcept override;

		private:
			void onFileDrop(const runtime::any& data) noexcept;

			void onOpenProject(const runtime::any& data) noexcept;
			void onSaveProject(const runtime::any& data) noexcept;
			void onSaveAsProject(const runtime::any& data) noexcept;

			void onOpenModel(const runtime::any& data) noexcept;
			void onSaveModel(const runtime::any& data) noexcept;

			void exit(const runtime::any& data) noexcept;

			void play(const runtime::any& data) noexcept;
			void offlineMode(const runtime::any& data) noexcept;

			void onRenderVideo(const runtime::any& data) noexcept;
			void renderPicture(const runtime::any& data) noexcept;

			GameObjectPtr createCamera(const PMMFile& camera) noexcept;

			void setupBoneAnimation(const PmmModel& model, animation::AnimationClips<float>& clips) noexcept;
			void setupMorphAnimation(const PmmModel& model, animation::AnimationClip<float>& clips) noexcept;
			void setupCameraAnimation(const std::vector<PmmKeyframeCamera>& camera, animation::AnimationClip<float>& clip) noexcept;

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