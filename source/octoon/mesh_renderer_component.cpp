#include <octoon/mesh_renderer_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/video/render_pipeline.h>
#include <octoon/video/render_system.h>
#include <octoon/video/basic_material.h>
#include <octoon/texture_loader.h>

namespace octoon
{
	OctoonImplementSubClass(MeshRendererComponent, RenderComponent, "MeshRenderer")

	MeshRendererComponent::MeshRendererComponent() noexcept
	{
	}

	MeshRendererComponent::MeshRendererComponent(material::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	MeshRendererComponent::MeshRendererComponent(const material::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	MeshRendererComponent::MeshRendererComponent(material::Materials&& materials) noexcept
	{
		this->setMaterials(std::move(materials));
	}

	MeshRendererComponent::MeshRendererComponent(const material::Materials& materials) noexcept
	{
		this->setMaterials(materials);
	}

	MeshRendererComponent::~MeshRendererComponent() noexcept
	{
	}

	GameComponentPtr
	MeshRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<MeshRendererComponent>();
		instance->setName(this->getName());

		if (!this->getMaterials().empty())
		{
			if (this->isSharedMaterial())
				instance->setMaterials(this->getMaterials());
			else
			{
				material::Materials materials;
				for (auto& it : this->getMaterials())
					materials.push_back(it->clone());
				instance->setMaterials(std::move(materials));
			}
		}

		return instance;
	}

	void
	MeshRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addMessageListener("octoon:mesh:update", std::bind(&MeshRendererComponent::onMeshReplace, this, std::placeholders::_1));
		
		this->onMaterialReplace(this->getMaterials());
		this->onMoveAfter();
		this->onLayerChangeAfter();
	}

	void
	MeshRendererComponent::onDeactivate() noexcept
	{
		pipelines_.clear();
		geometries_.clear();
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeMessageListener("octoon:mesh:update", std::bind(&MeshRendererComponent::onMeshReplace, this, std::placeholders::_1));
	}

	void
	MeshRendererComponent::onMeshReplace(const std::any& mesh_) noexcept
	{
		assert(mesh_.type() == typeid(mesh::MeshPtr));
		auto mesh = std::any_cast<mesh::MeshPtr>(mesh_);
		if (mesh)
			this->uploadMeshData(mesh);
	}

	void
	MeshRendererComponent::onMaterialReplace(const material::Materials& material) noexcept
	{
		this->uploadMaterialData(material);
	}

	void
	MeshRendererComponent::onMoveAfter() noexcept
	{
		auto transform = this->getComponent<TransformComponent>();
		for (auto& it : geometries_)
			it->setTransform(transform->getTransform(), transform->getTransformInverse());
	}

	void
	MeshRendererComponent::onLayerChangeAfter() noexcept
	{
		auto layer = this->getGameObject()->getLayer();
		for (auto& it : geometries_)
			it->setLayer(layer);
	}

	void
	MeshRendererComponent::uploadMeshData(const mesh::MeshPtr& mesh) noexcept
	{
		geometries_.clear();

		if (mesh)
		{
			auto& vertices = mesh->getVertexArray();
			auto& texcoord = mesh->getTexcoordArray();
			auto& normals = mesh->getNormalArray();

			hal::GraphicsInputLayoutDesc inputLayout;
			inputLayout.addVertexLayout(hal::GraphicsVertexLayout(0, "POSITION", 0, hal::GraphicsFormat::R32G32B32SFloat));
			inputLayout.addVertexLayout(hal::GraphicsVertexLayout(0, "TEXCOORD", 0, hal::GraphicsFormat::R32G32SFloat));
			inputLayout.addVertexBinding(hal::GraphicsVertexBinding(0, inputLayout.getVertexSize()));

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

			auto vbo = video::RenderSystem::instance()->createGraphicsData(dataDesc);

			for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
			{
				auto geometry_ = std::make_shared<video::Geometry>();
				geometry_->setActive(true);
				geometry_->setOwnerListener(this);
				geometry_->setVertexBuffer(vbo);
				geometry_->setNumVertices((std::uint32_t)vertices.size());
				geometry_->setBoundingBox(mesh->getBoundingBox(i));

				auto& indices = mesh->getIndicesArray(i);
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

				geometries_.push_back(geometry_);
			}

			this->onMoveAfter();
			this->onLayerChangeAfter();

			for (std::size_t i = 0; i < geometries_.size(); i++)
			{
				if (i < pipelines_.size())
					geometries_[i]->setRenderPipeline(pipelines_[i]);
				else
					geometries_[i]->setRenderPipeline(pipelines_.front());
			}
		}
	}

	void
	MeshRendererComponent::uploadMaterialData(const material::Materials& materials) noexcept
	{
		pipelines_.clear();

		for (auto& mat : materials)
		{
			std::string name;
			std::string path;
			std::string textureName;

			math::float3 base = math::float3(1.0f, 0.0f, 1.0f);
			math::float3 ambient;

			mat->get(MATKEY_NAME, name);
			mat->get(MATKEY_PATH, path);
			mat->get(MATKEY_TEXTURE_DIFFUSE, textureName);
			mat->get(MATKEY_COLOR_DIFFUSE, base);
			mat->get(MATKEY_COLOR_AMBIENT, ambient);

			auto pipeline = std::make_shared<video::BasicPipeline>();
			pipeline->setMaterial(mat);
			pipeline->setBaseColor(math::float4(base, 1.0));

			if (!textureName.empty())
				pipeline->setTexture(TextureLoader::load(path + textureName));

			pipelines_.push_back(pipeline);
		}

		for (std::size_t i = 0; i < geometries_.size(); i++)
		{
			if (i < pipelines_.size())
				geometries_[i]->setRenderPipeline(pipelines_[i]);
			else
				geometries_[i]->setRenderPipeline(pipelines_.front());
		}
	}
}