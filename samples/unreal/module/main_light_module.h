#ifndef UNREAL_MAIN_LIGHT_MODULE_H_
#define UNREAL_MAIN_LIGHT_MODULE_H_

#include <unreal_model.h>
#include <octoon/game_object.h>
#include <octoon/math/vector3.h>

namespace unreal
{
	class MainLightModule final : public UnrealModule
	{
	public:
		MainLightModule() noexcept;
		virtual ~MainLightModule() noexcept;

		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& writer) noexcept override;

		virtual void disconnect() noexcept;

	private:
		MainLightModule(const MainLightModule&) = delete;
		MainLightModule& operator=(const MainLightModule&) = delete;

	public:
		MutableLiveData<float> intensity;
		MutableLiveData<float> size;
		MutableLiveData<octoon::math::float3> color;
		MutableLiveData<octoon::math::float3> rotation;
		MutableLiveData<octoon::GameObjectPtr> mainLight;
	};
}

#endif