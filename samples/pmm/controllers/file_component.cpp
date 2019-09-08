#include "file_component.h"
#include "../libs/nativefiledialog/nfd.h"
#include "../mysticLit_profile.h"
#include "../mysticlit_behaviour.h"
#include <octoon/octoon.h>
#include <octoon/io/fstream.h>
#include <octoon/animation/animation_curve.h>
#include <octoon/animation/animation_clip.h>
#include <octoon/animation/path_interpolator.h>
#include <octoon/offline_feature.h>
#include <octoon/offline_camera_component.h>
#include <octoon/offline_mesh_renderer_component.h>
#include <octoon/offline_environment_light_component.h>
#include <octoon/offline_directional_light_component.h>
#include <octoon/offline_skinned_mesh_renderer_component.h>
#include <octoon/timer_feature.h>
#include <octoon/cloth_component.h>
#include <octoon/game_base_features.h>
#include <octoon/physics_feature.h>
#include <octoon/game_object_manager.h>
#include <octoon/runtime/string.h>

#include <fstream>
#include <unordered_map>

using namespace octoon;
using namespace octoon::math;
using namespace octoon::animation;

namespace MysticLit
{
	FileComponent::FileComponent() noexcept
	{
	}

	FileComponent::~FileComponent() noexcept
	{
	}

	void
	FileComponent::setActive(bool active) noexcept
	{
	}

	bool
	FileComponent::getActive() const noexcept
	{
		return true;
	}

	bool 
	FileComponent::showFileOpenBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept
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
	FileComponent::showFileSaveBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept
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
				if (std::strstr(outpath, ".") == 0)
				{
					std::strcat(buffer, ".");
					std::strcat(buffer, ext_name);
				}

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
	FileComponent::importModel(const std::string& path) noexcept
	{
		auto mdoel = octoon::GamePrefabs::instance()->createModel(path);
		if (mdoel)
		{
			this->getContext()->profile->entitiesModule->objects.push_back(mdoel);
			return true;
		}

		return false;
	}

	bool
	FileComponent::exportModel(const std::string& path) noexcept
	{
		return false;
	}

	void
	FileComponent::importHDRi(const std::string& filepath) noexcept
	{
		if (this->getContext()->profile->entitiesModule->enviromentLight)
		{
			auto environmentLight = this->getContext()->profile->entitiesModule->enviromentLight->getComponent<octoon::OfflineEnvironmentLightComponent>();
			if (environmentLight)
				environmentLight->setBgImage(filepath);
		}
	}

	void
	FileComponent::clearHDRi() noexcept
	{
		if (this->getContext()->profile->entitiesModule->enviromentLight)
		{
			auto environmentLight = this->getContext()->profile->entitiesModule->enviromentLight->getComponent<octoon::OfflineEnvironmentLightComponent>();
			if (environmentLight)
				environmentLight->setBgImage("");
		}
	}

	std::optional<std::string>
	FileComponent::open(const std::string& filepath) noexcept
	{
		try
		{
			octoon::GameObjects objects;

			auto stream = octoon::io::ifstream(filepath);
			auto pmm = octoon::PMMFile::load(stream).value();

			auto camera = this->createCamera(pmm);
			if (camera)
			{
				objects.emplace_back(camera);
				this->getContext()->profile->entitiesModule->camera = camera;
			}

			for (auto& it : pmm.model)
			{
				auto model = GamePrefabs::instance()->createModel(it.path);
				if (model)
				{
					AnimationClips<float> boneClips;
					this->setupBoneAnimation(it, boneClips);

					AnimationClip<float> morphClip;
					this->setupMorphAnimation(it, morphClip);

					model->setName(it.name);
					model->addComponent<AnimatorComponent>(animation::Animation(std::move(boneClips)), model->getComponent<OfflineSkinnedMeshRendererComponent>()->getTransforms())->sample();
					model->addComponent<AnimatorComponent>(animation::Animation(std::move(morphClip)))->sample();

					objects.emplace_back(std::move(model));
				}
				else
				{
					return std::string(u8"无法找到文件:" + it.path);
				}
			}

			auto mainLight = octoon::GameObject::create("DirectionalLight");
			mainLight->addComponent<octoon::OfflineDirectionalLightComponent>();
			mainLight->getComponent<octoon::OfflineDirectionalLightComponent>()->setIntensity(2.0f);
			mainLight->getComponent<octoon::OfflineDirectionalLightComponent>()->setColor(pmm.main_light.rgb);
			mainLight->getComponent<octoon::TransformComponent>()->setQuaternion(math::normalize(math::Quaternion(math::float3::Forward, math::normalize(-pmm.main_light.xyz))));
			objects.push_back(mainLight);

			auto enviromentLight = octoon::GameObject::create("EnvironmentLight");
			enviromentLight->addComponent<octoon::OfflineEnvironmentLightComponent>();
			enviromentLight->addComponent<octoon::OfflineEnvironmentLightComponent>()->setColor(octoon::math::srgb2linear(octoon::math::float3(0.90196078f, 0.90196078f, 0.925490196f)));
			enviromentLight->getComponent<octoon::OfflineEnvironmentLightComponent>()->setIntensity(1.0f);
			this->getContext()->profile->entitiesModule->enviromentLight = enviromentLight;

			objects.push_back(enviromentLight);

			this->getContext()->profile->entitiesModule->objects = objects;
			return std::nullopt;
		}
		catch (const std::bad_optional_access&)
		{
			this->getContext()->behaviour->sendMessage("editor:message:error", "Failed to open the file");
			return std::nullopt;
		}
	}

	GameObjectPtr
	FileComponent::createCamera(const PMMFile& pmm) noexcept
	{
		AnimationClip<float> clip;
		this->setupCameraAnimation(pmm.camera_keyframes, clip);

		auto obj = GameObject::create("MainCamera");

		auto offlineCamera = obj->addComponent<OfflineCameraComponent>();
		offlineCamera->setActive(false);
		offlineCamera->setAperture((float)pmm.camera_keyframes[0].fov);

		auto camera = obj->addComponent<PerspectiveCameraComponent>();
		camera->setAperture((float)pmm.camera_keyframes[0].fov);
		camera->setCameraType(video::CameraType::Main);
		camera->setClearFlags(octoon::hal::GraphicsClearFlagBits::AllBit);
		camera->setClearColor(octoon::math::float4(0.1f, 0.1f, 0.1f, 1.0f));

		obj->getComponent<TransformComponent>()->setQuaternion(math::Quaternion(-pmm.camera.rotation));
		obj->getComponent<TransformComponent>()->setTranslate(pmm.camera.eye);
		obj->getComponent<TransformComponent>()->setTranslateAccum(math::rotate(math::Quaternion(pmm.camera.rotation), -math::float3::Forward) * math::distance(pmm.camera.eye, pmm.camera.target));
		obj->addComponent<AnimatorComponent>(animation::Animation(clip))->sample();
		obj->addComponent<EditorCameraComponent>();

		auto active = this->getContext()->profile->offlineModule->offlineEnable;
		obj->getComponent<octoon::OfflineCameraComponent>()->setActive(active);
		obj->getComponent<octoon::PerspectiveCameraComponent>()->setActive(!active);

		this->getContext()->behaviour->sendMessage("editor:camera:set", obj);

		return obj;
	}

	void
	FileComponent::setupCameraAnimation(const std::vector<PmmKeyframeCamera>& camera_keyframes, AnimationClip<float>& clip) noexcept
	{
		Keyframes<float> distance;
		Keyframes<float> eyeX;
		Keyframes<float> eyeY;
		Keyframes<float> eyeZ;
		Keyframes<float> rotationX;
		Keyframes<float> rotationY;
		Keyframes<float> rotationZ;
		Keyframes<float> fov;

		distance.reserve(camera_keyframes.size());
		eyeX.reserve(camera_keyframes.size());
		eyeY.reserve(camera_keyframes.size());
		eyeZ.reserve(camera_keyframes.size());
		rotationX.reserve(camera_keyframes.size());
		rotationY.reserve(camera_keyframes.size());
		rotationZ.reserve(camera_keyframes.size());
		fov.reserve(camera_keyframes.size());

		for (auto& it : camera_keyframes)
		{
			auto interpolationDistance = std::make_shared<PathInterpolator<float>>(it.interpolation_distance[0] / 127.0f, it.interpolation_distance[2] / 127.0f, it.interpolation_distance[1] / 127.0f, it.interpolation_distance[3] / 127.0f);
			auto interpolationX = std::make_shared<PathInterpolator<float>>(it.interpolation_x[0] / 127.0f, it.interpolation_x[2] / 127.0f, it.interpolation_x[2] / 127.0f, it.interpolation_x[1] / 127.0f);
			auto interpolationY = std::make_shared<PathInterpolator<float>>(it.interpolation_y[0] / 127.0f, it.interpolation_y[2] / 127.0f, it.interpolation_y[2] / 127.0f, it.interpolation_y[1] / 127.0f);
			auto interpolationZ = std::make_shared<PathInterpolator<float>>(it.interpolation_z[0] / 127.0f, it.interpolation_z[2] / 127.0f, it.interpolation_z[2] / 127.0f, it.interpolation_z[1] / 127.0f);
			auto interpolationRotation = std::make_shared<PathInterpolator<float>>(it.interpolation_rotation[0] / 127.0f, it.interpolation_rotation[2] / 127.0f, it.interpolation_rotation[1] / 127.0f, it.interpolation_rotation[3] / 127.0f);
			auto interpolationAngleView = std::make_shared<PathInterpolator<float>>(it.interpolation_angleview[0] / 127.0f, it.interpolation_angleview[2] / 127.0f, it.interpolation_angleview[1] / 127.0f, it.interpolation_angleview[3] / 127.0f);

			distance.emplace_back((float)it.frame / 30.0f, it.distance, interpolationDistance);
			eyeX.emplace_back((float)it.frame / 30.0f, it.eye.x, interpolationX);
			eyeY.emplace_back((float)it.frame / 30.0f, it.eye.y, interpolationY);
			eyeZ.emplace_back((float)it.frame / 30.0f, it.eye.z, interpolationZ);
			rotationX.emplace_back((float)it.frame / 30.0f, it.rotation.x, interpolationRotation);
			rotationY.emplace_back((float)it.frame / 30.0f, it.rotation.y, interpolationRotation);
			rotationZ.emplace_back((float)it.frame / 30.0f, it.rotation.z, interpolationRotation);
			fov.emplace_back((float)it.frame / 30.0f, (float)it.fov, interpolationAngleView);
		}

		clip.setCurve("LocalPosition.x", AnimationCurve(std::move(eyeX)));
		clip.setCurve("LocalPosition.y", AnimationCurve(std::move(eyeY)));
		clip.setCurve("LocalPosition.z", AnimationCurve(std::move(eyeZ)));
		clip.setCurve("LocalEulerAnglesRaw.x", AnimationCurve(std::move(rotationX)));
		clip.setCurve("LocalEulerAnglesRaw.y", AnimationCurve(std::move(rotationY)));
		clip.setCurve("LocalEulerAnglesRaw.z", AnimationCurve(std::move(rotationZ)));
		clip.setCurve("Transform:move", AnimationCurve(std::move(distance)));
		clip.setCurve("Camera:fov", AnimationCurve(std::move(fov)));
	}

	void
	FileComponent::setupBoneAnimation(const PmmModel& it, animation::AnimationClips<float>& clips) noexcept
	{
		std::vector<Keyframes<float>> translateX(it.bone_init_frame.size());
		std::vector<Keyframes<float>> translateY(it.bone_init_frame.size());
		std::vector<Keyframes<float>> translateZ(it.bone_init_frame.size());
		std::vector<Keyframes<float>> rotationX(it.bone_init_frame.size());
		std::vector<Keyframes<float>> rotationY(it.bone_init_frame.size());
		std::vector<Keyframes<float>> rotationZ(it.bone_init_frame.size());

		std::vector<uint32_t> key_to_animation_count(it.bone_init_frame.size());
		std::vector<uint32_t> key_to_array_index(it.bone_key_frame.size() << 1);
		std::vector<uint32_t> key_to_data_index(it.bone_key_frame.size());

		std::size_t numBone = it.bone_name.size();

		for (int i = 0; i < it.bone_key_frame.size(); i++)
		{
			auto index = it.bone_key_frame[i].data_index;
			if (index >= key_to_array_index.size())
				key_to_array_index.resize(index + 1);
			key_to_array_index[index] = i;
		}

		for (int i = 0; i < it.bone_key_frame.size(); i++)
		{
			auto index = it.bone_key_frame[i].pre_index;
			while (index >= numBone)
				index = it.bone_key_frame[key_to_array_index[index]].pre_index;
			key_to_data_index[i] = index;
			key_to_animation_count[index]++;
		}

		for (std::size_t i = 0; i < key_to_animation_count.size(); i++)
		{
			auto count = key_to_animation_count[i] + 1;
			translateX[i].reserve(count);
			translateY[i].reserve(count);
			translateZ[i].reserve(count);
			rotationX[i].reserve(count * 10);
			rotationY[i].reserve(count * 10);
			rotationZ[i].reserve(count * 10);
		}

		for (std::size_t i = 0; i < it.bone_init_frame.size(); i++)
		{
			auto& key = it.bone_init_frame[i];

			auto interpolationX = std::make_shared<PathInterpolator<float>>(key.interpolation_x[0] / 127.0f, key.interpolation_x[2] / 127.0f, key.interpolation_x[1] / 127.0f, key.interpolation_x[3] / 127.0f);
			auto interpolationY = std::make_shared<PathInterpolator<float>>(key.interpolation_y[0] / 127.0f, key.interpolation_y[2] / 127.0f, key.interpolation_y[1] / 127.0f, key.interpolation_y[3] / 127.0f);
			auto interpolationZ = std::make_shared<PathInterpolator<float>>(key.interpolation_z[0] / 127.0f, key.interpolation_z[2] / 127.0f, key.interpolation_z[1] / 127.0f, key.interpolation_z[3] / 127.0f);
			auto interpolationRotation = std::make_shared<PathInterpolator<float>>(key.interpolation_rotation[0] / 127.0f, key.interpolation_rotation[2] / 127.0f, key.interpolation_rotation[1] / 127.0f, key.interpolation_rotation[3] / 127.0f);

			auto euler = math::eulerAngles(key.quaternion);

			translateX[i].emplace_back((float)key.frame / 30.0f, key.translation.x, interpolationX);
			translateY[i].emplace_back((float)key.frame / 30.0f, key.translation.y, interpolationY);
			translateZ[i].emplace_back((float)key.frame / 30.0f, key.translation.z, interpolationZ);
			rotationX[i].emplace_back((float)key.frame / 30.0f, euler.x, interpolationRotation);
			rotationY[i].emplace_back((float)key.frame / 30.0f, euler.y, interpolationRotation);
			rotationZ[i].emplace_back((float)key.frame / 30.0f, euler.z, interpolationRotation);
		}

		for (int i = 0; i < it.bone_key_frame.size(); i++)
		{
			auto& key = it.bone_key_frame[i];
			auto& keyLast = key.pre_index < numBone ? it.bone_init_frame[key.pre_index] : it.bone_key_frame[key_to_array_index[key.pre_index]];

			auto interpolationX = std::make_shared<PathInterpolator<float>>(key.interpolation_x[0] / 127.0f, key.interpolation_x[2] / 127.0f, key.interpolation_x[1] / 127.0f, key.interpolation_x[3] / 127.0f);
			auto interpolationY = std::make_shared<PathInterpolator<float>>(key.interpolation_y[0] / 127.0f, key.interpolation_y[2] / 127.0f, key.interpolation_y[1] / 127.0f, key.interpolation_y[3] / 127.0f);
			auto interpolationZ = std::make_shared<PathInterpolator<float>>(key.interpolation_z[0] / 127.0f, key.interpolation_z[2] / 127.0f, key.interpolation_z[1] / 127.0f, key.interpolation_z[3] / 127.0f);
			auto interpolationRotation = std::make_shared<PathInterpolator<float>>(keyLast.interpolation_rotation[0] / 127.0f, keyLast.interpolation_rotation[2] / 127.0f, keyLast.interpolation_rotation[1] / 127.0f, keyLast.interpolation_rotation[3] / 127.0f);

			auto index = key_to_data_index[i];

			for (std::size_t i = 1; i <= (key.frame - keyLast.frame) * 10; i++)
			{
				auto t = i / ((key.frame - keyLast.frame) * 10.0f);
				auto euler = math::eulerAngles(math::slerp(keyLast.quaternion, key.quaternion, interpolationRotation->interpolator(t)));
				auto frame = keyLast.frame + (key.frame - keyLast.frame) / ((key.frame - keyLast.frame) * 10.0f) * i;

				rotationX[index].emplace_back((float)frame / 30.0f, euler.x);
				rotationY[index].emplace_back((float)frame / 30.0f, euler.y);
				rotationZ[index].emplace_back((float)frame / 30.0f, euler.z);
			}

			translateX[index].emplace_back((float)key.frame / 30.0f, key.translation.x, interpolationX);
			translateY[index].emplace_back((float)key.frame / 30.0f, key.translation.y, interpolationY);
			translateZ[index].emplace_back((float)key.frame / 30.0f, key.translation.z, interpolationZ);
		}

		clips.resize(it.bone_init_frame.size());

		for (std::size_t i = 0; i < clips.size(); i++)
		{
			auto& clip = clips[i];
			clip.setName(it.bone_name[i]);
			clip.setCurve("LocalPosition.x", AnimationCurve(std::move(translateX[i])));
			clip.setCurve("LocalPosition.y", AnimationCurve(std::move(translateY[i])));
			clip.setCurve("LocalPosition.z", AnimationCurve(std::move(translateZ[i])));
			clip.setCurve("LocalEulerAnglesRaw.x", AnimationCurve(std::move(rotationX[i])));
			clip.setCurve("LocalEulerAnglesRaw.y", AnimationCurve(std::move(rotationY[i])));
			clip.setCurve("LocalEulerAnglesRaw.z", AnimationCurve(std::move(rotationZ[i])));
		}
	}

	void
	FileComponent::setupMorphAnimation(const PmmModel& it, animation::AnimationClip<float>& clip) noexcept
	{
		std::vector<Keyframes<float>> keyframes(it.morph_name.size());

		std::vector<uint32_t> key_to_animation_count(it.morph_init_frame.size());
		std::vector<uint32_t> key_to_array_index(it.morph_key_frame.size() << 1);
		std::vector<uint32_t> key_to_data_index(it.morph_key_frame.size());

		std::size_t numMorph = it.morph_name.size();

		for (int i = 0; i < it.morph_key_frame.size(); i++)
		{
			auto index = it.morph_key_frame[i].data_index;
			if (index >= key_to_array_index.size())
				key_to_array_index.resize(index + 1);
			key_to_array_index[index] = i;
		}

		for (int i = 0; i < it.morph_key_frame.size(); i++)
		{
			auto index = it.morph_key_frame[i].pre_index;
			while (index >= numMorph)
				index = it.morph_key_frame[key_to_array_index[index]].pre_index;
			key_to_data_index[i] = index;
			key_to_animation_count[index]++;
		}

		for (std::size_t i = 0; i < it.morph_name.size(); i++)
		{
			keyframes[i].reserve(key_to_animation_count[i] + 1);
		}

		for (std::size_t i = 0; i < it.morph_init_frame.size(); i++)
		{
			auto& key = it.morph_init_frame[i];
			keyframes[i].emplace_back((float)key.frame / 30.0f, key.value);
		}

		for (int i = 0; i < it.morph_key_frame.size(); i++)
		{
			auto& key = it.morph_key_frame[i];
			keyframes[key_to_data_index[i]].emplace_back((float)key.frame / 30.0f, key.value);
		}

		for (std::size_t i = 0; i < it.morph_name.size(); i++)
		{
			clip.setCurve(it.morph_name[i], AnimationCurve(std::move(keyframes[i])));
		}
	}

	void
	FileComponent::onEnable() noexcept
	{
		auto baseFeature = this->getContext()->behaviour->getFeature<octoon::GameBaseFeature>();
		if (baseFeature)
		{
			auto gameObjectManager = baseFeature->getGameObjectManager();
			if (gameObjectManager)
				gameObjectManager->addMessageListener("feature:input:drop", std::bind(&FileComponent::onFileDrop, this, std::placeholders::_1));
		}
	}

	void
	FileComponent::onDisable() noexcept
	{
		auto baseFeature = this->getContext()->behaviour->getFeature<octoon::GameBaseFeature>();
		if (baseFeature)
		{
			auto gameObjectManager = baseFeature->getGameObjectManager();
			if (gameObjectManager)
				gameObjectManager->removeMessageListener("feature:input:drop", std::bind(&FileComponent::onFileDrop, this, std::placeholders::_1));
		}
	}

	void
	FileComponent::onFileDrop(const octoon::runtime::any& data) noexcept
	{
		if (data.type() == typeid(std::vector<const char*>))
		{
			auto files = octoon::runtime::any_cast<std::vector<const char*>>(data);
			if (files.empty())
				return;

			std::string_view str(files.front());
			auto ext = str.substr(str.find_first_of("."));
			if (ext == ".pmm")
				this->open(std::string(str));
			else if (ext == ".pmx")
				this->importModel(std::string(str));
			else if (ext == ".hdr")
				this->importHDRi(std::string(str));
		}
	}
}