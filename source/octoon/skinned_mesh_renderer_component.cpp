#include <octoon/skinned_mesh_renderer_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/video/render_system.h>

namespace octoon
{
	OctoonImplementSubClass(SkinnedMeshRendererComponent, RenderComponent, "MeshRenderer")

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent() noexcept
		: needUpdate_(true)
	{
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(video::MaterialPtr&& material) noexcept
		: needUpdate_(true)
	{
		this->setMaterial(std::move(material));
	}

	SkinnedMeshRendererComponent::SkinnedMeshRendererComponent(const video::MaterialPtr& material) noexcept
		: needUpdate_(true)
	{
		this->setMaterial(material);
	}

	SkinnedMeshRendererComponent::~SkinnedMeshRendererComponent() noexcept
	{
	}

	void
	SkinnedMeshRendererComponent::setTransforms(const GameObjects& transforms) noexcept
	{
		transforms_ = transforms;
	}

	void
	SkinnedMeshRendererComponent::setTransforms(GameObjects&& transforms) noexcept
	{
		transforms_ = std::move(transforms);
	}

	const GameObjects&
	SkinnedMeshRendererComponent::getTransforms() const noexcept
	{
		return transforms_;
	}

	GameComponentPtr
	SkinnedMeshRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<SkinnedMeshRendererComponent>();
		instance->setName(this->getName());
		instance->setMaterial(this->getMaterial() ? (this->isSharedMaterial() ? this->getMaterial() : this->getMaterial()->clone()) : nullptr, this->isSharedMaterial());

		return instance;
	}

	void
	SkinnedMeshRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addComponentDispatch(GameDispatchType::FixedUpdate);

		this->addMessageListener("octoon:mesh:update", std::bind(&SkinnedMeshRendererComponent::onMeshReplace, this, std::placeholders::_1));
		
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
	SkinnedMeshRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeComponentDispatch(GameDispatchType::FixedUpdate);
		this->removeMessageListener("octoon:mesh:update", std::bind(&SkinnedMeshRendererComponent::onMeshReplace, this, std::placeholders::_1));
		
		mesh_.reset();

		if (geometry_)
		{
			geometry_->setActive(false);
			geometry_ = nullptr;
		}
	}

	void
	SkinnedMeshRendererComponent::onMoveBefore() noexcept
	{
	}

	void
	SkinnedMeshRendererComponent::onMoveAfter() noexcept
	{
		if (geometry_)
		{
			auto transform = this->getComponent<TransformComponent>();
			geometry_->setTransform(transform->getTransform(), transform->getTransformInverse());
		}
	}

	void
	SkinnedMeshRendererComponent::onFixedUpdate() noexcept
	{
		needUpdate_ = true;
	}

	void
	SkinnedMeshRendererComponent::onMeshReplace(const runtime::any& data_) noexcept
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
	SkinnedMeshRendererComponent::onMaterialReplace(const video::MaterialPtr& material) noexcept
	{
		if (geometry_)
			geometry_->setMaterial(material);
	}

	void
	SkinnedMeshRendererComponent::onLayerChangeAfter() noexcept
	{
		if (geometry_)
			geometry_->setLayer(this->getGameObject()->getLayer());
	}

	void
	SkinnedMeshRendererComponent::onPreRender(const video::Camera& camera) noexcept
	{
		if (needUpdate_)
		{
			std::vector<math::float4x4> joints(transforms_.size());

			auto& bindposes = mesh_->getBindposes();
			if (bindposes.size() != transforms_.size())
			{
				for (std::size_t i = 0; i < transforms_.size(); ++i)
					joints[i].makeIdentity();
			}
			else
			{
				for (std::size_t i = 0; i < transforms_.size(); ++i)
					joints[i] = math::transformMultiply(transforms_[i]->getComponent<TransformComponent>()->getTransform(), bindposes[i]);
			}

			auto& vertices = mesh_->getVertexArray();
			auto& normals = mesh_->getNormalArray();
			auto& weights = mesh_->getWeightArray();

#pragma omp parallel for num_threads(4)
			for (std::int32_t i = 0; i < (std::int32_t)vertices.size(); i++)
			{
				auto w1 = weights[i].weights[0];
				auto w2 = weights[i].weights[1];
				auto w3 = weights[i].weights[2];
				auto w4 = weights[i].weights[3];

				if (w1 != 0.0f || w2 != 0.0f || w3 != 0.0f || w4 != 0.0f)
				{
					math::float3 v = math::float3::Zero;
					math::float3 n = math::float3::Zero;

					for (std::uint8_t j = 0; j < 4; j++)
					{
						auto w = weights[i].weights[j];
						if (w == 0.0f)
							break;
						v += (joints[weights[i].bones[j]] * vertices[i]) * w;
						n += ((math::float3x3)joints[weights[i].bones[j]] * normals[i]) * w;
					}

					skinnedMesh_->getVertexArray()[i] = v;
					skinnedMesh_->getNormalArray()[i] = n;
				}
			}

			this->uploadMeshData(*skinnedMesh_);
			needUpdate_ = false;
		}
	}

	void
	SkinnedMeshRendererComponent::onPostRender(const video::Camera& camera) noexcept
	{
	}

	void
	SkinnedMeshRendererComponent::uploadMeshData(const model::Mesh& mesh) noexcept
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