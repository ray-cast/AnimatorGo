#include <octoon/offline_mesh_renderer_component.h>
#include <octoon/offline_feature.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/video/render_system.h>
#include <octoon/game_prefabs.h>
#include <octoon/image/image.h>

#include <RadeonProRender.h>

namespace octoon
{
	OctoonImplementSubClass(OfflineMeshRendererComponent, OfflineRenderComponent, "OfflineMeshRenderer")

	OfflineMeshRendererComponent::OfflineMeshRendererComponent() noexcept
	{
	}

	OfflineMeshRendererComponent::OfflineMeshRendererComponent(model::Materials&& materials) noexcept
	{
		this->setMaterials(std::move(materials));
	}

	OfflineMeshRendererComponent::OfflineMeshRendererComponent(model::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	OfflineMeshRendererComponent::OfflineMeshRendererComponent(const model::Materials& materials) noexcept
	{
		this->setMaterials(materials);
	}

	OfflineMeshRendererComponent::OfflineMeshRendererComponent(const model::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	OfflineMeshRendererComponent::~OfflineMeshRendererComponent() noexcept
	{
	}

	std::pair<void*, void*>
	OfflineMeshRendererComponent::createMaterialTextures(const std::string& path) noexcept
	{	
		try
		{
			auto feature = this->tryGetFeature<OfflineFeature>();
			if (!feature)
				return std::make_pair(nullptr, nullptr);

			image::Image image;
			if (!image.load(path))
				return std::make_pair(nullptr, nullptr);

			bool hasAlpha = false;
			std::uint8_t channel = 3;
			switch (image.format())
			{
			case image::Format::B8G8R8A8UNorm:
			case image::Format::R8G8B8A8SNorm:
			case image::Format::R8G8B8A8SRGB:
			case image::Format::B8G8R8A8SRGB:
				hasAlpha = true;
				channel = 4;
				break;
			}

			rpr_image_format rgbFormat = { 3, RPR_COMPONENT_TYPE_UINT8 };
			rpr_image_desc rgbDesc;
			rgbDesc.image_width = image.width();
			rgbDesc.image_height = image.height();
			rgbDesc.image_depth = 1;
			rgbDesc.image_row_pitch = image.width() * rgbFormat.num_components;
			rgbDesc.image_slice_pitch = rgbDesc.image_row_pitch * rgbDesc.image_height;

			rpr_image_format alphaFormat = { 1, RPR_COMPONENT_TYPE_UINT8 };
			rpr_image_desc alphaDesc;
			alphaDesc.image_width = image.width();
			alphaDesc.image_height = image.height();
			alphaDesc.image_depth = 1;
			alphaDesc.image_row_pitch = image.width() * alphaFormat.num_components;
			alphaDesc.image_slice_pitch = alphaDesc.image_row_pitch * alphaDesc.image_height;

			auto data = image.data();

			std::vector<std::uint8_t> srgb((rgbDesc.image_width * rgbDesc.image_height * rgbFormat.num_components));
			std::vector<std::uint8_t> alpha;

			if (hasAlpha)
			{
				alpha.resize(alphaDesc.image_width * alphaDesc.image_height * alphaFormat.num_components);

#				pragma omp parallel for num_threads(4)
				for (std::size_t y = 0; y < rgbDesc.image_height; y++)
				{
					auto srcHeight = y * rgbDesc.image_width;
					auto dstHeight = (rgbDesc.image_height - 1 - y) * rgbDesc.image_width;

					for (std::size_t x = 0; x < rgbDesc.image_width; x++)
					{
						auto dstRGB = (dstHeight + x) * rgbFormat.num_components;
						auto dstAlpha = (dstHeight + x) * alphaFormat.num_components;
						auto src = (srcHeight + x) * 4;

						srgb[dstRGB] = data[src];
						srgb[dstRGB + 1] = data[src + 1];
						srgb[dstRGB + 2] = data[src + 2];
						alpha[dstAlpha] = 255 - std::pow(data[src + 3] / 255.0f, 2.2f) * 255.0f;
					}
				}
			}
			else
			{
				for (std::size_t y = 0; y < rgbDesc.image_height; y++)
				{
					auto dst = ((rgbDesc.image_height - 1 - y) * rgbDesc.image_width) * rgbFormat.num_components;
					auto src = y * rgbDesc.image_width * rgbFormat.num_components;

					std::memcpy(srgb.data() + dst, data + src, rgbDesc.image_width * rgbFormat.num_components);
				}
			}

			rpr_image image_ = nullptr;
			rpr_image alphaImage_ = nullptr;

			if (!srgb.empty())
				rprContextCreateImage(feature->getContext(), rgbFormat, &rgbDesc, srgb.data(), &image_);

			if (!alpha.empty())
			{
				hasAlpha = false;
				for (auto& it : alpha)
				{
					if (it > 0)
					{
						hasAlpha = true;
						break;
					}
				}
				
				if (hasAlpha)
					rprContextCreateImage(feature->getContext(), alphaFormat, &alphaDesc, alpha.data(), &alphaImage_);
			}

			if (image_)
				images_.push_back(image_);
			if (alphaImage_)
				images_.push_back(alphaImage_);

			return std::make_pair(image_, alphaImage_);
		}
		catch (...)
		{
			return std::make_pair(nullptr, nullptr);
		}
	}

	void
	OfflineMeshRendererComponent::uploadMaterialData(const model::Materials& materials) noexcept
	{
		auto feature = this->tryGetFeature<OfflineFeature>();
		if (!feature)
			return;

		for (auto& it : this->materials_)
			rprObjectDelete(it);

		this->materials_.clear();

		for (auto& mat : materials)
		{
			std::string name;
			std::string path;
			std::string normalName;
			std::string textureName;

			math::float3 base = math::float3(1.0f, 0.0f, 1.0f);
			math::float3 specular = math::float3::One;
			math::float3 ambient;
			math::float4 edgeColor = math::float4::Zero;
			
			float opacity = 1.0f;
			float shininess = 10.0f;
			
			mat->get(MATKEY_NAME, name);
			mat->get(MATKEY_PATH, path);
			mat->get(MATKEY_TEXTURE_SPECULAR, normalName);
			mat->get(MATKEY_TEXTURE_DIFFUSE, textureName);
			mat->get(MATKEY_COLOR_DIFFUSE, base);
			mat->get(MATKEY_COLOR_AMBIENT, ambient);			
			mat->get(MATKEY_COLOR_SPECULAR, specular);
			mat->get(MATKEY_COLOR_EDGE, edgeColor);
			mat->get(MATKEY_OPACITY, opacity);
			mat->get(MATKEY_SHININESS, shininess);

			float roughness = 1.0f - math::saturate(shininess / 1000.0f);

			rpr_material_node rprMaterial;
			rprMaterialSystemCreateNode(feature->getMaterialSystem(), RPR_MATERIAL_NODE_UBERV2, &rprMaterial);

			std::uint32_t layers = 0;
			if (ambient != math::float3::Zero)
				layers |= RPR_UBER_MATERIAL_LAYER_EMISSION;
			else
			{
				layers |= RPR_UBER_MATERIAL_LAYER_REFLECTION;

				if (opacity < 1.0f)
					layers |= RPR_UBER_MATERIAL_LAYER_TRANSPARENCY;
				if (!normalName.empty())
					layers |= RPR_UBER_MATERIAL_LAYER_SHADING_NORMAL;
				if (edgeColor.x == 0.0f)
					layers |= RPR_UBER_MATERIAL_LAYER_DIFFUSE;
			}

			rprMaterialNodeSetInputU(rprMaterial, "uberv2.layers", layers);

			if (layers & RPR_UBER_MATERIAL_LAYER_TRANSPARENCY)
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.transparency", 1.0f - opacity, 1.0f, 1.0f, 1.0f);

			if (layers & RPR_UBER_MATERIAL_LAYER_EMISSION)
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.emission.color", ambient.x, ambient.y, ambient.z, 0.0f);

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
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.anisotropy", edgeColor.w, edgeColor.w, edgeColor.w, edgeColor.w);
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.sheen", edgeColor.y, edgeColor.y, edgeColor.y, edgeColor.y);
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.color", specular.x, specular.y, specular.z, 1.0f);
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.metalness", edgeColor.x, edgeColor.x, edgeColor.x, edgeColor.x);
			}

			if (layers & RPR_UBER_MATERIAL_LAYER_REFRACTION)
			{
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.refraction.ior", 1.5f, 1.5f, 1.5f, 1.5f);
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.refraction.roughness", 1.0f, 1.0f, 1.0f, 1.0f);
				rprMaterialNodeSetInputF(rprMaterial, "uberv2.refraction.color", 1.0f, 1.0f, 1.0f, 1.0f);
			}

			if (layers & RPR_UBER_MATERIAL_LAYER_SHADING_NORMAL)
			{
				rpr_material_node textureNode;
				rprMaterialSystemCreateNode(feature->getMaterialSystem(), RPR_MATERIAL_NODE_NORMAL_MAP, &textureNode);
				rprMaterialNodeSetInputImageData(textureNode, "data", this->createMaterialTextures(path + normalName).first);
				rprMaterialNodeSetInputN(rprMaterial, "uberv2.normal", textureNode);
			}

			if (!textureName.empty())
			{
				auto image = this->createMaterialTextures(path + textureName);
				if (image.first)
				{
					rpr_material_node textureNode;
					rprMaterialSystemCreateNode(feature->getMaterialSystem(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &textureNode);
					rprMaterialNodeSetInputImageData(textureNode, "data", image.first);

					if (layers & RPR_UBER_MATERIAL_LAYER_DIFFUSE)
						rprMaterialNodeSetInputN(rprMaterial, "uberv2.diffuse.color", textureNode);

					if (layers & RPR_UBER_MATERIAL_LAYER_REFLECTION && edgeColor.x > 0.0f)
						rprMaterialNodeSetInputN(rprMaterial, "uberv2.reflection.color", textureNode);

					if (layers & RPR_UBER_MATERIAL_LAYER_REFRACTION)
						rprMaterialNodeSetInputN(rprMaterial, "uberv2.refraction.color", textureNode);
				}
				else
				{
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.diffuse.color", 1.0f, 0.0f, 1.0f, 1.0f);
				}

				if (image.second)
				{
					rpr_material_node textureNode;
					rprMaterialSystemCreateNode(feature->getMaterialSystem(), RPR_MATERIAL_NODE_IMAGE_TEXTURE, &textureNode);
					rprMaterialNodeSetInputImageData(textureNode, "data", image.second);

					rprMaterialNodeSetInputU(rprMaterial, "uberv2.layers", layers | RPR_UBER_MATERIAL_LAYER_TRANSPARENCY);
					rprMaterialNodeSetInputN(rprMaterial, "uberv2.transparency", textureNode);
				}
			}

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
	OfflineMeshRendererComponent::uploadMeshData(const model::Mesh& mesh) noexcept
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

			for (std::size_t i = 0; i < mesh.getNumSubsets(); i++)
			{
				math::uint1s faceArray(mesh.getIndicesArray(i).size() / 3, 3);

				rpr_shape rprShape;
				rprContextCreateMesh(feature->getContext(),
					mesh.getVertexArray().data()->ptr(), mesh.getVertexArray().size() / 3, sizeof(math::float3),
					mesh.getNormalArray().data()->ptr(), mesh.getNormalArray().size() / 3, sizeof(math::float3),
					mesh.getTexcoordArray().data()->ptr(), mesh.getTexcoordArray().size() / 2, sizeof(math::float2),
					(rpr_int*)mesh.getIndicesArray(i).data(), sizeof(rpr_int),
					(rpr_int*)mesh.getIndicesArray(i).data(), sizeof(rpr_int),
					(rpr_int*)mesh.getIndicesArray(i).data(), sizeof(rpr_int),
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
			feature->removeOfflineListener(this);
			for (auto& shape : shapes_)
				rprSceneDetachShape(feature->getScene(), shape);
		}

		for (auto& shape : shapes_)
			rprObjectDelete(shape);
		shapes_.clear();

		for (auto& material : materials_)
			rprObjectDelete(material);
		materials_.clear();

		for (auto& image : images_)
			rprObjectDelete(image);
		images_.clear();
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
	OfflineMeshRendererComponent::onMeshReplace(const runtime::any& mesh_) noexcept
	{
		auto mesh = runtime::any_cast<model::MeshPtr>(mesh_);
		if (mesh && this->getMaterial())
			this->uploadMeshData(*mesh);
	}

	void
	OfflineMeshRendererComponent::onMaterialReplace(const model::Materials& material) noexcept
	{
		if (this->getGameScene())
		{
			this->uploadMaterialData(material);
			this->trySendMessage("octoon:mesh:get");
		}
	}
}