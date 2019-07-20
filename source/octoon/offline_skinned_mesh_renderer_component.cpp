#include <octoon/offline_skinned_mesh_renderer_component.h>
#include <octoon/offline_feature.h>
#include <octoon/game_scene.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/video/render_system.h>

#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubClass(OfflineSkinnedMeshRendererComponent, OfflineRenderComponent, "OfflineMeshRenderer")

	OfflineSkinnedMeshRendererComponent::OfflineSkinnedMeshRendererComponent() noexcept
		: rprShape_(nullptr)
		, rprMaterial_(nullptr)
		, needUpdate_(true)
	{
	}

	OfflineSkinnedMeshRendererComponent::OfflineSkinnedMeshRendererComponent(video::MaterialPtr&& material) noexcept
		: OfflineSkinnedMeshRendererComponent()
	{
		this->setMaterial(std::move(material));
	}

	OfflineSkinnedMeshRendererComponent::OfflineSkinnedMeshRendererComponent(const video::MaterialPtr& material) noexcept
		: OfflineSkinnedMeshRendererComponent()
	{
		this->setMaterial(material);
	}

	OfflineSkinnedMeshRendererComponent::~OfflineSkinnedMeshRendererComponent() noexcept
	{
	}

	void
	OfflineSkinnedMeshRendererComponent::setTransforms(const GameObjects& transforms) noexcept
	{
		transforms_ = transforms;
	}

	void
	OfflineSkinnedMeshRendererComponent::setTransforms(GameObjects&& transforms) noexcept
	{
		transforms_ = std::move(transforms);
	}

	const GameObjects&
	OfflineSkinnedMeshRendererComponent::getTransforms() const noexcept
	{
		return transforms_;
	}

	GameComponentPtr
	OfflineSkinnedMeshRendererComponent::clone() const noexcept
	{
		auto instance = std::make_shared<OfflineSkinnedMeshRendererComponent>();
		instance->setName(this->getName());
		instance->setMaterial(this->getMaterial() ? (this->isSharedMaterial() ? this->getMaterial() : this->getMaterial()->clone()) : nullptr, this->isSharedMaterial());

		return instance;
	}

	void
	OfflineSkinnedMeshRendererComponent::onActivate() noexcept
	{
		this->addComponentDispatch(GameDispatchType::FrameEnd);
		this->addComponentDispatch(GameDispatchType::MoveAfter);
		this->addMessageListener("octoon:mesh:update", std::bind(&OfflineSkinnedMeshRendererComponent::onMeshReplace, this, std::placeholders::_1));
		this->addMessageListener("octoon:animation:update", std::bind(&OfflineSkinnedMeshRendererComponent::onAnimationUpdate, this, std::placeholders::_1));

		this->sendMessage("octoon:mesh:get", nullptr);		
	}

	void
	OfflineSkinnedMeshRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeMessageListener("octoon:mesh:update", std::bind(&OfflineSkinnedMeshRendererComponent::onMeshReplace, this, std::placeholders::_1));
		this->removeMessageListener("octoon:animation:update", std::bind(&OfflineSkinnedMeshRendererComponent::onAnimationUpdate, this, std::placeholders::_1));

		auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
		if (feature && this->rprShape_)
			rprSceneDetachShape(feature->getScene(), this->rprShape_);

		if (this->rprShape_)
		{
			rprObjectDelete(this->rprShape_);
			this->rprShape_ = nullptr;
		}

		if (this->rprMaterial_)
		{
			rprObjectDelete(this->rprMaterial_);
			this->rprMaterial_ = nullptr;
		}
	}

	void
	OfflineSkinnedMeshRendererComponent::onFrameEnd() noexcept
	{
		if (!mesh_)
			return;

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

#pragma omp parallel for
			for (std::int32_t i = 0; i < (std::int32_t)vertices.size(); i++)
			{
				math::float3 v = math::float3::Zero;
				math::float3 n = math::float3::Zero;

				for (std::uint8_t j = 0; j < 4; j++)
				{
					auto w = weights[i].weights[j];
					if (w > 0.0)
					{
						v += (joints[weights[i].bones[j]] * vertices[i]) * w;
						n += ((math::float3x3)joints[weights[i].bones[j]] * normals[i]) * w;
					}
				}

				skinnedMesh_->getVertexArray()[i] = v;
				skinnedMesh_->getNormalArray()[i] = n;
			}

			this->uploadMeshData(*skinnedMesh_);

			auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
			if (feature)
				feature->setFramebufferDirty(true);

			needUpdate_ = false;
		}
	}

	void
	OfflineSkinnedMeshRendererComponent::onMoveBefore() noexcept
	{
	}

	void
	OfflineSkinnedMeshRendererComponent::onMoveAfter() noexcept
	{
		if (this->rprShape_)
		{
			auto transform = this->getComponent<TransformComponent>();
			rprShapeSetTransform(this->rprShape_, false, transform->getTransform().ptr());

			auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
			if (feature)
				feature->setFramebufferDirty(true);
		}
	}

	void
	OfflineSkinnedMeshRendererComponent::onMeshReplace(const runtime::any& data) noexcept
	{
		if (!this->getMaterial())
			return;

		auto mesh = runtime::any_cast<model::MeshPtr>(data);
		if (mesh)
		{
			mesh_ = mesh;
			skinnedMesh_ = mesh->clone();
		}

		needUpdate_ = true;
	}

	void
	OfflineSkinnedMeshRendererComponent::onMaterialReplace(const video::MaterialPtr& material) noexcept
	{
	}

	void
	OfflineSkinnedMeshRendererComponent::onAnimationUpdate(const runtime::any& data) noexcept
	{
		this->needUpdate_ = true;
	}

	void
	OfflineSkinnedMeshRendererComponent::onLayerChangeAfter() noexcept
	{
		if (this->rprShape_)
			rprShapeSetLayerMask(this->rprShape_, this->getGameObject()->getLayer());
	}

	void
	OfflineSkinnedMeshRendererComponent::uploadMatData(const video::Material& mat) noexcept
	{
		auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
		if (feature)
		{
			if (!this->rprMaterial_)
			{
				rprMaterialSystemCreateNode(feature->getMaterialSystem(), RPR_MATERIAL_NODE_UBERV2, &rprMaterial_);

				for (auto& it : mat.getDescriptorSet()->getUniformSets())
				{
					if (it->getName() == "color")
					{
						auto color = it->getFloat4();
						rprMaterialNodeSetInputF(rprMaterial_, "uberv2.diffuse.color", color[0], color[1], color[2], color[3]);
					}
					else if (it->getName() == "decal")
					{
						auto texture = it->getTexture();
						if (texture)
						{
							try
							{
								std::uint8_t* data;
								auto desc = texture->getTextureDesc();
								texture->map(0, 0, desc.getWidth(), desc.getHeight(), 0, (void**)& data);

								rpr_image_format imageFormat = { 4, RPR_COMPONENT_TYPE_UINT8 };
								rpr_image_desc imageDesc;
								imageDesc.image_width = desc.getWidth();
								imageDesc.image_height = desc.getHeight();
								imageDesc.image_depth = 1;
								imageDesc.image_row_pitch = desc.getWidth() * imageFormat.num_components;
								imageDesc.image_slice_pitch = imageDesc.image_row_pitch * desc.getHeight();

								std::vector<std::uint8_t> array(desc.getWidth() * desc.getHeight() * imageFormat.num_components);
								for (std::size_t y = 0; y < desc.getHeight(); y++)
								{
									for (std::size_t x = 0; x < desc.getWidth(); x++)
									{
										auto dst = ((desc.getHeight() - 1 - y) * desc.getWidth() + x) * imageFormat.num_components;
										auto src = (y * desc.getWidth() + x) * imageFormat.num_components;

										array[dst] = data[src];
										array[dst + 1] = data[src + 1];
										array[dst + 2] = data[src + 2];
										array[dst + 3] = data[src + 3];
									}
								}

								rpr_image image_;
								rprContextCreateImage(feature->getContext(), imageFormat, &imageDesc, array.data(), &image_);

								rpr_material_node textureNode;
								rprMaterialSystemCreateNode(feature->getMaterialSystem(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &textureNode);
								rprMaterialNodeSetInputImageData(textureNode, "data", image_);

								rprMaterialNodeSetInputN(rprMaterial_, "uberv2.diffuse.color", textureNode);
								rprMaterialNodeSetInputF(rprMaterial_, "uberv2.reflection.color", 1.0f, 0.0f, 1.0f, 1.0f);
								rprMaterialNodeSetInputF(rprMaterial_, "uberv2.refraction.color", 1.0f, 0.0f, 1.0f, 1.0f);
								rprMaterialNodeSetInputF(rprMaterial_, "uberv2.reflection.roughness", 0.0f, 0.0f, 0.0f, 0.0f);
								rprMaterialNodeSetInputF(rprMaterial_, "uberv2.refraction.roughness", 0.0f, 0.0f, 0.0f, 0.0f);

								texture->unmap();
							}
							catch (...)
							{
								texture->unmap();
							}
						}
					}
				}
			}
		}
	}

	void
	OfflineSkinnedMeshRendererComponent::uploadMeshData(const model::Mesh& mesh) noexcept
	{
		auto feature = this->getGameObject()->getGameScene()->getFeature<OfflineFeature>();
		if (feature)
		{
			this->uploadMatData(*this->getMaterial());

			if (this->rprShape_)
			{
				rprSceneDetachShape(feature->getScene(), this->rprShape_);
				rprObjectDelete(this->rprShape_);
				this->rprShape_ = nullptr;
			}			

			math::uint1s faceArray(mesh.getVertexArray().size() / 3, 3);

			rprContextCreateMesh(feature->getContext(), 
				mesh.getVertexArray().data()->ptr(), mesh.getVertexArray().size() / 3, sizeof(math::float3),
				mesh.getNormalArray().data()->ptr(), mesh.getNormalArray().size() / 3, sizeof(math::float3),
				mesh.getTexcoordArray().data()->ptr(), mesh.getTexcoordArray().size() / 2, sizeof(math::float2),
				(rpr_int*)mesh.getIndicesArray().data(), sizeof(rpr_int),
				(rpr_int*)mesh.getIndicesArray().data(), sizeof(rpr_int),
				(rpr_int*)mesh.getIndicesArray().data(), sizeof(rpr_int),
				(rpr_int*)faceArray.data(), faceArray.size(),
				&this->rprShape_);

			rprShapeSetShadow(this->rprShape_, true);
			rprShapeSetShadowCatcher(this->rprShape_, true);
			rprShapeSetVisibility(this->rprShape_, true);
			rprShapeSetObjectGroupID(this->rprShape_, this->getGameObject()->getLayer());
			rprShapeSetMaterial(this->rprShape_, this->rprMaterial_);
			rprShapeSetTransform(this->rprShape_, false, this->getComponent<TransformComponent>()->getTransform().ptr());

			rprSceneAttachShape(feature->getScene(), this->rprShape_);
		}
	}
}