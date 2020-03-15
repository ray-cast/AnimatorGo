#include <octoon/offline_mesh_renderer_component.h>
#include <octoon/offline_feature.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/video/render_system.h>

#include <unordered_map>
#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubClass(OfflineMeshRendererComponent, OfflineRenderComponent, "OfflineMeshRenderer")

	OfflineMeshRendererComponent::OfflineMeshRendererComponent() noexcept
	{
	}

	OfflineMeshRendererComponent::OfflineMeshRendererComponent(material::Materials&& materials) noexcept
	{
		this->setMaterials(std::move(materials));
	}

	OfflineMeshRendererComponent::OfflineMeshRendererComponent(material::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	OfflineMeshRendererComponent::OfflineMeshRendererComponent(const material::Materials& materials) noexcept
	{
		this->setMaterials(materials);
	}

	OfflineMeshRendererComponent::OfflineMeshRendererComponent(const material::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	OfflineMeshRendererComponent::~OfflineMeshRendererComponent() noexcept
	{
	}

	void
	OfflineMeshRendererComponent::uploadMaterialData(const material::Materials& materials) noexcept
	{
		auto feature = this->tryGetFeature<OfflineFeature>();
		if (!feature)
			return;

		for (auto& it : this->materials_)
			rprObjectDelete(it);

		this->materials_.clear();

		std::unordered_map<std::size_t, void*> mtlHash;

		for (auto& mat : materials)
		{
			auto hash = mat->hash();
			auto it = mtlHash.find(hash);
			if (it != mtlHash.end())
			{
				materials_.push_back((*it).second);
				continue;
			}

			std::string path;
			std::string normalName;
			std::string textureName;

			math::float3 base = math::float3(1.0f, 0.0f, 1.0f);
			math::float3 specular = math::float3::One;
			math::float3 ambient = math::float3::Zero;
			math::float4 edgeColor = math::float4::Zero;
			
			float opacity = 1.0f;
			float shininess = 10.0f;
			
			mat->get(MATKEY_PATH, path);
			mat->get(MATKEY_TEXTURE_SPECULAR, normalName);
			mat->get(MATKEY_TEXTURE_DIFFUSE, textureName);
			mat->get(MATKEY_COLOR_DIFFUSE, base);
			mat->get(MATKEY_COLOR_AMBIENT, ambient);			
			mat->get(MATKEY_COLOR_SPECULAR, specular);
			mat->get(MATKEY_COLOR_EDGE, edgeColor);
			mat->get(MATKEY_OPACITY, opacity);
			mat->get(MATKEY_SHININESS, shininess);

			float roughness = std::pow(1.0f - math::saturate(shininess / 1000.0f), 2);

			rpr_material_node rprMaterial;
			rprMaterialSystemCreateNode(feature->getMaterialSystem(), RPR_MATERIAL_NODE_UBERV2, &rprMaterial);

			std::uint32_t layers = RPR_UBER_MATERIAL_LAYER_DIFFUSE;
			if (math::any(specular))
				layers |= RPR_UBER_MATERIAL_LAYER_REFLECTION;
			if (opacity < 1.0f)
				layers |= RPR_UBER_MATERIAL_LAYER_TRANSPARENCY;
			/*if (!normalName.empty())
				layers |= RPR_UBER_MATERIAL_LAYER_SHADING_NORMAL;
			if (edgeColor.x == 0.0f)
				layers |= RPR_UBER_MATERIAL_LAYER_DIFFUSE;*/

			rprMaterialNodeSetInputU(rprMaterial, "uberv2.layers", layers);

			if (layers & RPR_UBER_MATERIAL_LAYER_TRANSPARENCY)
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.transparency", 1.0f - opacity, 1.0f, 1.0f, 1.0f);

			if (layers & RPR_UBER_MATERIAL_LAYER_DIFFUSE)
			{
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.diffuse.roughness", roughness, roughness, roughness, roughness);
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.diffuse.subsurface", edgeColor.z, edgeColor.z, edgeColor.z, edgeColor.z);
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.diffuse.color", base[0], base[1], base[2], 1.0f);
			}

			if (layers & RPR_UBER_MATERIAL_LAYER_REFLECTION)
			{
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.ior", 1.5f, 1.5f, 1.5f, 1.5f);
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.roughness", roughness, roughness, roughness, roughness);
				//rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.anisotropy", edgeColor.w, edgeColor.w, edgeColor.w, edgeColor.w);
				//rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.sheen", edgeColor.y, edgeColor.y, edgeColor.y, edgeColor.y);
				//rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.color", base.x * specular.x, base.y * specular.y, base.z * specular.z, 1.0f);
				//rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.metalness", edgeColor.x, edgeColor.x, edgeColor.x, edgeColor.x);
			}

			if (layers & RPR_UBER_MATERIAL_LAYER_REFRACTION)
			{
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.refraction.ior", 1.5f, 1.5f, 1.5f, 1.5f);
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.refraction.roughness", 1.0f, 1.0f, 1.0f, 1.0f);
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.refraction.color", 1.0f, 1.0f, 1.0f, 1.0f);
			}

			/*if (layers & RPR_UBER_MATERIAL_LAYER_SHADING_NORMAL)
			{
				try
				{
					rpr_material_node textureNode;
					rprMaterialSystemCreateNode(feature->getMaterialSystem(), RPR_MATERIAL_NODE_NORMAL_MAP, &textureNode);
					rprMaterialNodeSetInputImageData(textureNode, "data", feature->createMaterialTextures(path + normalName).first);
					rprMaterialNodeSetInputN(rprMaterial, "uberv2.normal", textureNode);
				}
				catch (...)
				{
					rprMaterialNodeSetInputU(rprMaterial, "uberv2.layers", layers & ~RPR_UBER_MATERIAL_LAYER_SHADING_NORMAL);
				}
			}*/

			if (!textureName.empty())
			{
				try
				{
					auto image = feature->createMaterialTextures(path + textureName);
					if (image.first)
					{
						if (layers & RPR_UBER_MATERIAL_LAYER_DIFFUSE)
							rprMaterialNodeSetInputN(rprMaterial, "uberv2.diffuse.color", image.first);

						if (layers & RPR_UBER_MATERIAL_LAYER_REFLECTION && edgeColor.x > 0.0f)
							rprMaterialNodeSetInputN(rprMaterial, "uberv2.reflection.color", image.first);

						if (layers & RPR_UBER_MATERIAL_LAYER_REFRACTION)
							rprMaterialNodeSetInputN(rprMaterial, "uberv2.refraction.color", image.first);
					}
					else
					{
						rprMaterialNodeSetInputF(rprMaterial, "uberv2.diffuse.color", 1.0f, 0.0f, 1.0f, 1.0f);
					}

					if (image.second)
					{
						rprMaterialNodeSetInputU(rprMaterial, "uberv2.layers", layers | RPR_UBER_MATERIAL_LAYER_TRANSPARENCY);
						rprMaterialNodeSetInputN(rprMaterial, "uberv2.transparency", image.second);
					}
				}
				catch (...)
				{
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.diffuse.color", 1.0f, 0.0f, 1.0f, 1.0f);
				}
			}

			mtlHash[hash] = rprMaterial;
			materials_.push_back(rprMaterial);
		}

		for (std::size_t i = 0; i < shapes_.size(); i++)
		{
			if (i < materials_.size())
				rprShapeSetMaterial(shapes_[i], materials_[i]);
			else
				rprShapeSetMaterial(shapes_[i], materials_.front());
		}
	}

	void
	OfflineMeshRendererComponent::uploadMeshData(const mesh::MeshPtr& mesh) noexcept
	{
		auto feature = this->tryGetFeature<OfflineFeature>();
		if (feature)
		{
			for (auto& it : shapes_)
			{
				rprSceneDetachShape(feature->getScene(), it);
				rprObjectDelete(it);
			}

			shapes_.clear();

			if (mesh)
			{
				for (std::size_t i = 0; i < mesh->getNumSubsets(); i++)
				{
					math::uint1s faceArray(mesh->getIndicesArray(i).size() / 3, 3);

					rpr_shape rprShape;
					rprContextCreateMesh(feature->getContext(),
						mesh->getVertexArray().data()->ptr(), mesh->getVertexArray().size() / 3, sizeof(math::float3),
						mesh->getNormalArray().data()->ptr(), mesh->getNormalArray().size() / 3, sizeof(math::float3),
						mesh->getTexcoordArray().data()->ptr(), mesh->getTexcoordArray().size() / 2, sizeof(math::float2),
						(rpr_int*)mesh->getIndicesArray(i).data(), sizeof(rpr_int),
						(rpr_int*)mesh->getIndicesArray(i).data(), sizeof(rpr_int),
						(rpr_int*)mesh->getIndicesArray(i).data(), sizeof(rpr_int),
						(rpr_int*)faceArray.data(), faceArray.size(),
						&rprShape);

					rprShapeSetShadow(rprShape, true);
					rprShapeSetShadowCatcher(rprShape, true);
					rprShapeSetVisibility(rprShape, true);
					rprShapeSetLayerMask(rprShape, this->getGameObject()->getLayer());
					rprShapeSetObjectGroupID(rprShape, (rpr_uint)this->getGameObject()->id());
					rprShapeSetTransform(rprShape, false, this->getComponent<TransformComponent>()->getTransform().ptr());
					rprSceneAttachShape(feature->getScene(), rprShape);

					shapes_.push_back(rprShape);
				}

				for (std::size_t i = 0; i < materials_.size(); i++)
				{
					if (i < shapes_.size())
						rprShapeSetMaterial(shapes_[i], materials_[i]);
					else
						rprShapeSetMaterial(shapes_[i], materials_.front());
				}
			}
		}
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

		auto feature = this->getFeature<OfflineFeature>();
		if (feature)
		{
			feature->addOfflineListener(this);
			feature->setFramebufferDirty(true);
		}

		this->onMaterialReplace(this->getMaterials());
	}

	void
	OfflineMeshRendererComponent::onDeactivate() noexcept
	{
		this->removeComponentDispatch(GameDispatchType::MoveAfter);
		this->removeMessageListener("octoon:mesh:update", std::bind(&OfflineMeshRendererComponent::onMeshReplace, this, std::placeholders::_1));

		auto feature = this->getFeature<OfflineFeature>();
		if (feature)
		{
			feature->setFramebufferDirty(true);
			feature->removeOfflineListener(this);
			for (auto& shape : shapes_)
				rprSceneDetachShape(feature->getScene(), shape);
		}

		for (auto& shape : shapes_)
			rprObjectDelete(shape);
		shapes_.clear();

		std::vector<void*> cache;

		for (auto& material : materials_)
		{
			if (cache.end() == std::find(cache.begin(), cache.end(), material))
			{
				rprObjectDelete(material);
				cache.push_back(material);
			}
		}

		materials_.clear();
	}

	void
	OfflineMeshRendererComponent::onMoveAfter() noexcept
	{
		for (auto& shape : shapes_)
		{
			auto transform = this->getComponent<TransformComponent>();
			rprShapeSetTransform(shape, false, transform->getTransform().ptr());

			auto feature = this->getFeature<OfflineFeature>();
			if (feature)
				feature->setFramebufferDirty(true);
		}
	}

	void
	OfflineMeshRendererComponent::onLayerChangeAfter() noexcept
	{
		for (auto& shape : shapes_)
			rprShapeSetObjectGroupID(shape, this->getGameObject()->getLayer());
		auto feature = this->getFeature<OfflineFeature>();
		if (feature)
			feature->setFramebufferDirty(true);
	}

	void
	OfflineMeshRendererComponent::onMeshReplace(const std::any& mesh_) noexcept
	{
		this->uploadMeshData(std::any_cast<mesh::MeshPtr>(mesh_));
	}

	void
	OfflineMeshRendererComponent::onMaterialReplace(const material::Materials& material) noexcept
	{
		if (this->getGameScene())
			this->uploadMaterialData(material);
	}
}