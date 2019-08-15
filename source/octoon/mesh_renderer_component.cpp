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
		instance->setMaterial(this->getMaterial() ? (this->isSharedMaterial() ? this->getMaterial() : this->getMaterial()->clone()) : nullptr, this->isSharedMaterial());

		return instance;
	}

	void
	MeshRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addMessageListener("octoon:mesh:update", std::bind(&MeshRendererComponent::onMeshReplace, this, std::placeholders::_1));
		
		auto transform = this->getComponent<TransformComponent>();

		geometry_ = std::make_shared<video::Geometry>();
		geometry_->setActive(true);
		geometry_->setOwnerListener(this);
		geometry_->setMaterial(this->getMaterial());
		geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
		geometry_->setLayer(this->getGameObject()->getLayer());

		this->sendMessage("octoon:mesh:get", nullptr);
	}

	void
	MeshRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeMessageListener("octoon:mesh:update", std::bind(&MeshRendererComponent::onMeshReplace, this, std::placeholders::_1));

		if (geometry_)
		{
			geometry_->setActive(false);
			geometry_ = nullptr;
		}
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
	MeshRendererComponent::onMeshReplace(const runtime::any& mesh_) noexcept
	{
		auto mesh = runtime::any_cast<model::MeshPtr>(mesh_);
		if (mesh)
			this->uploadMeshData(*mesh);
	}

	void
	MeshRendererComponent::onMaterialReplace(const video::MaterialPtr& material) noexcept
	{
		if (geometry_)
			geometry_->setMaterial(material);
	}

	void
	MeshRendererComponent::onLayerChangeAfter() noexcept
	{
		if (geometry_)
			geometry_->setLayer(this->getGameObject()->getLayer());
	}

	void
	MeshRendererComponent::uploadMeshData(const model::Mesh& mesh) noexcept
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