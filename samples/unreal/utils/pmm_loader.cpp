#include "pmm_loader.h"
#include <octoon/asset_importer.h>
#include <octoon/runtime/guid.h>

namespace unreal
{
	PMMLoader::PMMLoader() noexcept
	{
	}

	PMMLoader::~PMMLoader() noexcept
	{
	}
	
	void
	PMMLoader::load(UnrealProfile& profile, const std::filesystem::path& path) noexcept(false)
	{
		octoon::GameObjects objects;

		auto stream = octoon::io::ifstream(path);
		auto pmm = octoon::PMMFile::load(stream).value();

		for (auto& it : pmm.model)
		{
			auto object = octoon::AssetDatabase::instance()->loadAssetAtPath<octoon::GameObject>(it.path);
			if (object)
			{
				auto boneClip = std::make_shared<octoon::AnimationClip>();
				setupBoneAnimation(it, *boneClip);

				auto morphClip = std::make_shared<octoon::AnimationClip>();
				setupMorphAnimation(it, *morphClip);
			
				object->setName(octoon::make_guid());
				object->getComponent<octoon::SkinnedMeshRendererComponent>()->setAutomaticUpdate(!profile.offlineModule->getEnable());

				if (!morphClip->empty())
				{
					auto morph = std::make_shared<octoon::Animation>(std::move(morphClip), "Morph");
					object->addComponent<octoon::AnimatorComponent>(std::move(morph));
				}

				if (!boneClip->empty())
				{
					auto motion = std::make_shared<octoon::Animation>(std::move(boneClip), "Motion");
					object->getComponent<octoon::AnimatorComponent>()->setAnimation(std::move(motion));
				}
			
				objects.emplace_back(std::move(object));
			}
			else
			{
				throw std::runtime_error("Failed to find the file: " + it.path);
			}
		}

		profile.physicsModule->gravity.setValue(octoon::math::float3(pmm.gravity_current_data.direction.x, pmm.gravity_current_data.direction.y * pmm.gravity_current_data.acceleration, pmm.gravity_current_data.direction.z));
		profile.soundModule->enable = pmm.is_wave_enabled;
		profile.soundModule->filepath = pmm.wave_path;
		profile.mainLightModule->intensity = 1.0f;
		profile.mainLightModule->color = octoon::math::float3(pmm.main_light.rgb.x, pmm.main_light.rgb.y, pmm.main_light.rgb.z);
		profile.mainLightModule->rotation = octoon::math::degrees(octoon::math::eulerAngles(octoon::math::Quaternion(octoon::math::float3(-0.1, octoon::math::PI + 0.5f, 0.0f))));
		profile.entitiesModule->objects = objects;

		auto animtion = std::make_shared<octoon::Animation>();
		setupCameraAnimation(pmm.camera_keyframes, *animtion);

		auto eye = octoon::math::float3(pmm.camera.eye.x, pmm.camera.eye.y, pmm.camera.eye.z);
		auto target = octoon::math::float3(pmm.camera.target.x, pmm.camera.target.y, pmm.camera.target.z);
		auto quat = -octoon::math::float3(pmm.camera.rotation.x, pmm.camera.rotation.y, pmm.camera.rotation.z);

		profile.cameraModule->animation = std::move(animtion);
		profile.cameraModule->fov = (float)pmm.camera_keyframes[0].fov;
		profile.cameraModule->rotation = quat;
		profile.cameraModule->translate = eye + octoon::math::rotate(octoon::math::Quaternion(quat), octoon::math::float3::Forward) * octoon::math::distance(eye, target);
	}

	void
	PMMLoader::setupCameraAnimation(const std::vector<octoon::PmmKeyframeCamera>& keyframes, octoon::Animation& animation) noexcept
	{
		octoon::Keyframes<float> distance;
		octoon::Keyframes<float> eyeX;
		octoon::Keyframes<float> eyeY;
		octoon::Keyframes<float> eyeZ;
		octoon::Keyframes<float> rotation;
		octoon::Keyframes<float> rotationY;
		octoon::Keyframes<float> rotationZ;
		octoon::Keyframes<float> fov;

		distance.reserve(keyframes.size());
		eyeX.reserve(keyframes.size());
		eyeY.reserve(keyframes.size());
		eyeZ.reserve(keyframes.size());
		rotation.reserve(keyframes.size());
		fov.reserve(keyframes.size());

		for (auto& it : keyframes)
		{
			auto interpolationDistance = std::make_shared<octoon::PathInterpolator<float>>(it.interpolation_distance[0] / 127.0f, it.interpolation_distance[2] / 127.0f, it.interpolation_distance[1] / 127.0f, it.interpolation_distance[3] / 127.0f);
			auto interpolationX = std::make_shared<octoon::PathInterpolator<float>>(it.interpolation_x[0] / 127.0f, it.interpolation_x[2] / 127.0f, it.interpolation_x[1] / 127.0f, it.interpolation_x[3] / 127.0f);
			auto interpolationY = std::make_shared<octoon::PathInterpolator<float>>(it.interpolation_y[0] / 127.0f, it.interpolation_y[2] / 127.0f, it.interpolation_y[1] / 127.0f, it.interpolation_y[3] / 127.0f);
			auto interpolationZ = std::make_shared<octoon::PathInterpolator<float>>(it.interpolation_z[0] / 127.0f, it.interpolation_z[2] / 127.0f, it.interpolation_z[1] / 127.0f, it.interpolation_z[3] / 127.0f);
			auto interpolationRotation = std::make_shared<octoon::PathInterpolator<float>>(it.interpolation_rotation[0] / 127.0f, it.interpolation_rotation[2] / 127.0f, it.interpolation_rotation[1] / 127.0f, it.interpolation_rotation[3] / 127.0f);
			auto interpolationAngleView = std::make_shared<octoon::PathInterpolator<float>>(it.interpolation_angleview[0] / 127.0f, it.interpolation_angleview[2] / 127.0f, it.interpolation_angleview[1] / 127.0f, it.interpolation_angleview[3] / 127.0f);

			distance.emplace_back((float)it.frame / 30.0f, it.distance, interpolationDistance);
			eyeX.emplace_back((float)it.frame / 30.0f, it.eye.x, interpolationX);
			eyeY.emplace_back((float)it.frame / 30.0f, it.eye.y, interpolationY);
			eyeZ.emplace_back((float)it.frame / 30.0f, it.eye.z, interpolationZ);
			rotation.emplace_back((float)it.frame / 30.0f, octoon::math::float3(it.rotation.x, it.rotation.y, it.rotation.z), interpolationRotation);
			fov.emplace_back((float)it.frame / 30.0f, (float)it.fov, interpolationAngleView);
		}

		auto clip = std::make_shared<octoon::AnimationClip>();
		clip->setCurve("", "LocalPosition.x", octoon::AnimationCurve(std::move(eyeX)));
		clip->setCurve("", "LocalPosition.y", octoon::AnimationCurve(std::move(eyeY)));
		clip->setCurve("", "LocalPosition.z", octoon::AnimationCurve(std::move(eyeZ)));
		clip->setCurve("", "LocalEulerAnglesRaw", octoon::AnimationCurve(std::move(rotation)));
		clip->setCurve("", "LocalForward", octoon::AnimationCurve(std::move(distance)));
		clip->setCurve("", "Camera:fov", octoon::AnimationCurve(std::move(fov)));

		animation.addClip(std::move(clip), "Camera");
	}

	void
	PMMLoader::setupBoneAnimation(const octoon::PmmModel& it, octoon::AnimationClip& clip) noexcept
	{
		std::size_t numBone = it.bone_init_frame.size();

		for (std::size_t i = 0; i < numBone; i++)
		{
			std::size_t keyframeCount = 1u;
			std::size_t next_index = it.bone_init_frame[i].next_index;
			std::size_t final_frame = it.bone_init_frame[i].frame;

			while (next_index > 0)
			{
				keyframeCount++;
				next_index -= it.bone_init_frame.size();
				final_frame = it.bone_key_frame[next_index].frame;
				next_index = it.bone_key_frame[next_index].next_index;
			}

			octoon::Keyframes<float> translateX;
			octoon::Keyframes<float> translateY;
			octoon::Keyframes<float> translateZ;
			octoon::Keyframes<float> rotation;

			translateX.reserve(keyframeCount);
			translateY.reserve(keyframeCount);
			translateZ.reserve(keyframeCount);
			rotation.reserve(keyframeCount);

			auto& initKey = it.bone_init_frame[i];
			auto interpolationX = std::make_shared<octoon::PathInterpolator<float>>(initKey.interpolation_x[0] / 127.0f, initKey.interpolation_x[2] / 127.0f, initKey.interpolation_x[1] / 127.0f, initKey.interpolation_x[3] / 127.0f);
			auto interpolationY = std::make_shared<octoon::PathInterpolator<float>>(initKey.interpolation_y[0] / 127.0f, initKey.interpolation_y[2] / 127.0f, initKey.interpolation_y[1] / 127.0f, initKey.interpolation_y[3] / 127.0f);
			auto interpolationZ = std::make_shared<octoon::PathInterpolator<float>>(initKey.interpolation_z[0] / 127.0f, initKey.interpolation_z[2] / 127.0f, initKey.interpolation_z[1] / 127.0f, initKey.interpolation_z[3] / 127.0f);
			auto interpolationRotation = std::make_shared<octoon::PathInterpolator<float>>(initKey.interpolation_rotation[0] / 127.0f, initKey.interpolation_rotation[2] / 127.0f, initKey.interpolation_rotation[1] / 127.0f, initKey.interpolation_rotation[3] / 127.0f);

			translateX.emplace_back((float)initKey.frame / 30.0f, initKey.translation.x, interpolationX);
			translateY.emplace_back((float)initKey.frame / 30.0f, initKey.translation.y, interpolationY);
			translateZ.emplace_back((float)initKey.frame / 30.0f, initKey.translation.z, interpolationZ);
			rotation.emplace_back((float)initKey.frame / 30.0f, octoon::math::Quaternion(initKey.quaternion.x, initKey.quaternion.y, initKey.quaternion.z, initKey.quaternion.w), interpolationRotation);

			auto next = it.bone_init_frame[i].next_index;
			while (next > 0)
			{
				auto& key = it.bone_key_frame[next - numBone];

				interpolationX = std::make_shared<octoon::PathInterpolator<float>>(key.interpolation_x[0] / 127.0f, key.interpolation_x[2] / 127.0f, key.interpolation_x[1] / 127.0f, key.interpolation_x[3] / 127.0f);
				interpolationY = std::make_shared<octoon::PathInterpolator<float>>(key.interpolation_y[0] / 127.0f, key.interpolation_y[2] / 127.0f, key.interpolation_y[1] / 127.0f, key.interpolation_y[3] / 127.0f);
				interpolationZ = std::make_shared<octoon::PathInterpolator<float>>(key.interpolation_z[0] / 127.0f, key.interpolation_z[2] / 127.0f, key.interpolation_z[1] / 127.0f, key.interpolation_z[3] / 127.0f);
				interpolationRotation = std::make_shared<octoon::PathInterpolator<float>>(key.interpolation_rotation[0] / 127.0f, key.interpolation_rotation[2] / 127.0f, key.interpolation_rotation[1] / 127.0f, key.interpolation_rotation[3] / 127.0f);

				translateX.emplace_back((float)key.frame / 30.0f, key.translation.x, interpolationX);
				translateY.emplace_back((float)key.frame / 30.0f, key.translation.y, interpolationY);
				translateZ.emplace_back((float)key.frame / 30.0f, key.translation.z, interpolationZ);
				rotation.emplace_back((float)key.frame / 30.0f, octoon::math::Quaternion(key.quaternion.x, key.quaternion.y, key.quaternion.z, key.quaternion.w), interpolationRotation);

				next = key.next_index;
			}

			clip.setCurve(it.bone_name[i], "LocalPosition.x", octoon::AnimationCurve(std::move(translateX)));
			clip.setCurve(it.bone_name[i], "LocalPosition.y", octoon::AnimationCurve(std::move(translateY)));
			clip.setCurve(it.bone_name[i], "LocalPosition.z", octoon::AnimationCurve(std::move(translateZ)));
			clip.setCurve(it.bone_name[i], "LocalRotation", octoon::AnimationCurve(std::move(rotation)));
		}
	}

	void
	PMMLoader::setupMorphAnimation(const octoon::PmmModel& it, octoon::AnimationClip& clip) noexcept
	{
		for (std::size_t i = 0, keyframeCount = 1; i < it.morph_init_frame.size(); i++)
		{
			auto index = it.morph_init_frame[i].next_index;
			while (index > 0)
			{
				keyframeCount++;
				index = it.morph_key_frame[index - it.morph_init_frame.size()].next_index;
			}

			octoon::Keyframes<float> keyframes;
			keyframes.reserve(keyframeCount);
			keyframes.emplace_back((float)it.morph_init_frame[i].frame / 30.0f, it.morph_init_frame[i].value);

			std::size_t next = it.morph_init_frame[i].next_index;
			while (next > 0)
			{
				next -= it.morph_init_frame.size();
				auto& frame = it.morph_key_frame[next];
				keyframes.emplace_back((float)frame.frame / 30.0f, frame.value);
				next = frame.next_index;
			}

			clip.setCurve("", it.morph_name[i], octoon::AnimationCurve(std::move(keyframes)));
		}
	}
}