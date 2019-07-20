#include <octoon/offline_mesh_renderer_component.h>
#include <octoon/offline_feature.h>
#include <octoon/game_scene.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/video/render_system.h>

#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubClass(OfflineMeshRendererComponent, OfflineRenderComponent, "OfflineMeshRenderer")

	OfflineMeshRendererComponent::OfflineMeshRendererComponent() noexcept
		: rprShape_(nullptr)
		, rprMaterial_(nullptr)
	{
	}

	OfflineMeshRendererComponent::OfflineMeshRendererComponent(video::MaterialPtr&& material) noexcept
		: rprShape_(nullptr)
		, rprMaterial_(nullptr)
	{
		this->setMaterial(std::move(material));
	}

	OfflineMeshRendererComponent::OfflineMeshRendererComponent(const video::MaterialPtr& material) noexcept
		: rprShape_(nullptr)
		, rprMaterial_(nullptr)
	{
		this->setMaterial(material);
	}

	OfflineMeshRendererComponent::~OfflineMeshRendererComponent() noexcept
	{
	}

	GameComponentPtr
	OfflineMeshRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<OfflineMeshRendererComponent>();
		instance->setName(this->getName());
		instance->setMaterial(this->getMaterial() ? (this->isSharedMaterial() ? this->getMaterial() : this->getMaterial()->clone()) : nullptr, this->isSharedMaterial());

		return instance;
	}

	void
	OfflineMeshRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addMessageListener("octoon:mesh:update", std::bind(&OfflineMeshRendererComponent::onMeshReplace, this, std::placeholders::_1));
		this->sendMessage("octoon:mesh:get", nullptr);		
	}

	void
	OfflineMeshRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeMessageListener("octoon:mesh:update", std::bind(&OfflineMeshRendererComponent::onMeshReplace, this, std::placeholders::_1));

		auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
		if (feature && this->rprShape_)
		{
			rprSceneDetachShape(feature->getScene(), this->rprShape_);
			rprObjectDelete(this->rprShape_);
			this->rprShape_ = nullptr;
		}
	}

	void
	OfflineMeshRendererComponent::onMoveBefore() noexcept
	{
	}

	void
	OfflineMeshRendererComponent::onMoveAfter() noexcept
	{
		if (this->rprShape_)
		{
			auto transform = this->getComponent<TransformComponent>();
			rprShapeSetTransform(this->rprShape_, false, transform->getTransform().ptr());
		}
	}

	void
	OfflineMeshRendererComponent::onMeshReplace(const runtime::any& mesh_) noexcept
	{
		auto mesh = runtime::any_cast<model::MeshPtr>(mesh_);
		if (mesh)
			this->uploadMeshData(*mesh);
	}

	void
	OfflineMeshRendererComponent::onMaterialReplace(const video::MaterialPtr& material) noexcept
	{
	}

	void
	OfflineMeshRendererComponent::onLayerChangeAfter() noexcept
	{
		if (this->rprShape_)
			rprShapeSetLayerMask(this->rprShape_, this->getGameObject()->getLayer());
	}

	void
	OfflineMeshRendererComponent::uploadMeshData(const model::Mesh& mesh) noexcept
	{
		auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
		if (feature)
		{
			if (!this->rprMaterial_)
			{
				rprMaterialSystemCreateNode(feature->getMaterialSystem(), RPR_MATERIAL_NODE_UBERV2, &rprMaterial_);
				rprMaterialNodeSetInputF(rprMaterial_, "uberv2.diffuse.color", 1.0, 1.0, 1.0, 1.0);
			}

			math::uint1s faceArray;
			for (std::size_t i = 0; i < mesh.getVertexArray().size() / 3; i++)
				faceArray.push_back(3);

			rprContextCreateMesh(feature->getContext(), 
				mesh.getVertexArray().data()->ptr(), mesh.getVertexArray().size() / 3, sizeof(math::float3),
				mesh.getNormalArray().data()->ptr(), mesh.getNormalArray().size() / 3, sizeof(math::float3),
				mesh.getTexcoordArray().data()->ptr(), mesh.getTexcoordArray().size() / 2, sizeof(math::float2),
				(rpr_int*)mesh.getIndicesArray().data(), sizeof(rpr_int),
				(rpr_int*)mesh.getIndicesArray().data(), sizeof(rpr_int),
				(rpr_int*)mesh.getIndicesArray().data(), sizeof(rpr_int),
				(rpr_int*)faceArray.data(), faceArray.size(),
				&this->rprShape_);

			rprObjectSetName(this->rprShape_, this->getGameObject()->getName().c_str());

			rprShapeSetShadow(this->rprShape_, true);
			rprShapeSetShadowCatcher(this->rprShape_, true);
			rprShapeSetVisibility(this->rprShape_, true);
			rprShapeSetLayerMask(this->rprShape_, this->getGameObject()->getLayer());
			rprShapeSetMaterial(this->rprShape_, this->rprMaterial_);

			auto transform = this->getComponent<TransformComponent>();
			rprShapeSetTransform(this->rprShape_, false, transform->getTransform().ptr());

			rprSceneAttachShape(feature->getScene(), this->rprShape_);
		}
	}
}