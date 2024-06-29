#ifndef UNREAL_BEHAVIOUR_H_
#define UNREAL_BEHAVIOUR_H_

#include "unreal_context.h"
#include "unreal_profile.h"

#include "controllers/client_component.h"
#include "controllers/entities_component.h"
#include "controllers/frame_sequence_component.h"
#include "controllers/gizmo_component.h"
#include "controllers/grid_component.h"
#include "controllers/h264_component.h"
#include "controllers/h265_component.h"
#include "controllers/light_component.h"
#include "controllers/environment_component.h"
#include "controllers/main_light_component.h"
#include "controllers/camera_component.h"
#include "controllers/mark_component.h"
#include "controllers/sound_component.h"
#include "controllers/offline_component.h"
#include "controllers/player_component.h"
#include "controllers/record_component.h"
#include "controllers/selector_component.h"
#include "controllers/material_component.h"

#include <octoon/octoon.h>

namespace unreal
{
	class UnrealBehaviour final : public octoon::GameComponent
	{
		OctoonDeclareSubClass(UnrealBehaviour, octoon::GameComponent)
	public : UnrealBehaviour() noexcept;
		UnrealBehaviour(const std::shared_ptr<UnrealProfile>& profile) noexcept;
		~UnrealBehaviour() noexcept;

		const std::shared_ptr<UnrealProfile>& getProfile() const noexcept;

		bool open(const std::filesystem::path& path) noexcept(false);
		void save(const std::filesystem::path& path) noexcept(false);
		void reset() noexcept;
		bool isOpen() const noexcept;

		void load(const std::filesystem::path& path) noexcept(false);

		void play() noexcept;
		void pause() noexcept;

		bool startRecord(const std::filesystem::path& path) noexcept;
		void stopRecord() noexcept;

		void renderPicture(const std::filesystem::path& path) noexcept(false);

		std::optional<octoon::RaycastHit> raycastHit(const octoon::math::float2& pos) noexcept;

		void addComponent(IUnrealComponent* component) noexcept;
		void removeComponent(const IUnrealComponent* component) noexcept;
		const std::vector<IUnrealComponent*>& getComponents() const noexcept;
		void initializeComponents() noexcept(false);
		void disableComponents() noexcept;

		IUnrealComponent* getComponent(const std::type_info& type) const noexcept;
		template<typename T, typename = std::enable_if_t<std::is_base_of<IUnrealComponent, T>::value>>
		T* getComponent() const noexcept { return dynamic_cast<T*>(this->getComponent(typeid(T))); }

		virtual octoon::GameComponentPtr clone() const noexcept override;

	  private:
		void onActivate() noexcept(false) override;
		void onDeactivate() noexcept override;

		void onFixedUpdate() noexcept override;
		void onUpdate() noexcept override;
		void onLateUpdate() noexcept override;

		void onMouseDown(const std::any& event) noexcept;
		void onMouseMotion(const std::any& event) noexcept;
		void onMouseUp(const std::any& event) noexcept;

		void onResize(const std::any& data) noexcept;

	  private:
		std::shared_ptr<UnrealProfile> profile_;
		std::shared_ptr<UnrealContext> context_;

		std::unique_ptr<RecordComponent> recordComponent_;
		std::unique_ptr<EntitiesComponent> entitiesComponent_;
		std::unique_ptr<OfflineComponent> offlineComponent_;
		std::unique_ptr<MainLightComponent> mainLightComponent_;
		std::unique_ptr<EnvironmentComponent> environmentComponent_;
		std::unique_ptr<CameraComponent> cameraComponent_;
		std::unique_ptr<SoundComponent> soundComponent_;
		std::unique_ptr<PlayerComponent> playerComponent_;
		std::unique_ptr<H264Component> h264Component_;
		std::unique_ptr<H265Component> h265Component_;
		std::unique_ptr<FrameSequenceComponent> frameSequenceComponent_;
		std::unique_ptr<MarkComponent> markComponent_;
		std::unique_ptr<MaterialComponent> materialComponent_;
		std::unique_ptr<SelectorComponent> selectorComponent_;
		std::unique_ptr<GridComponent> gridComponent_;
		std::unique_ptr<GizmoComponent> gizmoComponent_;
		std::unique_ptr<LightComponent> lightComponent_;

		std::vector<IUnrealComponent*> components_;
	};
}

#endif