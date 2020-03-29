#include "entities_component.h"
#include "../rabbit_profile.h"
#include "../rabbit_behaviour.h"

#pragma warning(push)
#pragma warning(disable:4245)
#include "../libs/nativefiledialog/nfd.h"
#pragma warning(pop)

#include <fstream>
#include <unordered_map>

using namespace octoon;
using namespace octoon::math;
using namespace octoon::animation;

namespace rabbit
{
	EntitiesComponent::EntitiesComponent() noexcept
	{
	}

	EntitiesComponent::~EntitiesComponent() noexcept
	{
	}

	void
	EntitiesComponent::setActive(bool active) noexcept
	{
	}

	bool
	EntitiesComponent::getActive() const noexcept
	{
		return true;
	}

	bool 
	EntitiesComponent::showFileOpenBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept
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
	EntitiesComponent::showFileSaveBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept
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
	EntitiesComponent::importAbc(std::string_view path) noexcept
	{
		auto model = octoon::GameObject::create();
		model->addComponent<octoon::MeshAnimationComponent>(path);

		this->getContext()->profile->entitiesModule->objects.push_back(model);

		this->sendMessage("rabbit:project:open");
		return true;
	}

	bool
	EntitiesComponent::importModel(std::string_view path) noexcept
	{
		auto model = octoon::MeshLoader::load(path);
		if (model)
		{
			this->getContext()->profile->entitiesModule->objects.push_back(model);
			return true;
		}

		return false;
	}

	bool
	EntitiesComponent::exportModel(std::string_view path) noexcept
	{
		return false;
	}

	void
	EntitiesComponent::importHDRi(std::string_view filepath) noexcept
	{
		auto& environmentLight = this->getContext()->profile->entitiesModule->enviromentLight;
		if (environmentLight)
		{
			auto light = environmentLight->getComponent<octoon::OfflineEnvironmentLightComponent>();
			if (light)
				light->setBgImage(filepath);

			auto material = environmentLight->getComponent<octoon::MeshRendererComponent>()->getMaterial()->downcast<octoon::material::MeshBasicMaterial>();
			material->setColorTexture(TextureLoader::load(filepath));
		}
	}

	void
	EntitiesComponent::clearHDRi() noexcept
	{
		auto& environmentLight = this->getContext()->profile->entitiesModule->enviromentLight;
		if (environmentLight)
		{
			auto light = environmentLight->getComponent<octoon::OfflineEnvironmentLightComponent>();
			if (light)
				light->setBgImage("");

			auto material = environmentLight->getComponent<octoon::MeshRendererComponent>()->getMaterial()->downcast<octoon::material::MeshBasicMaterial>();
			material->setColorTexture(nullptr);
		}
	}

	void
	EntitiesComponent::open(std::string_view filepath) noexcept(false)
	{
		auto& context = this->getContext();
		if (!context->profile->entitiesModule->objects.empty())
		{
			context->profile->entitiesModule->objects.clear();
			context->profile->entitiesModule->camera.reset();
		}

		octoon::GameObjects objects;

		auto stream = octoon::io::ifstream(std::string(filepath));
		auto pmm = octoon::PMMFile::load(stream).value();

		auto camera = this->createCamera(pmm);
		if (camera)
			objects.emplace_back(camera);

		auto mainLight = octoon::GameObject::create("DirectionalLight");
		auto rotation = math::normalize(math::Quaternion(math::float3::Forward, math::normalize(pmm.main_light.xyz * math::float3(-1, -1, -1))));
		mainLight->addComponent<octoon::OfflineDirectionalLightComponent>();
		mainLight->getComponent<octoon::OfflineDirectionalLightComponent>()->setColor(context->profile->sunModule->color);
		mainLight->getComponent<octoon::OfflineDirectionalLightComponent>()->setIntensity(context->profile->sunModule->intensity);
		mainLight->addComponent<octoon::DirectionalLightComponent>();
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setIntensity(this->getContext()->profile->sunModule->intensity);
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setColor(this->getContext()->profile->sunModule->color);
		mainLight->getComponent<octoon::TransformComponent>()->setQuaternion(rotation);

		for (auto& it : pmm.model)
		{
			auto model = octoon::MeshLoader::load(it.path);
			if (model)
			{
				AnimationClips<float> boneClips;
				this->setupBoneAnimation(it, boneClips);

				AnimationClip<float> morphClip;
				this->setupMorphAnimation(it, morphClip);

				model->setName(it.name);
				model->addComponent<AnimatorComponent>(animation::Animation(std::move(boneClips)), model->getComponent<OfflineSkinnedMeshRendererComponent>()->getTransforms());
				model->addComponent<AnimatorComponent>(animation::Animation(std::move(morphClip)));

				objects.emplace_back(std::move(model));
			}
			else
			{
				throw std::runtime_error(u8"无法找到文件:" + it.path);
			}
		}

		context->profile->sunModule->rotation = octoon::math::degress(octoon::math::eulerAngles(rotation));
		context->profile->entitiesModule->camera = camera;
		context->profile->entitiesModule->sunLight = mainLight;
		context->profile->entitiesModule->objects = objects;

		this->sendMessage("rabbit:project:open");
	}

	GameObjectPtr
	EntitiesComponent::createCamera(const PMMFile& pmm) noexcept
	{
		AnimationClip<float> clip;
		this->setupCameraAnimation(pmm.camera_keyframes, clip);

		auto obj = GameObject::create("MainCamera");

		auto offlineCamera = obj->addComponent<OfflineCameraComponent>();
		offlineCamera->setAperture((float)pmm.camera_keyframes[0].fov);

		auto camera = obj->addComponent<PerspectiveCameraComponent>();
		camera->setFar(2000.0f);
		camera->setAperture((float)pmm.camera_keyframes[0].fov);
		camera->setCameraType(CameraType::Main);
		camera->setClearFlags(hal::GraphicsClearFlagBits::AllBit);
		camera->setClearColor(math::float4(0.1f, 0.1f, 0.1f, 1.0f));

		obj->getComponent<TransformComponent>()->setQuaternion(math::Quaternion(-pmm.camera.rotation));
		obj->getComponent<TransformComponent>()->setTranslate(pmm.camera.eye);
		obj->getComponent<TransformComponent>()->setTranslateAccum(math::rotate(math::Quaternion(pmm.camera.rotation), -math::float3::Forward) * math::distance(pmm.camera.eye, pmm.camera.target));
		obj->addComponent<AnimatorComponent>(animation::Animation(clip));
		obj->addComponent<FirstPersonCameraComponent>();

		auto active = this->getContext()->profile->offlineModule->offlineEnable;
		obj->getComponent<octoon::OfflineCameraComponent>()->setActive(active);
		obj->getComponent<octoon::PerspectiveCameraComponent>()->setActive(!active);

		this->getContext()->behaviour->sendMessage("editor:camera:set", obj);

		return obj;
	}

	void
	EntitiesComponent::setupCameraAnimation(const std::vector<PmmKeyframeCamera>& camera_keyframes, AnimationClip<float>& clip) noexcept
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
	EntitiesComponent::setupBoneAnimation(const PmmModel& it, animation::AnimationClips<float>& clips) noexcept
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
			rotationX[i].reserve(count * 20);
			rotationY[i].reserve(count * 20);
			rotationZ[i].reserve(count * 20);
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

			for (std::size_t j = 1; j <= (key.frame - keyLast.frame) * 20; j++)
			{
				auto t = j / ((key.frame - keyLast.frame) * 20.0f);
				auto euler = math::eulerAngles(math::slerp(keyLast.quaternion, key.quaternion, interpolationRotation->interpolator(t)));
				auto frame = keyLast.frame + (key.frame - keyLast.frame) / ((key.frame - keyLast.frame) * 20.0f) * j;

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
	EntitiesComponent::setupMorphAnimation(const PmmModel& it, animation::AnimationClip<float>& clip) noexcept
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
	EntitiesComponent::onEnable() noexcept
	{
		auto mainLight = octoon::GameObject::create("DirectionalLight");
		mainLight->addComponent<octoon::OfflineDirectionalLightComponent>();
		mainLight->getComponent<octoon::OfflineDirectionalLightComponent>()->setIntensity(this->getContext()->profile->sunModule->intensity);
		mainLight->getComponent<octoon::OfflineDirectionalLightComponent>()->setColor(this->getContext()->profile->sunModule->color);
		mainLight->addComponent<octoon::DirectionalLightComponent>();
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setIntensity(this->getContext()->profile->sunModule->intensity);
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setColor(this->getContext()->profile->sunModule->color);

		auto envMaterial = octoon::material::MeshBasicMaterial::create(octoon::math::srgb2linear(this->getContext()->profile->environmentModule->color));
		envMaterial->setCullMode(octoon::hal::GraphicsCullMode::None);

		auto enviromentLight = octoon::GameObject::create("EnvironmentLight");
		enviromentLight->addComponent<octoon::OfflineEnvironmentLightComponent>();
		enviromentLight->getComponent<octoon::OfflineEnvironmentLightComponent>()->setColor(octoon::math::srgb2linear(this->getContext()->profile->environmentModule->color));
		enviromentLight->getComponent<octoon::OfflineEnvironmentLightComponent>()->setIntensity(this->getContext()->profile->environmentModule->intensity);
		enviromentLight->addComponent<octoon::MeshFilterComponent>(octoon::mesh::SphereMesh(1000, 24, 32));
		enviromentLight->addComponent<octoon::MeshRendererComponent>(envMaterial);

		auto mainCamera = octoon::GameObject::create("MainCamera");
		mainCamera->addComponent<octoon::FirstPersonCameraComponent>();
		mainCamera->addComponent<OfflineCameraComponent>();
		mainCamera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 10, -10));

		auto camera = mainCamera->addComponent<octoon::PerspectiveCameraComponent>(60.0f);
		camera->setCameraType(octoon::CameraType::Main);
		camera->setClearColor(octoon::math::float4(0.1f, 0.1f, 0.1f, 1.0f));

		auto active = this->getContext()->profile->offlineModule->offlineEnable;
		mainCamera->getComponent<octoon::OfflineCameraComponent>()->setActive(active);
		mainCamera->getComponent<octoon::PerspectiveCameraComponent>()->setActive(!active);

		this->getContext()->profile->entitiesModule->camera = mainCamera;
		this->getContext()->profile->entitiesModule->sunLight = mainLight;
		this->getContext()->profile->entitiesModule->enviromentLight = enviromentLight;
		this->getContext()->profile->entitiesModule->objects.push_back(mainCamera);

		this->sendMessage("editor:camera:set", mainCamera);
	}

	void
	EntitiesComponent::onDisable() noexcept
	{
	}

	void
	EntitiesComponent::onDrop(std::string_view path) noexcept
	{
		auto ext = path.substr(path.find_last_of("."));
		if (ext == ".pmm")
			this->open(path);
		else if (ext == ".pmx")
			this->importModel(path);
		else if (ext == ".hdr")
			this->importHDRi(path);
		else if (ext == ".abc")
			this->importAbc(path);
	}
}