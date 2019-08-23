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

	bool
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
				AnimationClips<float> boneClips;
				this->setupBoneAnimation(it, boneClips);

				AnimationClip<float> morphClip;
				this->setupMorphAnimation(it, morphClip);

				auto model = GamePrefabs::instance()->createModel(it.path);
				if (model)
				{
					model->setName(it.name);
					model->addComponent<AnimatorComponent>(model->getComponent<SkinnedMeshRendererComponent>()->getTransforms());
					model->getComponent<AnimatorComponent>()->setAnimation(animation::Animation(boneClips));
					model->getComponent<AnimatorComponent>()->setTime(0.0f);

					model->addComponent<AnimationComponent>(animation::Animation(morphClip));
					model->getComponent<AnimationComponent>()->setTime(0.0f);

					objects.emplace_back(std::move(model));
				}
			}

			auto mainLight = octoon::GameObject::create("DirectionalLight");
			mainLight->addComponent<octoon::OfflineDirectionalLightComponent>();
			mainLight->getComponent<octoon::OfflineDirectionalLightComponent>()->setIntensity(4.0f);
			mainLight->getComponent<octoon::OfflineDirectionalLightComponent>()->setColor(pmm.main_light.rgb);
			mainLight->getComponent<octoon::TransformComponent>()->setQuaternion(math::normalize(math::Quaternion(math::float3::Forward, math::normalize(-pmm.main_light.xyz))));
			objects.push_back(mainLight);

			auto enviromentLight = octoon::GameObject::create("EnvironmentLight");
			enviromentLight->addComponent<octoon::OfflineEnvironmentLightComponent>();
			enviromentLight->getComponent<octoon::OfflineEnvironmentLightComponent>()->setIntensity(2.0f);
			this->getContext()->profile->entitiesModule->enviromentLight = enviromentLight;

			objects.push_back(enviromentLight);

			this->getContext()->profile->entitiesModule->objects = objects;
			return true;
		}
		catch (const std::bad_optional_access&)
		{
			this->getContext()->behaviour->sendMessage("editor:message:error", "Failed to open the file");
			return false;
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
		camera->setClearColor(octoon::math::float4::One);

		obj->getComponent<TransformComponent>()->setQuaternion(math::Quaternion(-pmm.camera.rotation));
		obj->getComponent<TransformComponent>()->setTranslate(pmm.camera.eye);
		obj->getComponent<TransformComponent>()->setTranslateAccum(math::rotate(math::Quaternion(pmm.camera.rotation), math::float3::Forward) * math::distance(pmm.camera.eye, pmm.camera.target));
		obj->addComponent<AnimationComponent>(animation::Animation(clip))->setTime(0.0f);
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

		for (auto& it : camera_keyframes)
		{
			auto interpolationDistance = std::make_shared<PathInterpolator<float>>(it.interpolation_distance[0] / 255.0f, it.interpolation_distance[1] / 255.0f, it.interpolation_distance[2] / 255.0f, it.interpolation_distance[3] / 255.0f);
			auto interpolationX = std::make_shared<PathInterpolator<float>>(it.interpolation_x[0] / 255.0f, it.interpolation_x[1] / 255.0f, it.interpolation_x[2] / 255.0f, it.interpolation_x[3] / 255.0f);
			auto interpolationY = std::make_shared<PathInterpolator<float>>(it.interpolation_y[0] / 255.0f, it.interpolation_y[1] / 255.0f, it.interpolation_y[2] / 255.0f, it.interpolation_y[3] / 255.0f);
			auto interpolationZ = std::make_shared<PathInterpolator<float>>(it.interpolation_z[0] / 255.0f, it.interpolation_z[1] / 255.0f, it.interpolation_z[2] / 255.0f, it.interpolation_z[3] / 255.0f);
			auto interpolationRotation = std::make_shared<PathInterpolator<float>>(it.interpolation_rotation[0] / 255.0f, it.interpolation_rotation[1] / 255.0f, it.interpolation_rotation[2] / 255.0f, it.interpolation_rotation[3] / 255.0f);
			auto interpolationAngleView = std::make_shared<PathInterpolator<float>>(it.interpolation_angleview[0] / 255.0f, it.interpolation_angleview[1] / 255.0f, it.interpolation_angleview[2] / 255.0f, it.interpolation_angleview[3] / 255.0f);

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
		std::vector<Keyframes<float>> rotationW(it.bone_init_frame.size());

		for (std::size_t i = 0; i < it.bone_init_frame.size(); i++)
		{
			auto& key = it.bone_init_frame[i];

			auto interpolationX = std::make_shared<PathInterpolator<float>>(key.interpolation_x[0] / 255.0f, key.interpolation_x[1] / 255.0f, key.interpolation_x[2] / 255.0f, key.interpolation_x[3] / 255.0f);
			auto interpolationY = std::make_shared<PathInterpolator<float>>(key.interpolation_y[0] / 255.0f, key.interpolation_y[1] / 255.0f, key.interpolation_y[2] / 255.0f, key.interpolation_y[3] / 255.0f);
			auto interpolationZ = std::make_shared<PathInterpolator<float>>(key.interpolation_z[0] / 255.0f, key.interpolation_z[1] / 255.0f, key.interpolation_z[2] / 255.0f, key.interpolation_z[3] / 255.0f);
			auto interpolationRotation = std::make_shared<PathInterpolator<float>>(key.interpolation_rotation[0] / 255.0f, key.interpolation_rotation[1] / 255.0f, key.interpolation_rotation[2] / 255.0f, key.interpolation_rotation[3] / 255.0f);

			auto euler = math::eulerAngles(key.quaternion);

			translateX[i].emplace_back((float)key.frame / 30.0f, key.translation.x, interpolationX);
			translateY[i].emplace_back((float)key.frame / 30.0f, key.translation.y, interpolationY);
			translateZ[i].emplace_back((float)key.frame / 30.0f, key.translation.z, interpolationZ);
			rotationX[i].emplace_back((float)key.frame / 30.0f, euler.x, interpolationRotation);
			rotationY[i].emplace_back((float)key.frame / 30.0f, euler.y, interpolationRotation);
			rotationZ[i].emplace_back((float)key.frame / 30.0f, euler.z, interpolationRotation);
		}

		for (auto& key : it.bone_key_frame)
		{
			auto index = key.pre_index;
			while (index >= it.bone_name.size())
				index = it.bone_key_frame[index - it.bone_name.size()].pre_index;

			auto frameB = key;
			auto frameA = PmmKeyframeBone();
			if (key.pre_index < it.bone_name.size())
				frameA = it.bone_init_frame[key.pre_index];
			else
				frameA = it.bone_key_frame[key.pre_index - it.bone_name.size()];

			auto interpolationX = std::make_shared<PathInterpolator<float>>(key.interpolation_x[0] / 255.0f, key.interpolation_x[1] / 255.0f, key.interpolation_x[2] / 255.0f, key.interpolation_x[3] / 255.0f);
			auto interpolationY = std::make_shared<PathInterpolator<float>>(key.interpolation_y[0] / 255.0f, key.interpolation_y[1] / 255.0f, key.interpolation_y[2] / 255.0f, key.interpolation_y[3] / 255.0f);
			auto interpolationZ = std::make_shared<PathInterpolator<float>>(key.interpolation_z[0] / 255.0f, key.interpolation_z[1] / 255.0f, key.interpolation_z[2] / 255.0f, key.interpolation_z[3] / 255.0f);
			auto interpolationRotation = std::make_shared<PathInterpolator<float>>(frameA.interpolation_rotation[0] / 255.0f, frameA.interpolation_rotation[1] / 255.0f, frameA.interpolation_rotation[2] / 255.0f, frameA.interpolation_rotation[3] / 255.0f);

			for (std::size_t i = 1; i <= (frameB.frame - frameA.frame) * 5; i++)
			{
				auto t = i / ((frameB.frame - frameA.frame) * 5.0f);
				auto euler = math::eulerAngles(math::slerp(frameA.quaternion, frameB.quaternion, interpolationRotation->interpolator(t)));
				auto frame = frameA.frame + (frameB.frame - frameA.frame) / ((frameB.frame - frameA.frame) * 5.0f) * i;

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

		for (std::size_t i = 0; i < it.morph_init_frame.size(); i++)
		{
			auto& key = it.morph_init_frame[i];
			keyframes[i].emplace_back((float)key.frame / 30.0f, key.value);
		}

		for (auto& key : it.morph_key_frame)
		{
			auto index = key.pre_index;
			while (index >= it.morph_name.size())
				index = it.morph_key_frame[index - it.morph_name.size()].pre_index;

			keyframes[index].emplace_back((float)key.frame / 30.0f, key.value);
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
		}
	}
}