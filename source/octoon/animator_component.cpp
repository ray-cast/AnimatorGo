#include <octoon/animator_component.h>
#include <octoon/transform_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/asset_database.h>
#include <octoon/asset_importer.h>
#include <octoon/timer_feature.h>
#include <octoon/runtime/guid.h>

namespace octoon
{
	OctoonImplementSubClass(AnimatorComponent, AnimationComponent, "Animator")

	AnimatorComponent::AnimatorComponent() noexcept
		: enableAnimation_(true)
		, enableAnimOnVisableOnly_(false)
	{
		animatorStateInfo_.finish = false;
		animatorStateInfo_.time = 0;
		animatorStateInfo_.timeLength = 0;
	}

	AnimatorComponent::AnimatorComponent(std::shared_ptr<Animation>&& animation, GameObjects&& avatar) noexcept
		: AnimatorComponent()
	{
		this->setAvatar(std::move(avatar));
		this->setAnimation(std::move(animation));
	}

	AnimatorComponent::AnimatorComponent(std::shared_ptr<Animation>&& animation, const GameObjects& avatar) noexcept
		: AnimatorComponent()
	{
		this->setAvatar(avatar);
		this->setAnimation(std::move(animation));
	}

	AnimatorComponent::AnimatorComponent(const std::shared_ptr<Animation>& animation, GameObjects&& avatar) noexcept
		: AnimatorComponent()
	{
		this->setAvatar(std::move(avatar));
		this->setAnimation(animation);
	}

	AnimatorComponent::AnimatorComponent(const std::shared_ptr<Animation>& animation, const GameObjects& avatar) noexcept
		: AnimatorComponent()
	{
		this->setAvatar(avatar);
		this->setAnimation(animation);
	}

	AnimatorComponent::AnimatorComponent(std::shared_ptr<Animation>&& animation) noexcept
		: AnimatorComponent()
	{
		animation_ = std::move(animation);
	}

	AnimatorComponent::AnimatorComponent(const std::shared_ptr<Animation>& animation) noexcept
		: AnimatorComponent()
	{
		animation_ = animation;
	}

	AnimatorComponent::AnimatorComponent(GameObjects&& avatar) noexcept
		: AnimatorComponent()
	{
		this->setAvatar(std::move(avatar));
	}

	AnimatorComponent::AnimatorComponent(const GameObjects& avatar) noexcept
		: AnimatorComponent()
	{
		this->setAvatar(avatar);
	}

	AnimatorComponent::~AnimatorComponent() noexcept
	{
	}

	bool
	AnimatorComponent::play(std::string_view status) noexcept
	{
		if (animation_ && animation_->hasClip(status))
		{
			this->animation_->setDefaultClip(status);
			this->addComponentDispatch(GameDispatchType::FixedUpdate);

			enableAnimation_ = true;
			return enableAnimation_;
		}

		return false;
	}

	void
	AnimatorComponent::pause() noexcept
	{
		enableAnimation_ = false;
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	AnimatorComponent::reset() noexcept
	{
		this->setTime(0.0f);
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	AnimatorComponent::setTime(float time) noexcept
	{
		if (this->animatorStateInfo_.time != time)
		{
			if (animation_)
			{
				for (auto& it : animation_->clips)
					it.second->setTime(time);

				this->animatorStateInfo_.time = time;
				this->animatorStateInfo_.finish = true;

				for (auto& it : animation_->clips)
					this->animatorStateInfo_.finish &= it.second->finish;
			}
			else
			{
				this->animatorStateInfo_.time = time;
				this->animatorStateInfo_.finish = true;
			}
		}
	}

	float
	AnimatorComponent::getTime() const noexcept
	{
		return this->animatorStateInfo_.time;
	}

	void
	AnimatorComponent::sample(float delta) noexcept
	{
		if (animation_ && !animation_->empty())
		{
			if (delta != 0.0f)
			{
				this->animatorStateInfo_.time += delta;
				this->animation_->evaluate(delta);
				this->animatorStateInfo_.finish = animation_->clip->finish;
			}

			if (!avatar_.empty())
			{
				this->updateAvatar();

				for (auto& bone : avatar_)
				{
					for (auto& child : bone->getChildren())
					{
						auto rigidbody = child->getComponent<RigidbodyComponent>();
						if (rigidbody)
						{
							auto transform = child->getComponent<TransformComponent>();
							rigidbody->setPositionAndRotation(transform->getTranslate(), transform->getRotation());
							rigidbody->setLinearVelocity(math::float3::Zero);
							rigidbody->setAngularVelocity(math::float3::Zero);
							rigidbody->setInterpolationLinearVelocity(math::float3::Zero);
							rigidbody->setInterpolationAngularVelocity(math::float3::Zero);
							rigidbody->clearForce();
						}
					}
				}
			}
			else
			{
				this->updateAnimation();
			}
		}
	}

	void
	AnimatorComponent::evaluate(float delta) noexcept
	{
		if (animation_ && !animation_->empty())
		{
			if (delta != 0.0f)
			{
				this->animatorStateInfo_.time += delta;
				this->animation_->evaluate(delta);
				this->animatorStateInfo_.finish = animation_->clip->finish;
			}

			if (!avatar_.empty())
				this->updateAvatar();
			else
				this->updateAnimation();
		}
	}

	void
	AnimatorComponent::setAnimation(std::shared_ptr<Animation>&& animtion) noexcept
	{
		if (animtion)
		{
			animation_ = std::move(animtion);

			for (auto& it : animation_->clips)
			{
				animatorStateInfo_.finish &= it.second->finish;
				animatorStateInfo_.timeLength = std::max(it.second->timeLength, animatorStateInfo_.timeLength);
			}

			animatorStateInfo_.time = std::min(animatorStateInfo_.time, animatorStateInfo_.timeLength);
			animation_->setTime(animatorStateInfo_.time);
		}
		else
		{
			animatorStateInfo_.time = 0;
			animatorStateInfo_.timeLength = 0;
			animatorStateInfo_.finish = true;
		}

		this->updateBindmap();
	}

	void
	AnimatorComponent::setAnimation(const std::shared_ptr<Animation>& animtion) noexcept
	{
		if (animtion)
		{
			animation_ = animtion;

			for (auto& it : animation_->clips)
			{
				animatorStateInfo_.finish &= it.second->finish;
				animatorStateInfo_.timeLength = std::max(it.second->timeLength, animatorStateInfo_.timeLength);
			}

			animatorStateInfo_.time = std::min(animatorStateInfo_.time, animatorStateInfo_.timeLength);
			animation_->setTime(animatorStateInfo_.time);
		}
		else
		{
			animatorStateInfo_.time = 0;
			animatorStateInfo_.timeLength = 0;
			animatorStateInfo_.finish = true;
		}

		this->updateBindmap();
	}

	const std::shared_ptr<Animation>&
	AnimatorComponent::getAnimation() const noexcept
	{
		return animation_;
	}

	void
	AnimatorComponent::setAvatar(GameObjects&& avatar) noexcept
	{
		avatar_ = std::move(avatar);
		this->updateBindpose(avatar_);
		this->updateBindmap();
	}

	void
	AnimatorComponent::setAvatar(const GameObjects& avatar) noexcept
	{
		avatar_ = avatar;
		this->updateBindpose(avatar_);
		this->updateBindmap();
	}

	const GameObjects&
	AnimatorComponent::getAvatar() const noexcept
	{
		return avatar_;
	}

	const AnimatorStateInfo&
	AnimatorComponent::getCurrentAnimatorStateInfo() const noexcept
	{
		return animatorStateInfo_;
	}

	void
	AnimatorComponent::load(const nlohmann::json& json) noexcept(false)
	{
		if (json.contains("animation"))
		{
			auto& data = json["animation"];

			auto guid = data["guid"].get<std::string>();
			auto localId = data["localId"].get<int>();

			auto animation = AssetDatabase::instance()->loadAsset<Animation>(guid, localId);
			if (animation)
				this->setAnimation(std::move(animation));
		}

		if (json.contains("avatar"))
		{
			GameObjects avatar;
			
			for (auto& it : json["avatar"])
			{
				auto guid = it["guid"].get<std::string>();
				auto localId = it["localId"].get<int>();

				avatar.push_back(AssetDatabase::instance()->loadAsset<GameObject>(guid, localId));
			}

			this->setAvatar(std::move(avatar));
		}
	}

	void
	AnimatorComponent::save(nlohmann::json& json) const noexcept(false)
	{
		if (this->getAnimation())
		{
			if (!AssetDatabase::instance()->contains(this->getAnimation()))
			{
				auto path = std::filesystem::path("Assets/Motions").append(make_guid() + ".vmd");
				AssetDatabase::instance()->createFolder(std::filesystem::path("Assets/Motions"));
				AssetDatabase::instance()->createAsset(this->getAnimation(), path);
			}

			std::string guid;
			std::int64_t localId;

			if (AssetDatabase::instance()->getGUIDAndLocalIdentifier(this->getAnimation(), guid, localId))
			{
				nlohmann::json animation;
				animation["guid"] = guid;
				animation["localId"] = localId;

				json["animation"] = std::move(animation);
			}
		}

		if (!this->getAvatar().empty())
		{
			auto& avatarJson = json["avatar"];

			for (auto& it : avatar_)
			{
				std::string guid;
				std::int64_t localId;

				if (AssetDatabase::instance()->getGUIDAndLocalIdentifier(it, guid, localId))
				{
					nlohmann::json avatar;
					avatar["guid"] = guid;
					avatar["localId"] = localId;

					avatarJson.push_back(std::move(avatar));
				}
			}
		}
	}

	GameComponentPtr
	AnimatorComponent::clone() const noexcept
	{
		auto instance = std::make_shared<AnimatorComponent>();
		instance->setName(this->getName());
		instance->setAvatar(this->getAvatar());
		instance->setAnimation(this->getAnimation());

		return instance;
	}

	void 
	AnimatorComponent::onActivate() except
	{
	}

	void
	AnimatorComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
	}

	void
	AnimatorComponent::onFixedUpdate() noexcept
	{
		if (enableAnimation_ && animation_)
		{
			auto timeFeature = this->getFeature<TimerFeature>();
			if (timeFeature)
			{
				auto delta = timeFeature->delta();
				if (delta != 0.0f)
					animation_->evaluate(delta);

				if (!avatar_.empty())
					this->updateAvatar();
				else
					this->updateAnimation();
			}
		}
	}

	void
	AnimatorComponent::updateBindpose(const GameObjects& avatar) noexcept
	{
		bindpose_.resize(avatar.size());

		for (std::size_t i = 0; i < avatar.size(); i++)
			bindpose_[i] = avatar[i]->getComponent<TransformComponent>()->getLocalTranslate();
	}

	void
	AnimatorComponent::updateBindmap() noexcept
	{
		bindmap_.clear();

		if (animation_ && animation_->clip && !avatar_.empty())
		{
			std::unordered_map<std::string, std::size_t> boneMap;

			for (std::size_t i = 0; i < avatar_.size(); i++)
				boneMap[avatar_[i]->getName()] = i;

			auto& clip = animation_->clip;
			bindmap_.reserve(clip->bindings.size());

			for (auto& binding : clip->bindings)
			{
				auto it = boneMap.find(binding.first);
				if (it != boneMap.end())
					bindmap_[binding.first] = (*it).second;
			}
		}
	}

	void
	AnimatorComponent::updateAvatar(float delta) noexcept
	{
		if (this->getCurrentAnimatorStateInfo().finish)
			return;

		for (auto& binding : animation_->clip->bindings)
		{
			if (!bindmap_.contains(binding.first))
				continue;

			auto& index = bindmap_[binding.first];

			auto transform = avatar_[index]->getComponent<TransformComponent>();
			auto scale = transform->getLocalScale();
			auto quat = transform->getLocalQuaternion();
			auto translate = transform->getLocalTranslate();
			auto euler = transform->getLocalEulerAngles();
			auto move = 0.0f;

			for (auto& curve : binding.second)
			{
				if (curve.first == "LocalPosition")
					translate = curve.second.value.getFloat3() + bindpose_[index];
				if (curve.first == "LocalPosition.x")
					translate.x = curve.second.value.getFloat() + bindpose_[index].x;
				else if (curve.first == "LocalPosition.y")
					translate.y = curve.second.value.getFloat() + bindpose_[index].y;
				else if (curve.first == "LocalPosition.z")
					translate.z = curve.second.value.getFloat() + bindpose_[index].z;
				else if (curve.first == "LocalScale")
					scale = curve.second.value.getFloat3();
				else if (curve.first == "LocalScale.x")
					scale.x = curve.second.value.getFloat();
				else if (curve.first == "LocalScale.y")
					scale.y = curve.second.value.getFloat();
				else if (curve.first == "LocalScale.z")
					scale.z = curve.second.value.getFloat();
				else if (curve.first == "LocalRotation")
					quat = curve.second.value.getQuaternion();
				else if (curve.first == "LocalRotation.x")
					quat.x = curve.second.value.getFloat();
				else if (curve.first == "LocalRotation.y")
					quat.y = curve.second.value.getFloat();
				else if (curve.first == "LocalRotation.z")
					quat.z = curve.second.value.getFloat();
				else if (curve.first == "LocalRotation.w")
					quat.w = curve.second.value.getFloat();
				else if (curve.first == "LocalForward")
					move = curve.second.value.getFloat();
				else if (curve.first == "LocalEulerAnglesRaw")
				{
					euler = curve.second.value.getFloat3();
					quat = math::Quaternion(euler);
				}
				else if (curve.first == "LocalEulerAnglesRaw.x")
				{
					euler.x = curve.second.value.getFloat();
					quat = math::Quaternion(euler);
				}
				else if (curve.first == "LocalEulerAnglesRaw.y")
				{
					euler.y = curve.second.value.getFloat();
					quat = math::Quaternion(euler);
				}
				else if (curve.first == "LocalEulerAnglesRaw.z")
				{
					euler.z = curve.second.value.getFloat();
					quat = math::Quaternion(euler);
				}
				else
				{
					this->sendMessage(curve.first, curve.second.value);
				}
			}

			if (move != 0.0f)
			{
				transform->setLocalScale(scale);
				transform->setLocalTranslate(translate + math::rotate(quat, math::float3::Forward) * move);
				transform->setLocalQuaternion(quat);
			}
			else
			{
				transform->setLocalScale(scale);
				transform->setLocalTranslate(translate);
				transform->setLocalQuaternion(quat);
			}
		}

		this->sendMessage("octoon:animation:update");
	}

	void
	AnimatorComponent::updateAnimation(float delta) noexcept
	{
		if (this->getCurrentAnimatorStateInfo().finish)
			return;

		for (auto& binging : animation_->clip->bindings)
		{
			auto transform = this->getComponent<TransformComponent>();
			auto scale = transform->getLocalScale();
			auto quat = transform->getLocalQuaternion();
			auto translate = transform->getLocalTranslate();
			auto euler = transform->getLocalEulerAngles();
			auto move = 0.0f;

			for (auto& curve : binging.second)
			{
				if (curve.first == "LocalPosition")
					translate = curve.second.value.getFloat3();
				if (curve.first == "LocalPosition.x")
					translate.x = curve.second.value.getFloat();
				else if (curve.first == "LocalPosition.y")
					translate.y = curve.second.value.getFloat();
				else if (curve.first == "LocalPosition.z")
					translate.z = curve.second.value.getFloat();
				else if (curve.first == "LocalScale")
					scale = curve.second.value.getFloat3();
				else if (curve.first == "LocalScale.x")
					scale.x = curve.second.value.getFloat();
				else if (curve.first == "LocalScale.y")
					scale.y = curve.second.value.getFloat();
				else if (curve.first == "LocalScale.z")
					scale.z = curve.second.value.getFloat();
				else if (curve.first == "LocalRotation")
					quat = curve.second.value.getQuaternion();
				else if (curve.first == "LocalRotation.x")
					quat.x = curve.second.value.getFloat();
				else if (curve.first == "LocalRotation.y")
					quat.y = curve.second.value.getFloat();
				else if (curve.first == "LocalRotation.z")
					quat.z = curve.second.value.getFloat();
				else if (curve.first == "LocalRotation.w")
					quat.w = curve.second.value.getFloat();
				else if (curve.first == "LocalForward")
					move = curve.second.value.getFloat();
				else if (curve.first == "LocalEulerAnglesRaw")
				{
					euler = curve.second.value.getFloat3();
					quat = math::Quaternion(euler);
				}
				else if (curve.first == "LocalEulerAnglesRaw.x")
				{
					euler.x = curve.second.value.getFloat();
					quat = math::Quaternion(euler);
				}
				else if (curve.first == "LocalEulerAnglesRaw.y")
				{
					euler.y = curve.second.value.getFloat();
					quat = math::Quaternion(euler);
				}
				else if (curve.first == "LocalEulerAnglesRaw.z")
				{
					euler.z = curve.second.value.getFloat();
					quat = math::Quaternion(euler);
				}
				else
					this->sendMessage(curve.first, curve.second.value);
			}

			if (move != 0.0f)
			{
				transform->setLocalScale(scale);
				transform->setLocalTranslate(translate + math::rotate(quat, math::float3::Forward) * move);
				transform->setLocalQuaternion(quat);
			}
			else
			{
				transform->setLocalScale(scale);
				transform->setLocalTranslate(translate);
				transform->setLocalQuaternion(quat);
			}
		}

		this->sendMessage("octoon:animation:update");
	}
}