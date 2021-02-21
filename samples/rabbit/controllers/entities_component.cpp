#include "entities_component.h"
#include "../rabbit_profile.h"
#include "../rabbit_behaviour.h"
#include <octoon/ass_loader.h>
#pragma warning(push)
#pragma warning(disable:4245)
#include "../libs/nativefiledialog/nfd.h"
#pragma warning(pop)

#include <fstream>
#include <unordered_map>
#include <omp.h>

using namespace octoon;
using namespace octoon::math;

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
	EntitiesComponent::importAudio(std::string_view path) noexcept
	{
		auto model = octoon::GameObject::create();
		model->setName(path);
		model->addComponent<octoon::AudioSourceComponent>()->setAudioReader(AudioLoader::load(path));

		this->getContext()->profile->entitiesModule->sound = model;
		return true;
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

	void
	EntitiesComponent::importAss(std::string_view path) noexcept(false)
	{
		auto& context = this->getContext();
		if (!context->profile->entitiesModule->objects.empty())
		{
			context->profile->entitiesModule->objects.clear();
		}

		octoon::GameObjects objects = octoon::ASSLoader::load(path);

		for (auto& it : objects)
		{
			if (it->getComponent<CameraComponent>())
				context->profile->entitiesModule->camera = it;
			else
			{
				auto renderer = it->getComponent<MeshRendererComponent>();
				if (renderer)
					renderer->setGlobalIllumination(true);
				context->profile->entitiesModule->objects.push_back(it);
			}
		}

		this->sendMessage("rabbit:project:open");
	}

	void
	EntitiesComponent::importPMM(std::string_view path) noexcept(false)
	{
		auto& context = this->getContext();
		if (!context->profile->entitiesModule->objects.empty())
		{
			context->profile->entitiesModule->objects.clear();
			context->profile->entitiesModule->camera.reset();
		}

		octoon::GameObjects objects;
		octoon::GameObjects rigidbodies;

		auto stream = octoon::io::ifstream(std::string(path));
		auto pmm = octoon::PMMFile::load(stream).value();

		auto rotation = math::Quaternion(math::float3(-0.1, math::PI + 0.5f, 0.0f));

		auto& mainLight = this->getContext()->profile->entitiesModule->sunLight;
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setIntensity(this->getContext()->profile->sunModule->intensity);
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setColor(this->getContext()->profile->sunModule->color);
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setShadowMapSize(math::uint2(2048, 2048));
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setShadowEnable(true);
		mainLight->getComponent<octoon::TransformComponent>()->setQuaternion(rotation);
		mainLight->getComponent<octoon::TransformComponent>()->setTranslate(-math::rotate(rotation, math::float3::UnitZ) * 60);

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
				model->addComponent<AnimatorComponent>(Animation(std::move(boneClips)), model->getComponent<SkinnedMeshRendererComponent>()->getTransforms());
				model->addComponent<AnimatorComponent>(Animation(std::move(morphClip)));

				auto smr = model->getComponent<octoon::SkinnedMeshRendererComponent>();
				if (smr)
					smr->setAutomaticUpdate(!this->getContext()->profile->offlineModule->offlineEnable);

				objects.emplace_back(std::move(model));
			}
			else
			{
				throw std::runtime_error(u8"无法找到文件:" + it.path);
			}
		}

		context->profile->sunModule->rotation = octoon::math::degrees(octoon::math::eulerAngles(rotation));
		context->profile->entitiesModule->camera = this->createCamera(pmm);
		context->profile->entitiesModule->objects = objects;

		this->sendMessage("rabbit:project:open");
	}

	bool
	EntitiesComponent::importModel(std::string_view path) noexcept
	{
		auto model = octoon::MeshLoader::load(path);
		if (model)
		{
			auto smr = model->getComponent<octoon::SkinnedMeshRendererComponent>();
			if (smr)
				smr->setAutomaticUpdate(!this->getContext()->profile->offlineModule->offlineEnable);

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
	EntitiesComponent::clearAudio() noexcept
	{
		this->getContext()->profile->entitiesModule->sound = nullptr;
	}

	void 
	EntitiesComponent::importHDRi(const octoon::hal::GraphicsTexturePtr& texture) noexcept
	{
		auto& environmentLight = this->getContext()->profile->entitiesModule->enviromentLight;
		if (environmentLight)
		{
			auto envLight = environmentLight->getComponent<octoon::EnvironmentLightComponent>();
			if (envLight)
				envLight->setEnvironmentMap(PMREMLoader::load(texture));

			auto material = environmentLight->getComponent<octoon::MeshRendererComponent>()->getMaterial()->downcast<octoon::MeshBasicMaterial>();
			material->setColorMap(texture);
		}
	}

	void
	EntitiesComponent::importHDRi(std::string_view filepath) noexcept
	{
		auto texture = TextureLoader::load(filepath);
		if (texture)
			this->importHDRi(texture);
	}

	void
	EntitiesComponent::clearHDRi() noexcept
	{
		auto& environmentLight = this->getContext()->profile->entitiesModule->enviromentLight;
		if (environmentLight)
		{
			auto envLight = environmentLight->getComponent<octoon::EnvironmentLightComponent>();
			if (envLight)
				envLight->setEnvironmentMap(nullptr);

			auto material = environmentLight->getComponent<octoon::MeshRendererComponent>()->getMaterial()->downcast<octoon::MeshBasicMaterial>();
			material->setColorMap(nullptr);
		}
	}

	GameObjectPtr
	EntitiesComponent::createCamera(const PMMFile& pmm) noexcept
	{
		AnimationClip<float> clip;
		this->setupCameraAnimation(pmm.camera_keyframes, clip);

		auto eye = octoon::math::float3(pmm.camera.eye.x, pmm.camera.eye.y, pmm.camera.eye.z);
		auto target = octoon::math::float3(pmm.camera.target.x, pmm.camera.target.y, pmm.camera.target.z);
		auto quat = math::Quaternion(-math::float3(pmm.camera.rotation.x, pmm.camera.rotation.y, pmm.camera.rotation.z));

		auto mainCamera = GameObject::create("MainCamera");
		mainCamera->addComponent<FirstPersonCameraComponent>();
		mainCamera->addComponent<AudioListenerComponent>();
		mainCamera->addComponent<AnimatorComponent>(Animation(clip));

		auto camera = mainCamera->addComponent<FilmCameraComponent>();
		camera->setFov((float)pmm.camera_keyframes[0].fov);
		camera->setCameraType(CameraType::Main);
		camera->setClearFlags(hal::GraphicsClearFlagBits::AllBit);
		camera->setClearColor(math::float4(0.0f, 0.0f, 0.0f, 1.0f));

		auto transform = mainCamera->getComponent<TransformComponent>();
		transform->setQuaternion(quat);
		transform->setTranslate(eye);
		transform->setTranslateAccum(math::rotate(quat, math::float3::Forward) * math::distance(eye, target));

		this->getContext()->behaviour->sendMessage("editor:camera:set", mainCamera);

		return mainCamera;
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
	EntitiesComponent::setupBoneAnimation(const PmmModel& it, AnimationClips<float>& clips) noexcept
	{
		std::size_t numBone = it.bone_init_frame.size();

		clips.resize(numBone);

#		pragma omp parallel for num_threads(omp_get_num_procs() / 2)
		for (std::int32_t i = 0; i < numBone; i++)
		{
			auto keyframeCount = 1u;
			auto next_index = it.bone_init_frame[i].next_index;
			auto final_frame = it.bone_init_frame[i].frame;
			while (next_index > 0)
			{
				keyframeCount++;
				next_index -= it.bone_init_frame.size();
				final_frame = it.bone_key_frame[next_index].frame;
				next_index = it.bone_key_frame[next_index].next_index;
			}

			Keyframes<float> translateX;
			Keyframes<float> translateY;
			Keyframes<float> translateZ;
			Keyframes<float> rotationX;
			Keyframes<float> rotationY;
			Keyframes<float> rotationZ;

			translateX.reserve(keyframeCount);
			translateY.reserve(keyframeCount);
			translateZ.reserve(keyframeCount);
			rotationX.reserve(final_frame * 20u + 1u);
			rotationY.reserve(final_frame * 20u + 1u);
			rotationZ.reserve(final_frame * 20u + 1u);

			auto& initKey = it.bone_init_frame[i];
			auto interpolationX = std::make_shared<PathInterpolator<float>>(initKey.interpolation_x[0] / 127.0f, initKey.interpolation_x[2] / 127.0f, initKey.interpolation_x[1] / 127.0f, initKey.interpolation_x[3] / 127.0f);
			auto interpolationY = std::make_shared<PathInterpolator<float>>(initKey.interpolation_y[0] / 127.0f, initKey.interpolation_y[2] / 127.0f, initKey.interpolation_y[1] / 127.0f, initKey.interpolation_y[3] / 127.0f);
			auto interpolationZ = std::make_shared<PathInterpolator<float>>(initKey.interpolation_z[0] / 127.0f, initKey.interpolation_z[2] / 127.0f, initKey.interpolation_z[1] / 127.0f, initKey.interpolation_z[3] / 127.0f);
			auto interpolationRotation = std::make_shared<PathInterpolator<float>>(initKey.interpolation_rotation[0] / 127.0f, initKey.interpolation_rotation[2] / 127.0f, initKey.interpolation_rotation[1] / 127.0f, initKey.interpolation_rotation[3] / 127.0f);

			auto euler = math::eulerAngles(math::Quaternion(initKey.quaternion.x, initKey.quaternion.y, initKey.quaternion.z, initKey.quaternion.w));

			translateX.emplace_back((float)initKey.frame / 30.0f, initKey.translation.x, interpolationX);
			translateY.emplace_back((float)initKey.frame / 30.0f, initKey.translation.y, interpolationY);
			translateZ.emplace_back((float)initKey.frame / 30.0f, initKey.translation.z, interpolationZ);
			rotationX.emplace_back((float)initKey.frame / 30.0f, euler.x, interpolationRotation);
			rotationY.emplace_back((float)initKey.frame / 30.0f, euler.y, interpolationRotation);
			rotationZ.emplace_back((float)initKey.frame / 30.0f, euler.z, interpolationRotation);

			auto next = it.bone_init_frame[i].next_index;
			while (next > 0)
			{
				auto& key = it.bone_key_frame[next - numBone];
				auto& keyLast = key.pre_index < numBone ? it.bone_init_frame[key.pre_index] : it.bone_key_frame[key.pre_index - numBone];

				auto interpolationX = std::make_shared<PathInterpolator<float>>(key.interpolation_x[0] / 127.0f, key.interpolation_x[2] / 127.0f, key.interpolation_x[1] / 127.0f, key.interpolation_x[3] / 127.0f);
				auto interpolationY = std::make_shared<PathInterpolator<float>>(key.interpolation_y[0] / 127.0f, key.interpolation_y[2] / 127.0f, key.interpolation_y[1] / 127.0f, key.interpolation_y[3] / 127.0f);
				auto interpolationZ = std::make_shared<PathInterpolator<float>>(key.interpolation_z[0] / 127.0f, key.interpolation_z[2] / 127.0f, key.interpolation_z[1] / 127.0f, key.interpolation_z[3] / 127.0f);
				auto interpolationRotation = std::make_shared<PathInterpolator<float>>(keyLast.interpolation_rotation[0] / 127.0f, keyLast.interpolation_rotation[2] / 127.0f, keyLast.interpolation_rotation[1] / 127.0f, keyLast.interpolation_rotation[3] / 127.0f);

				for (std::size_t j = 1; j <= (key.frame - keyLast.frame) * 20; j++)
				{
					auto qkeyLast = math::Quaternion(keyLast.quaternion.x, keyLast.quaternion.y, keyLast.quaternion.z, keyLast.quaternion.w);
					auto qkey = math::Quaternion(key.quaternion.x, key.quaternion.y, key.quaternion.z, key.quaternion.w);

					auto t = j / ((key.frame - keyLast.frame) * 20.0f);
					auto eulerDelta = math::eulerAngles(math::slerp(qkeyLast, qkey, interpolationRotation->interpolator(t)));
					auto frame = keyLast.frame + (key.frame - keyLast.frame) / ((key.frame - keyLast.frame) * 20.0f) * j;

					rotationX.emplace_back((float)frame / 30.0f, eulerDelta.x);
					rotationY.emplace_back((float)frame / 30.0f, eulerDelta.y);
					rotationZ.emplace_back((float)frame / 30.0f, eulerDelta.z);
				}

				translateX.emplace_back((float)key.frame / 30.0f, key.translation.x, interpolationX);
				translateY.emplace_back((float)key.frame / 30.0f, key.translation.y, interpolationY);
				translateZ.emplace_back((float)key.frame / 30.0f, key.translation.z, interpolationZ);

				next = key.next_index;
			}

			auto& clip = clips[i];
			clip.setName(it.bone_name[i]);
			clip.setCurve("LocalPosition.x", AnimationCurve(std::move(translateX)));
			clip.setCurve("LocalPosition.y", AnimationCurve(std::move(translateY)));
			clip.setCurve("LocalPosition.z", AnimationCurve(std::move(translateZ)));
			clip.setCurve("LocalEulerAnglesRaw.x", AnimationCurve(std::move(rotationX)));
			clip.setCurve("LocalEulerAnglesRaw.y", AnimationCurve(std::move(rotationY)));
			clip.setCurve("LocalEulerAnglesRaw.z", AnimationCurve(std::move(rotationZ)));
		}		
	}

	void
	EntitiesComponent::setupMorphAnimation(const PmmModel& it, AnimationClip<float>& clip) noexcept
	{
		for (std::size_t i = 0, keyframeCount = 1; i < it.morph_init_frame.size(); i++)
		{
			auto index = it.morph_init_frame[i].next_index;
			while (index > 0)
			{
				keyframeCount++;
				index = it.morph_key_frame[index - it.morph_init_frame.size()].next_index;
			}

			Keyframes<float> keyframes;
			keyframes.reserve(keyframeCount);
			keyframes.emplace_back((float)it.morph_init_frame[i].frame / 30.0f, it.morph_init_frame[i].value);

			auto next = it.morph_init_frame[i].next_index;
			while (next > 0)
			{
				next -= it.morph_init_frame.size();
				auto& frame = it.morph_key_frame[next];
				keyframes.emplace_back((float)frame.frame / 30.0f, frame.value);
				next = frame.next_index;
			}

			clip.setCurve(it.morph_name[i], AnimationCurve(std::move(keyframes)));
		}
	}

	void
	EntitiesComponent::onEnable() noexcept
	{
		auto mainLight = octoon::GameObject::create("DirectionalLight");
		mainLight->addComponent<octoon::DirectionalLightComponent>();
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setIntensity(this->getContext()->profile->sunModule->intensity);
		mainLight->getComponent<octoon::DirectionalLightComponent>()->setColor(this->getContext()->profile->sunModule->color);
		mainLight->getComponent<octoon::TransformComponent>()->setQuaternion(octoon::math::Quaternion(octoon::math::radians(this->getContext()->profile->sunModule->rotation)));

		auto envMaterial = octoon::MeshBasicMaterial::create(octoon::math::srgb2linear(this->getContext()->profile->environmentModule->color));
		envMaterial->setCullMode(octoon::hal::GraphicsCullMode::None);
		envMaterial->setGamma(1.0f);
		envMaterial->setDepthEnable(false);
		envMaterial->setDepthWriteEnable(false);

		auto enviromentLight = octoon::GameObject::create("EnvironmentLight");
		enviromentLight->addComponent<octoon::EnvironmentLightComponent>();
		enviromentLight->getComponent<octoon::EnvironmentLightComponent>()->setColor(octoon::math::srgb2linear(this->getContext()->profile->environmentModule->color));
		enviromentLight->getComponent<octoon::EnvironmentLightComponent>()->setIntensity(this->getContext()->profile->environmentModule->intensity);
		enviromentLight->getComponent<octoon::EnvironmentLightComponent>()->setOffset(this->getContext()->profile->environmentModule->offset);
		enviromentLight->addComponent<octoon::MeshFilterComponent>(octoon::SphereMesh(10000, 32, 24, math::PI * 0.5));
		enviromentLight->addComponent<octoon::MeshRendererComponent>(envMaterial)->setRenderOrder(-2);

		auto mainCamera = octoon::GameObject::create("MainCamera");
		mainCamera->addComponent<octoon::FirstPersonCameraComponent>();
		mainCamera->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 10, -10));

		auto camera = mainCamera->addComponent<octoon::FilmCameraComponent>();
		camera->setFov(60.0f);
		camera->setCameraType(octoon::CameraType::Main);
		camera->setClearColor(octoon::math::float4(0.0f, 0.0f, 0.0f, 1.0f));

		this->getContext()->profile->entitiesModule->camera = mainCamera;
		this->getContext()->profile->entitiesModule->sunLight = mainLight;
		this->getContext()->profile->entitiesModule->enviromentLight = enviromentLight;
		
		auto planeGeometry = octoon::CubeMesh::create(1, 1, 1);

		auto material = std::make_shared<octoon::MeshStandardMaterial>();
		material->setCullMode(octoon::hal::GraphicsCullMode::None);

		this->sendMessage("editor:camera:set", mainCamera);
	}

	void
	EntitiesComponent::onDisable() noexcept
	{
	}
}