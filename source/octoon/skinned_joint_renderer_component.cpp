#include <octoon/skinned_joint_renderer_component.h>
#include <octoon/transform_component.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedJointRendererComponent, RenderComponent, "JointRenderer")

	SkinnedJointRendererComponent::SkinnedJointRendererComponent() noexcept
		: needUpdate_(true)
	{
	}

	SkinnedJointRendererComponent::SkinnedJointRendererComponent(material::MaterialPtr&& material) noexcept
		: needUpdate_(true)
	{
		this->setMaterial(std::move(material));
	}

	SkinnedJointRendererComponent::SkinnedJointRendererComponent(const material::MaterialPtr& material) noexcept
		: needUpdate_(true)
	{
		this->setMaterial(material);
	}

	SkinnedJointRendererComponent::~SkinnedJointRendererComponent() noexcept
	{
	}

	void
	SkinnedJointRendererComponent::setTransforms(const GameObjects& transforms) noexcept
	{
		transforms_ = transforms;
	}

	void
	SkinnedJointRendererComponent::setTransforms(GameObjects&& transforms) noexcept
	{
		transforms_ = std::move(transforms);
	}

	const GameObjects&
	SkinnedJointRendererComponent::getTransforms() const noexcept
	{
		return transforms_;
	}

	GameComponentPtr
	SkinnedJointRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SkinnedJointRendererComponent>();
		instance->setName(this->getName());
		instance->setTransforms(this->getTransforms());
		instance->setMaterial(this->getMaterial() ? (this->isSharedMaterial() ? this->getMaterial() : this->getMaterial()->clone()) : nullptr, this->isSharedMaterial());

		return instance;
	}

	void
	SkinnedJointRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::FixedUpdate);
		this->addMessageListener("octoon:animation:update", std::bind(&SkinnedJointRendererComponent::onAnimationUpdate, this, std::placeholders::_1));
		MeshRendererComponent::onActivate();
	}

	void
	SkinnedJointRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
		this->removeMessageListener("octoon:animation:update", std::bind(&SkinnedJointRendererComponent::onAnimationUpdate, this, std::placeholders::_1));
		MeshRendererComponent::onDeactivate();
	}

	void
	SkinnedJointRendererComponent::onFixedUpdate() noexcept
	{
		this->needUpdate_ = true;
	}

	void
	SkinnedJointRendererComponent::onAnimationUpdate(const runtime::any& data) noexcept
	{
		this->needUpdate_ = true;
	}

	void
	SkinnedJointRendererComponent::onPreRender(const video::Camera& camera) noexcept
	{
		if (needUpdate_)
		{
			auto meshes = std::make_shared<mesh::Mesh>();
			auto& vertices = meshes->getVertexArray();

			for (auto& transfrom : transforms_)
			{
				if (transfrom->getParent())
				{
					vertices.push_back(transfrom->getParent()->getComponent<TransformComponent>()->getTranslate());
					vertices.push_back(transfrom->getComponent<TransformComponent>()->getTranslate());
				}
			}

			if (!vertices.empty())
				this->uploadMeshData(meshes);

			needUpdate_ = false;
		}
	}
}