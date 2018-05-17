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
		if (geometry_ && mesh)
		{
			auto& vertices = mesh->getVertexArray();
			auto& normals = mesh->getNormalArray();

			graphics::GraphicsDataDesc dataDesc;
			dataDesc.setType(graphics::GraphicsDataType::StorageVertexBuffer);
			dataDesc.setStream(0);
			dataDesc.setStreamSize(vertices.size() * sizeof(math::float3) * 2);
			dataDesc.setUsage(graphics::GraphicsUsageFlagBits::WriteBit);

			auto vertices_ = video::RenderSystem::instance()->createGraphicsData(dataDesc);

			math::float3* data = nullptr;
			if (vertices_->map(0, vertices.size() * sizeof(math::float3) * 2, (void**)&data))
			{
				auto v = data;
				for (auto& it : vertices)
				{
					*v = it;
					v += 2;
				}

				auto n = ++data;
				for (auto& it : normals)
				{
					*n = it;
					n += 2;
				}

				vertices_->unmap();
			}

			geometry_->setVertexBuffer(vertices_);
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