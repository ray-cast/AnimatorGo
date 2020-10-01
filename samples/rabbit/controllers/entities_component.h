#ifndef RABBIT_ENTITIES_COMPONENT_H_
#define RABBIT_ENTITIES_COMPONENT_H_

#include "../utils/pmm.h"
#include "../rabbit_component.h"
#include "../module/file_module.h"
#include "../module/entities_module.h"

#include <optional>
#include <octoon/octoon.h>

namespace rabbit
{
	class EntitiesComponent final : public RabbitComponent<EntitiesModule>
	{
	public:
		EntitiesComponent() noexcept;
		virtual ~EntitiesComponent() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;
		
		bool importAudio(std::string_view path) noexcept;
		bool importAbc(std::string_view path) noexcept;
		void importPMM(std::string_view path) noexcept(false);
		bool importModel(std::string_view path) noexcept;
		void importHDRi(std::string_view path) noexcept;

		bool exportModel(std::string_view path) noexcept;

		void clearHDRi() noexcept;
		void clearAudio() noexcept;

		bool showFileOpenBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept;
		bool showFileSaveBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(EntitiesComponent);
		}

	private:
		octoon::GameObjectPtr createCamera(const octoon::PMMFile& pmm) noexcept;
		void setupBoneAnimation(const octoon::PmmModel& model, octoon::animation::AnimationClips<float>& clips) noexcept;
		void setupMorphAnimation(const octoon::PmmModel& model, octoon::animation::AnimationClip<float>& clips) noexcept;
		void setupCameraAnimation(const std::vector<octoon::PmmKeyframeCamera>& camera, octoon::animation::AnimationClip<float>& clip) noexcept;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;
	};
}

#endif