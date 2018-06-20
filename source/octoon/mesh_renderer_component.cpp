#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/video/render_system.h>

namespace octoon
{
	OctoonImplementSubClass(MeshRendererComponent, RenderComponent, "MeshRenderer")

	MeshRendererComponent::MeshRendererComponent() noexcept
	{
	}

	MeshRendererComponent::MeshRendererComponent(video::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	MeshRendererComponent::MeshRendererComponent(const video::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	MeshRendererComponent::~MeshRendererComponent() noexcept
	{
	}

	GameComponentPtr
	MeshRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<MeshRendererComponent>();
		instance->setName(this->getName());
		if (this->isSharedMaterial())
			instance->setMaterial(this->getMaterial(), this->isSharedMaterial());
		else
			instance->setMaterial(this->getMaterial() ? this->getMaterial()->clone() : nullptr, this->isSharedMaterial());

		return instance;
	}

	void
	MeshRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);

		auto transform = this->getComponent<TransformComponent>();
		auto meshFilter = this->getComponent<MeshFilterComponent>();

		geometry_ = std::make_shared<video::Geometry>();
		geometry_->setActive(true);
		geometry_->setMaterial(this->getMaterial());
		geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());

		if (meshFilter)
			this->onMeshReplace(meshFilter->getMesh());
	}

	void
	MeshRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);

		if (geometry_)
		{
			geometry_->setActive(false);
			geometry_ = nullptr;
		}
	}

	void
	MeshRendererComponent::onAttachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<MeshFilterComponent>())
		{
			onMeshReplaceEvent_ = std::bind(&MeshRendererComponent::onMeshReplace, this, std::placeholders::_1);
			component->downcast<MeshFilterComponent>()->addMeshListener(&onMeshReplaceEvent_);
		}
	}

	void
	MeshRendererComponent::onDetachComponent(const GameComponentPtr& component) noexcept
	{
		if (component->isA<MeshFilterComponent>())
			component->downcast<MeshFilterComponent>()->removeMeshListener(&onMeshReplaceEvent_);
	}

	void
	MeshRendererComponent::onMoveBefore() noexcept
	{
	}

	void
	MeshRendererComponent::onMoveAfter() noexcept
	{
		if (geometry_)
		{
			auto transform = this->getComponent<TransformComponent>();
			geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	MeshRendererComponent::onMeshReplace(const model::MeshPtr& mesh) noexcept
	{
		if (!this->getMaterial())
			return;

		if (geometry_ && mesh)
		{
			auto& vertices = mesh->getVertexArray();
			auto& texcoord = mesh->getTexcoordArray();
			auto& normals = mesh->getNormalArray();

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

			graphics::GraphicsDataDesc dataDesc;
			dataDesc.setType(graphics::GraphicsDataType::StorageVertexBuffer);
			dataDesc.setStream((std::uint8_t*)data.data());
			dataDesc.setStreamSize(data.size() * sizeof(float));
			dataDesc.setUsage(graphics::GraphicsUsageFlagBits::ReadBit);

			geometry_->setVertexBuffer(video::RenderSystem::instance()->createGraphicsData(dataDesc));
			geometry_->setNumVertices((std::uint32_t)vertices.size());

			auto& indices = mesh->getIndicesArray();
			if (!indices.empty())
			{
				graphics::GraphicsDataDesc indiceDesc;
				indiceDesc.setType(graphics::GraphicsDataType::StorageIndexBuffer);
				indiceDesc.setStream((std::uint8_t*)indices.data());
				indiceDesc.setStreamSize(indices.size() * sizeof(std::uint32_t));
				indiceDesc.setUsage(graphics::GraphicsUsageFlagBits::ReadBit);

				geometry_->setIndexBuffer(video::RenderSystem::instance()->createGraphicsData(indiceDesc));
				geometry_->setNumIndices((std::uint32_t)indices.size());
			}
		}
	}

	void
	MeshRendererComponent::onMaterialReplace(const video::MaterialPtr& material) noexcept
	{
		if (geometry_)
			geometry_->setMaterial(material);
	}
}