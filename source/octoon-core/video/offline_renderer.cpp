#include "offline_renderer.h"
#include <octoon/runtime/except.h>

#include <octoon/camera/ortho_camera.h>
#include <octoon/camera/perspective_camera.h>

#include <octoon/light/ambient_light.h>
#include <octoon/light/directional_light.h>
#include <octoon/light/point_light.h>
#include <octoon/light/spot_light.h>
#include <octoon/light/disk_light.h>
#include <octoon/light/rectangle_light.h>
#include <octoon/light/environment_light.h>
#include <octoon/light/tube_light.h>

#include <octoon/image/image.h>
#include <octoon/material/mesh_standard_material.h>

#include <assert.h>
#include <atomic>
#include <string>
#include <CL/cl.h>
#include <stdexcept>

#include <GL/GL.h>

#include <RadeonProRender.h>
#include <RadeonProRender_GL.h>

namespace octoon::video
{
	OfflineRenderer::OfflineRenderer() noexcept
		: width_(0)
		, height_(0)
		, rprContext_(nullptr)
		, rprScene_(nullptr)
		, rprMaterialSystem_(nullptr)
		, colorFramebuffer_(nullptr)
		, normalFramebuffer_(nullptr)
		, albedoFramebuffer_(nullptr)
		, tileNums_(0)
		, dirty_(true)
	{
	}

	OfflineRenderer::OfflineRenderer(std::uint32_t w, std::uint32_t h) noexcept
		: OfflineRenderer()
	{
		this->setup(w, h);
	}

	OfflineRenderer::~OfflineRenderer() noexcept
	{
	}

	void
	OfflineRenderer::setup(std::uint32_t w, std::uint32_t h) noexcept(false)
	{
		width_ = w;
		height_ = h;

		auto GetErrorString = [](int status)
		{
			switch (status)
			{
			case RPR_ERROR_COMPUTE_API_NOT_SUPPORTED:         return "RPR_ERROR_COMPUTE_API_NOT_SUPPORTED";
			case RPR_ERROR_OUT_OF_SYSTEM_MEMORY:              return "RPR_ERROR_OUT_OF_SYSTEM_MEMORY";
			case RPR_ERROR_OUT_OF_VIDEO_MEMORY:               return "RPR_ERROR_OUT_OF_VIDEO_MEMORY";
			case RPR_ERROR_INVALID_LIGHTPATH_EXPR:            return "RPR_ERROR_INVALID_LIGHTPATH_EXPR";
			case RPR_ERROR_INVALID_IMAGE:                     return "RPR_ERROR_INVALID_IMAGE";
			case RPR_ERROR_INVALID_AA_METHOD:                 return "RPR_ERROR_INVALID_AA_METHOD";
			case RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT:          return "RPR_ERROR_UNSUPPORTED_IMAGE_FORMAT";
			case RPR_ERROR_INVALID_GL_TEXTURE:                return "RPR_ERROR_INVALID_GL_TEXTURE";
			case RPR_ERROR_INVALID_CL_IMAGE:                  return "RPR_ERROR_INVALID_CL_IMAGE";
			case RPR_ERROR_INVALID_OBJECT:                    return "RPR_ERROR_INVALID_OBJECT";
			case RPR_ERROR_INVALID_PARAMETER:                 return "RPR_ERROR_INVALID_PARAMETER";
			case RPR_ERROR_INVALID_TAG:                       return "RPR_ERROR_INVALID_TAG";
			case RPR_ERROR_INVALID_LIGHT:                     return "RPR_ERROR_INVALID_LIGHT";
			case RPR_ERROR_INVALID_CONTEXT:                   return "RPR_ERROR_INVALID_CONTEXT";
			case RPR_ERROR_UNIMPLEMENTED:                     return "RPR_ERROR_UNIMPLEMENTED";
			case RPR_ERROR_INVALID_API_VERSION:               return "RPR_ERROR_INVALID_API_VERSION";
			case RPR_ERROR_INTERNAL_ERROR:                    return "RPR_ERROR_INTERNAL_ERROR";
			case RPR_ERROR_IO_ERROR:                          return "RPR_ERROR_IO_ERROR";
			case RPR_ERROR_UNSUPPORTED_SHADER_PARAMETER_TYPE: return "RPR_ERROR_UNSUPPORTED_SHADER_PARAMETER_TYPE";
			case RPR_ERROR_MATERIAL_STACK_OVERFLOW:           return "RPR_ERROR_MATERIAL_STACK_OVERFLOW";
			case RPR_ERROR_INVALID_PARAMETER_TYPE:            return "RPR_ERROR_INVALID_PARAMETER_TYPE";
			case RPR_ERROR_UNSUPPORTED:                       return "RPR_ERROR_UNSUPPORTED";
			case RPR_ERROR_OPENCL_OUT_OF_HOST_MEMORY:         return "RPR_ERROR_OPENCL_OUT_OF_HOST_MEMORY";
			case RPR_ERROR_OPENGL:                            return "RPR_ERROR_OPENGL";
			case RPR_ERROR_OPENCL:                            return "RPR_ERROR_OPENCL";
			case RPR_ERROR_NULLPTR:                           return "RPR_ERROR_NULLPTR";
			case RPR_ERROR_NODETYPE:                          return "RPR_ERROR_NODETYPE";
			default: return "RPR_SUCCESS";
			}
		};

		auto status = rprCreateContext(RPR_API_VERSION, 0, 0, RPR_CREATION_FLAGS_ENABLE_GPU0 | RPR_CREATION_FLAGS_ENABLE_GL_INTEROP, 0, 0, &this->rprContext_);
		if (RPR_SUCCESS != status)
		{
			status = rprCreateContext(RPR_API_VERSION, 0, 0, RPR_CREATION_FLAGS_ENABLE_GPU1 | RPR_CREATION_FLAGS_ENABLE_GL_INTEROP, 0, 0, &this->rprContext_);
			if (RPR_SUCCESS != status)
				throw runtime::runtime_error::create(std::string("rprCreateContext() failed, error : ") + GetErrorString(status));
		}

		status = rprContextCreateScene(rprContext_, &rprScene_);
		if (RPR_SUCCESS != status)
			throw runtime::runtime_error::create(std::string("rprContextCreateScene() failed, error : ") + GetErrorString(status));

		status = rprContextCreateMaterialSystem(rprContext_, 0, &this->rprMaterialSystem_);
		if (RPR_SUCCESS != status)
			throw runtime::runtime_error::create(std::string("rprContextCreateMaterialSystem() failed, error : ") + GetErrorString(status));

		status = rprContextSetScene(rprContext_, rprScene_);
		if (RPR_SUCCESS != status)
			throw runtime::runtime_error::create(std::string("rprContextCreateMaterialSystem() failed, error : ") + GetErrorString(status));
	}

	void
	OfflineRenderer::setGraphicsContext(const hal::GraphicsContextPtr& context) noexcept(false)
	{
		this->context_ = context;
	}

	const hal::GraphicsContextPtr&
	OfflineRenderer::getGraphicsContext() const noexcept(false)
	{
		return this->context_;
	}

	std::pair<void*, void*>
	OfflineRenderer::createMaterialTextures(std::string_view path) noexcept(false)
	{
		try
		{
			auto it = imageNodes_.find(std::string(path));
			if (it != imageNodes_.end())
				return (*it).second;

			image::Image image;
			if (!image.load(std::string(path)))
				return std::make_pair(nullptr, nullptr);

			bool bgra = false;
			bool hasAlpha = false;
			std::uint8_t channel = 3;
			switch (image.format())
			{
			case image::Format::R8G8B8SNorm:
			case image::Format::R8G8B8SRGB:
			case image::Format::B8G8R8UNorm:
			case image::Format::B8G8R8SRGB:
				hasAlpha = false;
				break;
			case image::Format::R8G8B8A8SNorm:
			case image::Format::R8G8B8A8SRGB:
				hasAlpha = true;
				channel = 4;
				break;
			case image::Format::B8G8R8A8UNorm:
			case image::Format::B8G8R8A8SRGB:
				bgra = true;
				hasAlpha = true;
				channel = 4;
				break;
			default:
				throw runtime::runtime_error::create("This image type is not supported by this function:" + std::string(path));
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

				for (rpr_uint y = 0; y < rgbDesc.image_height; y++)
				{
					auto srcHeight = y * rgbDesc.image_width;
					auto dstHeight = (rgbDesc.image_height - 1 - y) * rgbDesc.image_width;

					for (std::size_t x = 0; x < rgbDesc.image_width; x++)
					{
						auto dstRGB = (dstHeight + x) * rgbFormat.num_components;
						auto dstAlpha = (dstHeight + x) * alphaFormat.num_components;
						auto src = (srcHeight + x) * 4;

						if (bgra)
						{
							srgb[dstRGB] = data[src + 2];
							srgb[dstRGB + 1] = data[src + 1];
							srgb[dstRGB + 2] = data[src];
						}
						else
						{
							srgb[dstRGB] = data[src];
							srgb[dstRGB + 1] = data[src + 1];
							srgb[dstRGB + 2] = data[src + 2];
						}

						alpha[dstAlpha] = static_cast<std::uint8_t>(255 - std::pow(data[src + 3] / 255.0f, 2.2f) * 255);
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

			rpr_material_node textureNode = nullptr;
			rpr_material_node alphaNode = nullptr;

			if (!srgb.empty())
			{
				rprContextCreateImage(this->rprContext_, rgbFormat, &rgbDesc, srgb.data(), &image_);
				rprMaterialSystemCreateNode(this->rprMaterialSystem_, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &textureNode);
				rprMaterialNodeSetInputImageData(textureNode, "data", image_);
			}

			if (!alpha.empty())
			{
				hasAlpha = false;
				for (auto& value : alpha)
				{
					if (value > 0)
					{
						hasAlpha = true;
						break;
					}
				}

				if (hasAlpha)
				{
					rprContextCreateImage(this->rprContext_, alphaFormat, &alphaDesc, alpha.data(), &alphaImage_);
					rprMaterialSystemCreateNode(this->rprMaterialSystem_, RPR_MATERIAL_NODE_IMAGE_TEXTURE, &alphaNode);
					rprMaterialNodeSetInputImageData(alphaNode, "data", alphaImage_);
				}
			}

			images_[std::string(path)] = std::make_pair(image_, alphaImage_);
			imageNodes_[std::string(path)] = std::make_pair(textureNode, alphaNode);

			return std::make_pair(textureNode, alphaNode);
		}
		catch (...)
		{
			return std::make_pair(nullptr, nullptr);
		}
	}

	bool
	OfflineRenderer::compileCamera(const camera::Camera* camera) noexcept(false)
	{
		bool force = false;

		auto& rprCamera = this->cameras_[camera];
		if (!rprCamera)
		{
			if (camera->isA<camera::PerspectiveCamera>())
			{
				if (RPR_SUCCESS != rprContextCreateCamera(this->rprContext_, &rprCamera))
					return false;
				if (RPR_SUCCESS != rprCameraSetMode(rprCamera, RPR_CAMERA_MODE_PERSPECTIVE))
					return false;

				force = true;
			}
			else if (camera->isA<camera::OrthoCamera>())
			{
				if (RPR_SUCCESS != rprContextCreateCamera(this->rprContext_, &rprCamera))
					return false;
				if (RPR_SUCCESS != rprCameraSetMode(rprCamera, RPR_CAMERA_MODE_ORTHOGRAPHIC))
					return false;

				force = true;
			}
		}

		if (camera->isDirty() && rprCamera || force)
		{
			if (camera->isA<camera::PerspectiveCamera>())
			{
				auto eye = camera->getTranslate();
				auto at = camera->getTranslate() + math::float3x3(camera->getTransform()) * math::float3::UnitZ;
				auto up = math::float3x3(camera->getTransform()) * math::float3::UnitY;

				auto filmSize_ = 36.0f;
				auto perspective = camera->downcast<camera::PerspectiveCamera>();
				auto ratio = std::tan(math::radians(perspective->getAperture()) * 0.5f) * 2.0f;
				auto focalLength = filmSize_ / ratio;

				if (RPR_SUCCESS != rprCameraSetNearPlane(rprCamera, perspective->getNear()))
					return false;
				if (RPR_SUCCESS != rprCameraSetFarPlane(rprCamera, perspective->getFar()))
					return false;
				if (RPR_SUCCESS != rprCameraSetFocalLength(rprCamera, focalLength))
					return false;
				if (RPR_SUCCESS != rprCameraSetFocusDistance(rprCamera, 1.0f))
					return false;
				if (RPR_SUCCESS != rprCameraSetSensorSize(rprCamera, filmSize_ * this->width_ / (float)this->height_, filmSize_))
					return false;
				if (RPR_SUCCESS != rprCameraLookAt(rprCamera, eye.x, eye.y, eye.z, at.x, at.y, at.z, up.x, up.y, up.z))
					return false;
				if (RPR_SUCCESS != rprSceneSetCamera(this->rprScene_, rprCamera))
					return false;
			}

			this->dirty_ = true;
		}
		else
		{
			rprSceneSetCamera(this->rprScene_, rprCamera);
		}

		return true;
	}

	bool
	OfflineRenderer::compileLight(const light::Light* light) noexcept(false)
	{
		bool force = false;

		auto& rprLight = this->lights_[light];
		if (!rprLight)
		{
			if (light->isA<light::EnvironmentLight>())
			{
				if (RPR_SUCCESS != rprContextCreateEnvironmentLight(this->rprContext_, &rprLight))
					return false;
				if (RPR_SUCCESS != rprSceneAttachLight(this->rprScene_, rprLight))
					return false;
				force = true;
			}
			else if (light->isA<light::DirectionalLight>())
			{
				if (RPR_SUCCESS != rprContextCreateDirectionalLight(this->rprContext_, &rprLight))
					return false;
				if (RPR_SUCCESS != rprSceneAttachLight(this->rprScene_, rprLight))
					return false;
				force = true;
			}
			else if (light->isA<light::SpotLight>())
			{
				if (RPR_SUCCESS != rprContextCreateSpotLight(this->rprContext_, &rprLight))
					return false;
				if (RPR_SUCCESS != rprSceneAttachLight(this->rprScene_, rprLight))
					return false;
				force = true;
			}
			else if (light->isA<light::PointLight>())
			{
				if (RPR_SUCCESS != rprContextCreatePointLight(this->rprContext_, &rprLight))
					return false;
				if (RPR_SUCCESS != rprSceneAttachLight(this->rprScene_, rprLight))
					return false;
				force = true;
			}
		}

		if (light->isDirty() && rprLight || force)
		{
			math::float4x4 matrix = light->getTransform();
			matrix = light->getTransform() * math::Quaternion(math::float3::UnitY, math::PI);
			matrix.setTranslate(light->getTranslate());

			auto color = light->getColor() * light->getIntensity();

			if (light->isA<light::EnvironmentLight>())
			{
				rpr_image rprImage = nullptr;
				rpr_image_format format = { 3, RPR_COMPONENT_TYPE_FLOAT32 };
				rpr_image_desc desc = { 1, 1, 1, 3, 3 };

				rpr_image oldImage = nullptr;
				rprLightGetInfo(rprLight, RPR_ENVIRONMENT_LIGHT_IMAGE, 0, &oldImage, 0);
				if (oldImage) {
					rprObjectDelete(oldImage);
				}

				auto environment = light->downcast<light::EnvironmentLight>();
				auto envmap = environment->getEnvironmentMap();
				if (envmap)
				{
					auto textureDesc = envmap->getTextureDesc();
					desc.image_width = textureDesc.getWidth();
					desc.image_height = textureDesc.getHeight();

					void* mapData = nullptr;
					if (envmap->map(0, 0, desc.image_width, desc.image_height, 0, &mapData))
					{
						if (RPR_SUCCESS != rprContextCreateImage(this->rprContext_, format, &desc, mapData, &rprImage))
							return false;
					}

					envmap->unmap();
				}
				else
				{
					if (RPR_SUCCESS != rprContextCreateImage(this->rprContext_, format, &desc, color.data(), &rprImage))
						return false;
				}
				
				if (RPR_SUCCESS != rprEnvironmentLightSetImage(rprLight, rprImage))
					return false;
				if (RPR_SUCCESS != rprEnvironmentLightSetIntensityScale(rprLight, light->getIntensity()))
					return false;
			}
			else if (light->isA<light::DirectionalLight>())
			{
				if (RPR_SUCCESS != rprDirectionalLightSetRadiantPower3f(rprLight, color.x, color.y, color.z))
					return false;
				if (RPR_SUCCESS != rprLightSetTransform(rprLight, false, matrix.ptr()))
					return false;
			}
			else if (light->isA<light::SpotLight>())
			{
				if (RPR_SUCCESS != rprSpotLightSetRadiantPower3f(rprLight, color.x, color.y, color.z))
					return false;
				if (RPR_SUCCESS != rprLightSetTransform(rprLight, false, matrix.ptr()))
					return false;
			}
			else if (light->isA<light::PointLight>())
			{
				if (RPR_SUCCESS != rprPointLightSetRadiantPower3f(rprLight, color.x, color.y, color.z))
					return false;
				if (RPR_SUCCESS != rprLightSetTransform(rprLight, false, matrix.ptr()))
					return false;
			}

			this->dirty_ = true;
		}

		return true;
	}

	bool
	OfflineRenderer::compileGeometry(const geometry::Geometry* geometry) noexcept(false)
	{
		bool force = false;

		auto& mesh = geometry->getMesh();
		auto& materials = geometry->getMaterials();

		for (auto& it : materials)
		{
			if (!it->isA<material::MeshStandardMaterial>())
				continue;

			auto& rprMaterial = this->materials_[it.get()];
			if (!rprMaterial)
			{
				rprMaterialSystemCreateNode(this->rprMaterialSystem_, RPR_MATERIAL_NODE_UBERV2, &rprMaterial);
				force = true;
			}

			if (it->isDirty() && rprMaterial || force)
			{
				std::string path;
				std::string normalName;
				std::string textureName;

				auto material = it->downcast<material::MeshStandardMaterial>();
				math::float3 base = material->getColor();
				math::float3 emissive = material->getEmissive();

				float opacity = material->getOpacity();
				float roughness = (1.0f - material->getSmoothness()) * (1.0f - material->getSmoothness());
				float metalness = material->getMetalness();
				float reflectivity = material->getReflectivity();

				auto colorTexture = material->getColorTexture();
				if (colorTexture)
					textureName = colorTexture->getTextureDesc().getName();

				std::uint32_t layers = 0;
				if (metalness == 0.0f)
					layers |= RPR_UBER_MATERIAL_LAYER_DIFFUSE;
				if (reflectivity > 0.0f)
					layers |= RPR_UBER_MATERIAL_LAYER_REFLECTION;
				if (opacity < 1.0f)
					layers |= RPR_UBER_MATERIAL_LAYER_TRANSPARENCY;
				if (math::any(emissive))
					layers |= RPR_UBER_MATERIAL_LAYER_EMISSION;

				rprMaterialNodeSetInputU(rprMaterial, "uberv2.layers", layers);

				if (layers & RPR_UBER_MATERIAL_LAYER_TRANSPARENCY)
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.transparency", 1.0f - opacity, 1.0f, 1.0f, 1.0f);

				if (layers & RPR_UBER_MATERIAL_LAYER_DIFFUSE)
				{
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.diffuse.roughness", roughness, roughness, roughness, roughness);
					//rprMaterialNodeSetInputF(rprMaterial, "uberv2.diffuse.subsurface", edgeColor.z, edgeColor.z, edgeColor.z, edgeColor.z);
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.diffuse.color", base[0], base[1], base[2], 1.0f);
				}

				if (layers & RPR_UBER_MATERIAL_LAYER_REFLECTION)
				{
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.ior", 1.5f, 1.5f, 1.5f, 1.5f);
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.roughness", roughness, roughness, roughness, roughness);
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.metalness", metalness, metalness, metalness, metalness);
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.color", base[0], base[1], base[2], 1.0f);
					//rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.anisotropy", edgeColor.w, edgeColor.w, edgeColor.w, edgeColor.w);
					//rprMaterialNodeSetInputF(rprMaterial, "uberv2.reflection.sheen", edgeColor.y, edgeColor.y, edgeColor.y, edgeColor.y);
				}

				if (layers & RPR_UBER_MATERIAL_LAYER_REFRACTION)
				{
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.refraction.ior", 1.5f, 1.5f, 1.5f, 1.5f);
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.refraction.roughness", 1.0f, 1.0f, 1.0f, 1.0f);
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.refraction.color", 1.0f, 1.0f, 1.0f, 1.0f);
				}

				if (layers & RPR_UBER_MATERIAL_LAYER_EMISSION)
				{
					rprMaterialNodeSetInputF(rprMaterial, "uberv2.emission.color", emissive[0], emissive[1], emissive[2], 1.0f);
				}

				if (!textureName.empty())
				{
					auto image = this->createMaterialTextures(path + textureName);
					if (image.first)
					{
						if (layers & RPR_UBER_MATERIAL_LAYER_DIFFUSE)
							rprMaterialNodeSetInputN(rprMaterial, "uberv2.diffuse.color", image.first);

						if (layers & RPR_UBER_MATERIAL_LAYER_REFLECTION && reflectivity > 0.0f)
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

				force = false;
				this->dirty_ = true;
			}
		}

		for (std::int32_t i = 0; i < mesh->getNumSubsets(); i++)
		{
			auto& rprShape = shape_[mesh->getIndicesArray(i).data()];
			if (!rprShape)
			{
				math::uint1s faceArray(mesh->getIndicesArray(i).size() / 3, 3);

				rprContextCreateMesh(this->rprContext_,
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
				rprShapeSetTransform(rprShape, false, geometry->getTransform().ptr());
				rprSceneAttachShape(this->rprScene_, rprShape);

				rprShapeSetMaterial(rprShape, this->materials_[geometry->getMaterial(i).get()]);
			}
		}

		return true;
	}

	bool
	OfflineRenderer::compileScene(const camera::Camera* camera, const std::vector<light::Light*>& lights, const std::vector<geometry::Geometry*>& geometries) noexcept(false)
	{
		this->compileCamera(camera);

		for (auto& light : lights)
		{
			if (camera->getLayer() == light->getLayer())
			{
				if (light->getVisible())
					this->compileLight(light);
			}
		
		}

		for (auto& geometry : geometries)
		{
			if (geometry->getVisible() && geometry->getGlobalIllumination())
				this->compileGeometry(geometry);
		}

		return true;
	}

	void
	OfflineRenderer::generateWorkspace(std::uint32_t numEstimate)
	{
		if (tileNums_ < numEstimate)
		{
			hal::GraphicsTextureDesc colorTextureDesc;
			colorTextureDesc.setWidth(this->width_);
			colorTextureDesc.setHeight(this->height_);
			colorTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2D);
			colorTextureDesc.setTexFormat(hal::GraphicsFormat::R32G32B32A32SFloat);
			colorTexture_ = context_->getDevice()->createTexture(colorTextureDesc);
			if (!colorTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			normalTexture_ = context_->getDevice()->createTexture(colorTextureDesc);
			if (!normalTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			albedoTexture_ = context_->getDevice()->createTexture(colorTextureDesc);
			if (!albedoTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			rpr_framebuffer colorFramebuffer = nullptr;
			if (RPR_SUCCESS == rprContextCreateFramebufferFromGLTexture2D(rprContext_, GL_TEXTURE_2D, 0, static_cast<rpr_uint>(colorTexture_->handle()), &colorFramebuffer))
			{
				rprContextSetAOV(rprContext_, RPR_AOV_COLOR, colorFramebuffer);

				if (this->colorFramebuffer_)
					rprObjectDelete(this->colorFramebuffer_);

				this->colorFramebuffer_ = colorFramebuffer;
			}

			rpr_framebuffer normalFramebuffer = nullptr;
			if (RPR_SUCCESS == rprContextCreateFramebufferFromGLTexture2D(rprContext_, GL_TEXTURE_2D, 0, static_cast<rpr_uint>(normalTexture_->handle()), &normalFramebuffer))
			{
				rprContextSetAOV(rprContext_, RPR_AOV_SHADING_NORMAL, normalFramebuffer);

				if (this->normalFramebuffer_)
					rprObjectDelete(this->normalFramebuffer_);

				this->normalFramebuffer_ = normalFramebuffer;
			}

			rpr_framebuffer albedoFramebuffer = nullptr;
			if (RPR_SUCCESS == rprContextCreateFramebufferFromGLTexture2D(rprContext_, GL_TEXTURE_2D, 0, static_cast<rpr_uint>(albedoTexture_->handle()), &albedoFramebuffer))
			{
				rprContextSetAOV(rprContext_, RPR_AOV_ALBEDO, albedoFramebuffer);

				if (this->albedoFramebuffer_)
					rprObjectDelete(this->albedoFramebuffer_);

				this->albedoFramebuffer_ = albedoFramebuffer;
			}

			hal::GraphicsTextureDesc depthTextureDesc;
			depthTextureDesc.setWidth(this->width_);
			depthTextureDesc.setHeight(this->height_);
			depthTextureDesc.setTexDim(hal::GraphicsTextureDim::Texture2D);
			depthTextureDesc.setTexFormat(hal::GraphicsFormat::D32_SFLOAT);
			auto depthTexture_ = context_->getDevice()->createTexture(depthTextureDesc);
			if (!depthTexture_)
				throw runtime::runtime_error::create("createTexture() failed");

			hal::GraphicsFramebufferLayoutDesc framebufferLayoutDesc;
			framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(0, hal::GraphicsImageLayout::ColorAttachmentOptimal, hal::GraphicsFormat::R32G32B32SFloat));
			framebufferLayoutDesc.addComponent(hal::GraphicsAttachmentLayout(1, hal::GraphicsImageLayout::DepthStencilAttachmentOptimal, hal::GraphicsFormat::D32_SFLOAT));

			hal::GraphicsFramebufferDesc framebufferDesc;
			framebufferDesc.setWidth(this->width_);
			framebufferDesc.setHeight(this->height_);
			framebufferDesc.setFramebufferLayout(context_->getDevice()->createFramebufferLayout(framebufferLayoutDesc));
			framebufferDesc.setDepthStencilAttachment(hal::GraphicsAttachmentBinding(depthTexture_, 0, 0));
			framebufferDesc.addColorAttachment(hal::GraphicsAttachmentBinding(colorTexture_, 0, 0));

			this->framebuffer_ = context_->getDevice()->createFramebuffer(framebufferDesc);
			if (!this->framebuffer_)
				throw runtime::runtime_error::create("createFramebuffer() failed");

			this->tileNums_ = numEstimate;
		}
	}

	void
	OfflineRenderer::estimate(const camera::Camera* camera, std::uint32_t frame, const RadeonRays::int2& offset, const RadeonRays::int2& size)
	{
		this->generateWorkspace(size.x * size.y);

		rpr_camera rprCamera;
		rprSceneGetCamera(this->rprScene_, &rprCamera);
		if (rprCamera)
		{
			std::size_t count = 0;
			std::size_t type_size = 0;

			rprSceneGetInfo(this->rprScene_, RPR_SCENE_SHAPE_COUNT, 1, &count, &type_size);
			if (count == 0)
				return;

			rprSceneGetInfo(this->rprScene_, RPR_SCENE_LIGHT_COUNT, 1, &count, &type_size);
			if (count == 0)
				return;

			if (this->dirty_)
			{
				if (this->colorFramebuffer_)
					rprFrameBufferClear(this->colorFramebuffer_);

				if (this->normalFramebuffer_)
					rprFrameBufferClear(this->normalFramebuffer_);

				if (this->albedoFramebuffer_)
					rprFrameBufferClear(this->albedoFramebuffer_);

				this->dirty_ = false;
			}

			rprContextRender(rprContext_);

			math::float4 viewport(0, 0, static_cast<float>(this->width_), static_cast<float>(this->height_));
			this->context_->blitFramebuffer(framebuffer_, viewport, nullptr, viewport);
		}
	}

	void
	OfflineRenderer::render(const camera::Camera* camera, const std::vector<light::Light*>& light, const std::vector<geometry::Geometry*>& geometries, std::uint32_t frame, std::uint32_t x, std::uint32_t y, std::uint32_t w, std::uint32_t h) noexcept
	{
		if (this->compileScene(camera, light, geometries))
			this->estimate(camera, frame, RadeonRays::int2(x, y), RadeonRays::int2(w, h));
	}
}