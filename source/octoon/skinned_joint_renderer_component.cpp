#include <octoon/skinned_joint_renderer_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/video/render_system.h>
#include <iostream>

namespace octoon
{
	OctoonImplementSubClass(SkinnedJointRendererComponent, RenderComponent, "JointRenderer")

	SkinnedJointRendererComponent::SkinnedJointRendererComponent() noexcept
		: needUpdate_(true)
	{
	}

	SkinnedJointRendererComponent::SkinnedJointRendererComponent(video::MaterialPtr&& material) noexcept
		: needUpdate_(true)
	{
		this->setMaterial(std::move(material));
	}

	SkinnedJointRendererComponent::SkinnedJointRendererComponent(const video::MaterialPtr& material) noexcept
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
		instance->setMaterial(this->getMaterial() ? (this->isSharedMaterial() ? this->getMaterial() : this->getMaterial()->clone()) : nullptr, this->isSharedMaterial());

		return instance;
	}

	void
	SkinnedJointRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addComponentDispatch(GameDispatchType::FixedUpdate);

		this->addMessageListener("octoon:mesh:update", std::bind(&SkinnedJointRendererComponent::onJointReplace, this, std::placeholders::_1));
		this->addMessageListener("octoon:animation:update", std::bind(&SkinnedJointRendererComponent::onAnimationUpdate, this, std::placeholders::_1));
		
		auto transform = this->getComponent<TransformComponent>();

		geometry_ = std::make_shared<video::Geometry>();
		geometry_->setActive(true);
		geometry_->setOwnerListener(this);
		geometry_->setMaterial(this->getMaterial());
		geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
		geometry_->setLayer(this->getGameObject()->getLayer());

		this->sendMessage("octoon:mesh:get");
	}

	void
	SkinnedJointRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);

		this->removeMessageListener("octoon:mesh:update", std::bind(&SkinnedJointRendererComponent::onJointReplace, this, std::placeholders::_1));
		this->removeMessageListener("octoon:animation:update", std::bind(&SkinnedJointRendererComponent::onAnimationUpdate, this, std::placeholders::_1));
		
		mesh_.reset();

		if (geometry_)
		{
			geometry_->setActive(false);
			geometry_ = nullptr;
		}
	}

	void
	SkinnedJointRendererComponent::onMoveBefore() noexcept
	{
	}

	void
	SkinnedJointRendererComponent::onMoveAfter() noexcept
	{
		if (geometry_)
		{
			auto transform = this->getComponent<TransformComponent>();
			geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	SkinnedJointRendererComponent::onFixedUpdate() noexcept
	{
		this->needUpdate_ = true;
	}

	void
	SkinnedJointRendererComponent::onJointReplace(const runtime::any& data_) noexcept
	{
		if (!this->getMaterial())
			return;

		auto mesh = runtime::any_cast<model::MeshPtr>(data_);
		if (mesh)
		{
			mesh_ = mesh;
			skinnedMesh_ = mesh->clone();
		}

		needUpdate_ = true;
	}

	void
	SkinnedJointRendererComponent::onAnimationUpdate(const runtime::any& data) noexcept
	{
		this->needUpdate_ = true;
	}

	void
	SkinnedJointRendererComponent::onMaterialReplace(const video::MaterialPtr& material) noexcept
	{
		if (geometry_)
			geometry_->setMaterial(material);
	}

	void
	SkinnedJointRendererComponent::onLayerChangeAfter() noexcept
	{
		if (geometry_)
			geometry_->setLayer(this->getGameObject()->getLayer());
	}

	void
	SkinnedJointRendererComponent::onPreRender(const video::Camera& camera) noexcept
	{
		if (!mesh_)
			return;

		if (needUpdate_)
		{
			model::Mesh meshes;
			auto& vertices = meshes.getVertexArray();

			for (auto& transfrom : transforms_)
			{
				if (transfrom->getParent())
				{
					vertices.push_back(transfrom->getParent()->getComponent<TransformComponent>()->getTranslate());
					vertices.push_back(transfrom->getComponent<TransformComponent>()->getTranslate());
				}
			}

			if (!vertices.empty())
				this->uploadJointData(meshes);

			needUpdate_ = false;
		}
	}

	void
	SkinnedJointRendererComponent::onPostRender(const video::Camera& camera) noexcept
	{
	}

	void
	SkinnedJointRendererComponent::uploadJointData(const model::Mesh& mesh) noexcept
	{
		if (geometry_)
		{
			auto& vertices = mesh.getVertexArray();
			auto& texcoord = mesh.getTexcoordArray();
			auto& normals = mesh.getNormalArray();

			auto inputLayout = this->getMaterial()->getPipeline()->getPipelineDesc().getInputLayout()->getInputLayoutDesc();
			auto vertexSize = inputLayout.getVertexSize() / sizeof(float);

			std::uint32_t offset = 0;
			std::vector<float> data(vertices.size() * vertexSize);

			for (auto& layout : inputLayout.getVertexLayouts())
			{
				if (layout.getSemantic() == "POSITION")
				{
					auto v = data.data() + offset;
					for (auto& it : vertices)
					{
						v[0] = it.x;
						v[1] = it.y;
						v[2] = it.z;
						v += vertexSize;
					}
				}
				else if (layout.getSemantic() == "TEXCOORD")
				{
					auto t = data.data() + offset;
					for (auto& it : texcoord)
					{
						t[0] = it.x;
						t[1] = it.y;
						t += vertexSize;
					}
				}
				else if (layout.getSemantic() == "NORMAL")
				{
					auto n = data.data() + offset;
					for (auto& it : normals)
					{
						n[0] = it.x;
						n[1] = it.y;
						n[2] = it.z;
						n += vertexSize;
					}
				}

				offset += layout.getVertexCount();
			}

			hal::GraphicsDataDesc dataDesc;
			dataDesc.setType(hal::GraphicsDataType::StorageVertexBuffer);
			dataDesc.setStream((std::uint8_t*)data.data());
			dataDesc.setStreamSize(data.size() * sizeof(float));
			dataDesc.setUsage(hal::GraphicsUsageFlagBits::ReadBit);

			geometry_->setVertexBuffer(video::RenderSystem::instance()->createGraphicsData(dataDesc));
			geometry_->setNumVertices((std::uint32_t)vertices.size());
			geometry_->setBoundingBox(mesh.getBoundingBox());

			auto& indices = mesh.getIndicesArray();
			if (!indices.empty())
			{
				hal::GraphicsDataDesc indiceDesc;
				indiceDesc.setType(hal::GraphicsDataType::StorageIndexBuffer);
				indiceDesc.setStream((std::uint8_t*)indices.data());
				indiceDesc.setStreamSize(indices.size() * sizeof(std::uint32_t));
				indiceDesc.setUsage(hal::GraphicsUsageFlagBits::ReadBit);

				geometry_->setIndexBuffer(video::RenderSystem::instance()->createGraphicsData(indiceDesc));
				geometry_->setNumIndices((std::uint32_t)indices.size());
			}
		}
	}
}