#include "project_controller.h"

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

#include "../libs/nativefiledialog/nfd.h"

using namespace octoon::animation;

constexpr std::size_t PATHLIMIT = 4096;

std::vector<const char*> g_SupportedProject = { "pmm" };
std::vector<const char*> g_SupportedModel = { "pmx" };
std::vector<const char*> g_SupportedImage = { "png", "jpg" };

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(ProjectController, GameComponent, "ProjectController")

		ProjectController::ProjectController() noexcept
		{
		}

		ProjectController::~ProjectController() noexcept
		{
		}

		bool 
		ProjectController::showFileOpenBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept
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
		ProjectController::showFileSaveBrowse(std::string::pointer buffer, std::uint32_t max_length, std::string::const_pointer ext_name) noexcept
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

		void
		ProjectController::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
			this->addMessageListener("editor:menu:file:open", std::bind(&ProjectController::openProject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:file:save", std::bind(&ProjectController::saveProject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:file:saveAs", std::bind(&ProjectController::saveAsProject, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:file:import", std::bind(&ProjectController::openModel, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:file:export", std::bind(&ProjectController::saveModel, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:file:exit", std::bind(&ProjectController::exit, this, std::placeholders::_1));
			this->addMessageListener("editor:menu:setting:render", std::bind(&ProjectController::play, this, std::placeholders::_1));	
			this->addMessageListener("editor:menu:file:picture", std::bind(&ProjectController::renderPicture, this, std::placeholders::_1));
			
		}

		void
		ProjectController::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
			this->removeMessageListener("editor:menu:file:open", std::bind(&ProjectController::openProject, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:file:save", std::bind(&ProjectController::saveProject, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:file:saveAs", std::bind(&ProjectController::saveAsProject, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:file:import", std::bind(&ProjectController::openModel, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:file:export", std::bind(&ProjectController::saveModel, this, std::placeholders::_1));
			this->removeMessageListener("editor:menu:file:picture", std::bind(&ProjectController::renderPicture, this, std::placeholders::_1));
		}

		void
		ProjectController::openProject(const runtime::any& data) noexcept
		{			
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileOpenBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
				return;

			try
			{
				auto stream = io::ifstream(filepath);
				auto pmm = PMMFile::load(stream).value();
				for (auto& it : pmm.model)
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

						auto interpolationX = std::make_shared<PathInterpolator<float>>(key.interpolation_x[0] / 255.0f, key.interpolation_x[1] / 255.0f, key.interpolation_x[2] / 255.0f, key.interpolation_x[3] / 255.0f);
						auto interpolationY = std::make_shared<PathInterpolator<float>>(key.interpolation_y[0] / 255.0f, key.interpolation_y[1] / 255.0f, key.interpolation_y[2] / 255.0f, key.interpolation_y[3] / 255.0f);
						auto interpolationZ = std::make_shared<PathInterpolator<float>>(key.interpolation_z[0] / 255.0f, key.interpolation_z[1] / 255.0f, key.interpolation_z[2] / 255.0f, key.interpolation_z[3] / 255.0f);
						auto interpolationRotation = std::make_shared<PathInterpolator<float>>(key.interpolation_rotation[0] / 255.0f, key.interpolation_rotation[1] / 255.0f, key.interpolation_rotation[2] / 255.0f, key.interpolation_rotation[3] / 255.0f);

						auto euler = math::eulerAngles(key.quaternion);

						translateX[index].emplace_back((float)key.frame / 30.0f, key.translation.x, interpolationX);
						translateY[index].emplace_back((float)key.frame / 30.0f, key.translation.y, interpolationY);
						translateZ[index].emplace_back((float)key.frame / 30.0f, key.translation.z, interpolationZ);
						rotationX[index].emplace_back((float)key.frame / 30.0f, euler.x, interpolationRotation);
						rotationY[index].emplace_back((float)key.frame / 30.0f, euler.y, interpolationRotation);
						rotationZ[index].emplace_back((float)key.frame / 30.0f, euler.z, interpolationRotation);
					}

					AnimationClips<float> clips(it.bone_init_frame.size());

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

					auto model = GamePrefabs::instance()->createOfflineModel(it.path);
					if (model)
					{
						model->setName(it.name);
						model->getComponent<AnimatorComponent>()->setClips(clips);

						objects_.emplace_back(std::move(model));
					}
				}

				auto mainLight = GameObject::create("DirectionalLight");
				mainLight->addComponent<OfflineDirectionalLightComponent>();
				mainLight->getComponent<OfflineDirectionalLightComponent>()->setIntensity(10.0f);
				mainLight->getComponent<OfflineDirectionalLightComponent>()->setColor(pmm.main_light.rgb);
				mainLight->getComponent<TransformComponent>()->setQuaternion(math::Quaternion(math::float3::Forward, math::normalize(-pmm.main_light.xyz)));
				objects_.push_back(mainLight);

				auto camera = this->createCamera(pmm);
				if (camera)
					objects_.emplace_back(std::move(camera));

				auto obj = GameObject::create("EnvironmentLight");
				obj->addComponent<OfflineEnvironmentLightComponent>();
				obj->getComponent<OfflineEnvironmentLightComponent>()->setIntensity(4.0f);

				objects_.push_back(obj);
			}
			catch (const std::bad_optional_access&)
			{
				this->sendMessage("editor:message:error", "Failed to open the file");
			}
		}

		void
		ProjectController::saveProject(const runtime::any& data) noexcept
		{
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
				return;
		}

		void
		ProjectController::saveAsProject(const runtime::any& data) noexcept
		{
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedProject[0]))
				return;
		}

		void
		ProjectController::openModel(const runtime::any& data) noexcept
		{
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileOpenBrowse(filepath, PATHLIMIT, g_SupportedModel[0]))
				return;

			objects_.push_back(GamePrefabs::instance()->createModel(filepath));
		}

		void
		ProjectController::saveModel(const runtime::any& data) noexcept
		{
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedModel[0]))
				return;
		}

		void
		ProjectController::renderPicture(const runtime::any& data) noexcept
		{
			std::string::value_type filepath[PATHLIMIT];
			std::memset(filepath, 0, sizeof(filepath));

			if (!showFileSaveBrowse(filepath, PATHLIMIT, g_SupportedImage[0]))
				return;

			auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
			if (feature)
				feature->saveToFile(filepath);
		}

		void 
		ProjectController::exit(const runtime::any& data) noexcept
		{
			std::exit(0);
		}

		void
		ProjectController::play(const runtime::any& data) noexcept
		{
			assert(data.type() == typeid(bool));

			auto play = runtime::any_cast<bool>(data);
			if (play)
			{
				for (auto& it : objects_)
				{
					auto animation = it->getComponent<AnimationComponent>();
					if (animation)
						animation->play();

					auto animator = it->getComponent<AnimatorComponent>();
					if (animator)
						animator->play();
				}
			}
			else
			{
				for (auto& it : objects_)
				{
					auto animation = it->getComponent<AnimationComponent>();
					if (animation)
						animation->stop();

					auto animator = it->getComponent<AnimatorComponent>();
					if (animator)
						animator->stop();
				}
			}
		}

		GameComponentPtr 
		ProjectController::clone() const noexcept
		{
			return std::make_shared<ProjectController>();
		}

		GameObjectPtr
		ProjectController::createCamera(const PMMFile& pmm) noexcept
		{
			Keyframes<float> distance;
			Keyframes<float> eyeX;
			Keyframes<float> eyeY;
			Keyframes<float> eyeZ;
			Keyframes<float> rotationX;
			Keyframes<float> rotationY;
			Keyframes<float> rotationZ;
			Keyframes<float> fov;

			for (auto& it : pmm.camera_keyframes)
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
				fov.emplace_back((float)it.frame / 30.0f, (float)it.fov * 2.0f, interpolationAngleView);
			}

			AnimationClip<float> clip;
			clip.setCurve("LocalPosition.x", AnimationCurve(std::move(eyeX)));
			clip.setCurve("LocalPosition.y", AnimationCurve(std::move(eyeY)));
			clip.setCurve("LocalPosition.z", AnimationCurve(std::move(eyeZ)));
			clip.setCurve("LocalEulerAnglesRaw.x", AnimationCurve(std::move(rotationX)));
			clip.setCurve("LocalEulerAnglesRaw.y", AnimationCurve(std::move(rotationY)));
			clip.setCurve("LocalEulerAnglesRaw.z", AnimationCurve(std::move(rotationZ)));
			clip.setCurve("Transform:move", AnimationCurve(std::move(distance)));
			clip.setCurve("Camera:fov", AnimationCurve(std::move(fov)));

			auto obj = GameObject::create("MainCamera");
			obj->addComponent<AnimationComponent>(clip);
			obj->addComponent<EditorCameraComponent>();
			obj->getComponent<TransformComponent>()->setQuaternion(math::Quaternion(pmm.camera_keyframes[0].rotation));
			obj->getComponent<TransformComponent>()->setTranslate(pmm.camera_keyframes[0].eye);
			obj->getComponent<TransformComponent>()->setTranslateAccum(math::rotate(math::Quaternion(pmm.camera_keyframes[0].rotation), math::float3::Forward) * pmm.camera_keyframes[0].distance);
			obj->addComponent<OfflineCameraComponent>();

			auto camera = obj->addComponent<PerspectiveCameraComponent>(pmm.camera_keyframes[0].fov * 2.0f);
			camera->setCameraType(video::CameraType::Main);
			camera->setClearFlags(octoon::hal::GraphicsClearFlagBits::DepthStencilBit);
			camera->setClearColor(octoon::math::float4(0.2f, 0.2f, 0.2f, 1.0f));

			this->sendMessage("editor:camera:set", obj);

			return obj;
		}
	}
}