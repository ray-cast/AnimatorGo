#ifndef MYSTIC_FILE_COMPONENT_H_
#define MYSTIC_FILE_COMPONENT_H_

#include "../utils/pmm.h"
#include "../mysticLit_component.h"
#include "../module/file_module.h"

#include <octoon/octoon.h>

namespace MysticLit
{
	class FileComponent final : public MysticLitComponent<FileModule>
	{
	public:
		FileComponent() noexcept;
		virtual ~FileComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		bool open(const std::string& path) noexcept;

		bool importModel(const std::string& path) noexcept;
		bool exportModel(const std::string& path) noexcept;

		bool showFileOpenBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept;
		bool showFileSaveBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept;

	private:
		octoon::GameObjectPtr createCamera(const octoon::PMMFile& pmm) noexcept;
		void setupBoneAnimation(const octoon::PmmModel& model, octoon::animation::AnimationClips<float>& clips) noexcept;
		void setupMorphAnimation(const octoon::PmmModel& model, octoon::animation::AnimationClip<float>& clips) noexcept;
		void setupCameraAnimation(const std::vector<octoon::PmmKeyframeCamera>& camera, octoon::animation::AnimationClip<float>& clip) noexcept;

	private:
		void onFileDrop(const octoon::runtime::any& data) noexcept;

		void onEnable() noexcept override;
		void onDisable() noexcept override;
	};
}

#endif