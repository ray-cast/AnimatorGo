#include <octoon/animator_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_scene.h>

namespace octoon
{
	OctoonImplementSubClass(AnimatorComponent, GameComponent, "Animator")

	AnimatorComponent::AnimatorComponent() noexcept
		: enableAnimation_(true)
		, enableAnimOnVisableOnly_(false)
		, needUpdate_(false)
		, timer_(nullptr)
	{
	}

	AnimatorComponent::AnimatorComponent(animation::AnimationClip<float>&& clip) noexcept
		: AnimatorComponent()
	{
		this->clips_.emplace_back(clip);
	}

	AnimatorComponent::AnimatorComponent(animation::AnimationClips<float>&& clips) noexcept
		: AnimatorComponent()
	{
		this->clips_ = std::move(clips);
	}

	AnimatorComponent::AnimatorComponent(const animation::AnimationClip<float>& clip) noexcept
		: AnimatorComponent()
	{
		this->clips_.emplace_back(clip);
	}

	AnimatorComponent::AnimatorComponent(const animation::AnimationClips<float>& clips) noexcept
		: AnimatorComponent()
	{
		this->clips_ = clips;
	}

	AnimatorComponent::~AnimatorComponent() noexcept
	{
	}

	bool
	AnimatorComponent::play(const std::string& filename) noexcept
	{
		if (this->getName() != filename)
		{
			_playAnimation(filename);
			this->setName(filename);
		}

		enableAnimation_ = true;

		return enableAnimation_;
	}

	void
	AnimatorComponent::pause() noexcept
	{
		enableAnimation_ = false;
	}

	void
	AnimatorComponent::stop() noexcept
	{
		_destroyAnimation();
	}

	void
	AnimatorComponent::setTransforms(GameObjects&& transforms) noexcept
	{
		transforms_ = std::move(transforms);
	}

	void
	AnimatorComponent::setTransforms(const GameObjects& transforms) noexcept
	{
		transforms_ = transforms;
	}

	const GameObjects&
	AnimatorComponent::getTransforms() const noexcept
	{
		return transforms_;
	}

	GameComponentPtr
	AnimatorComponent::clone() const noexcept
	{
		auto animtion = std::make_shared<AnimatorComponent>();
		animtion->transforms_ = transforms_;
		return animtion;
	}

	void 
	AnimatorComponent::onActivate() except
	{
		timer_ = this->getGameObject()->getGameScene()->getFeature<TimerFeature>();
	}

	void
	AnimatorComponent::onDeactivate() noexcept
	{
		timer_ = nullptr;
		this->removeComponentDispatch(GameDispatchType::FrameEnd);
	}

	void
	AnimatorComponent::onFrameEnd() noexcept
	{
		if (enableAnimation_ && !enableAnimOnVisableOnly_)
			_updateAnimation();
		else
			needUpdate_ = true;
	}

	bool
	AnimatorComponent::_playAnimation(const std::string& filename) noexcept
	{
		/*this->_destroyAnimation();

		if (filename.empty())
			return false;

		ResLoader<Model> loader;
		loader.load(filename,
			[&](ray::ModelPtr model, const std::string& name)
		{
			StreamReaderPtr stream;
			if (IoServer::instance()->openFileURL(stream, name))
				return model->load(*stream);
			return false;
		}
		);

		auto model = loader.data();
		if (!model || !model->hasAnimations())
			return false;

		std::intptr_t i = 0;
		std::map<util::string, std::intptr_t> boneMap;
		for (auto& it : transforms_)
		{
			boneMap[it->getName()] = i++;
		}

		std::size_t index = 0;

		std::vector<Bone> bones;
		std::vector<IKAttr> iks;

		for (auto& it : transforms_)
		{
			Bone bone;
			bone.setName(it->getName());
			bone.setPosition(it->getWorldTranslate());
			if (it->getParent())
				bone.setParent(boneMap[it->getParent()->getName()]);
			else
				bone.setParent(-1);

			auto iksolver = it->getComponent<IKSolverComponent>();
			if (iksolver)
			{
				IKAttr attr;
				attr.boneIndex = index;
				attr.targetBoneIndex = boneMap[iksolver->getTargetBone()->getName()];
				attr.chainLength = iksolver->getChainLength();
				attr.iterations = iksolver->getIterations();

				for (auto& child : iksolver->getBones())
				{
					IKChild ikbone;
					ikbone.boneIndex = boneMap[child->bone->getName()];
					ikbone.angleWeight = child->angleWeight;
					ikbone.rotateLimited = child->rotateLimited;
					ikbone.minimumDegrees = child->minimumDegrees;
					ikbone.maximumDegrees = child->maximumDegrees;

					attr.child.push_back(ikbone);
				}

				iks.push_back(attr);
			}

			bones.push_back(bone);

			++index;
		}

		_animtion = model->getAnimationList().back()->clone();
		_animtion->setBoneArray(bones);
		_animtion->setIKArray(iks);
		_animtion->updateMotion();

		_enableAnimation = true;*/
		return true;
	}

	void
	AnimatorComponent::_updateAnimation() noexcept
	{
		/*_animtion->updateFrame(this->getGameObject()->getGameScene()->getFeature<TimerFeature>()->delta());
		_animtion->updateMotion();

		std::size_t i = 0;
		for (auto& it : _animtion->getBoneArray())
			transforms_[i++]->getComponent<TransformComponent>()->setTransformOnlyRotate(it.getTransform());*/
	}

	void
	AnimatorComponent::_destroyAnimation() noexcept
	{
		enableAnimation_ = false;
	}
}