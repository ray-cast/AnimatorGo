#include <octoon/game_prefabs.h>
#include <octoon/graphics/graphics.h>
#include <octoon/image/image.h>
#include <octoon/io/vstream.h>

#include <octoon/video/render_system.h>
#include <octoon/video/ggx_material.h>
#include <octoon/video/basic_material.h>

#include <octoon/model/model.h>
#include <octoon/model/property.h>
#include <octoon/model/text_meshing.h>

#include <octoon/camera_component.h>
#include <octoon/transform_component.h>
#include <octoon/first_person_camera_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/string.h>

using namespace octoon::io;
using namespace octoon::image;
using namespace octoon::graphics;
using namespace octoon::video;
using namespace octoon::model;

namespace octoon
{
	OctoonImplementSingleton(GamePrefabs)

	GamePrefabs::GamePrefabs() noexcept
	{
	}

	GamePrefabs::~GamePrefabs() noexcept
	{
	}

	GameObjectPtr
	GamePrefabs::createCamera(float fov) except
	{
		auto object = GameObject::create("MainCamera");
		object->getComponent<TransformComponent>()->setTranslate(math::float3(0, 1, -10));
		object->addComponent<FirstPersonCameraComponent>();

		auto camera = object->addComponent<CameraComponent>();
		camera->setCameraOrder(video::CameraOrder::Main);
		camera->setCameraType(video::CameraType::Perspective);
		camera->setAperture(fov);

		return object;
	}

	GameObjectPtr
	GamePrefabs::createCamera2D() except
	{
		auto camera = GameObject::create("MainCamera");
		camera->addComponent<CameraComponent>();
		camera->getComponent<CameraComponent>()->setCameraOrder(video::CameraOrder::Main);
		camera->getComponent<CameraComponent>()->setCameraType(video::CameraType::Ortho);
		camera->getComponent<CameraComponent>()->setOrtho(math::float4(0.0f, 1.0f, 0.0f, 1.0f));
		camera->getComponent<TransformComponent>()->setTranslate(math::float3(0.0f, 0.0f, -10.0f));
		camera->addComponent<FirstPersonCameraComponent>();

		return camera;
	}

	GameObjectPtr
	GamePrefabs::createFrustum() except
	{
		auto camera = GameObject::create("MainCamera");
		camera->addComponent<CameraComponent>();
		camera->getComponent<CameraComponent>()->setCameraOrder(video::CameraOrder::Main);
		camera->getComponent<CameraComponent>()->setCameraType(video::CameraType::Frustum);
		camera->getComponent<CameraComponent>()->setNear(1.0f);
		camera->getComponent<CameraComponent>()->setOrtho(math::float4(0.0f, 1.0f, 0.0f, 1.0f));
		camera->getComponent<TransformComponent>()->setTranslate(math::float3(0.0f, 0.0f, -10.0f));
		camera->addComponent<FirstPersonCameraComponent>();

		return camera;
	}

	GameObjectPtr
	GamePrefabs::createCircle(float radius, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeCircle(radius, segments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createPlane(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makePlane(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createPlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makePlane(width, height, depth, widthSegments, heightSegments, depthSegments, u, v, udir, vdir));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createPlaneWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makePlaneWireframe(width, height, depth, widthSegments, heightSegments, depthSegments, u, v, udir, vdir, clear));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createFloor(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeFloor(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createNoise(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeNoise(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createCube(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeCube(width, height, depth, widthSegments, heightSegments, depthSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createCubeWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeCubeWireframe(width, height, depth, widthSegments, heightSegments, depthSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeRing(innerRadius, outerRadius, thetaSegments, phiSegments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createSphere(float radius, std::uint32_t widthSegments, std::uint32_t heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeSphere(radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createVolumes(float fovy, float znear, float zfar) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeVolumes(fovy, znear, zfar));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createCone(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeCone(radius, height, segments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr 
	GamePrefabs::createText(const wchar_t* text, std::uint16_t fontsize, const char* fontPath) noexcept
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeMesh(model::makeTextContours(text, { fontPath, fontsize })));
		object->addComponent<MeshRendererComponent>(std::make_shared<BasicMaterial>());

		return object;
	}

	GameObjectPtr 
	GamePrefabs::createText(const char* u8str, std::uint16_t fontsize, const char* fontPath) noexcept
	{
		std::string::size_type len = std::strlen(u8str);

		std::wstring u16str;
		u16str.reserve(len);

		const unsigned char* p = (unsigned char*)(u8str);
		if (len > 3 && p[0] == 0xEF && p[1] == 0xBB && p[2] == 0xBF) {
			p += 3;
			len -= 3;
		}

		bool is_ok = true;
		for (std::string::size_type i = 0; i < len; ++i) 
		{
			uint32_t ch = p[i];
			if ((ch & 0x80) == 0)
			{
				u16str.push_back((char16_t)ch);
				continue;
			}

			switch (ch & 0xF0)
			{
			case 0xF0:
			{
				uint32_t c2 = p[++i];
				uint32_t c3 = p[++i];
				uint32_t c4 = p[++i];
				uint32_t codePoint = ((ch & 0x07U) << 18) | ((c2 & 0x3FU) << 12) | ((c3 & 0x3FU) << 6) | (c4 & 0x3FU);
				if (codePoint >= 0x10000)
				{
					codePoint -= 0x10000;
					u16str.push_back((char16_t)((codePoint >> 10) | 0xD800U));
					u16str.push_back((char16_t)((codePoint & 0x03FFU) | 0xDC00U));
				}
				else
				{
					u16str.push_back((char16_t)codePoint);
				}
			}
			break;
			case 0xE0:
			{
				uint32_t c2 = p[++i];
				uint32_t c3 = p[++i];
				uint32_t codePoint = ((ch & 0x0FU) << 12) | ((c2 & 0x3FU) << 6) | (c3 & 0x3FU);
				u16str.push_back((char16_t)codePoint);
			}
			break;
			case 0xD0:
			case 0xC0:
			{
				uint32_t c2 = p[++i];
				uint32_t codePoint = ((ch & 0x1FU) << 12) | ((c2 & 0x3FU) << 6);
				u16str.push_back((char16_t)codePoint);
			}
			break;
			default:
				is_ok = false;
				break;
			}
		}

		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makeMesh(model::makeTextContours(u16str, { fontPath, fontsize }, 8),0.0f));
		object->addComponent<MeshRendererComponent>(std::make_shared<BasicMaterial>());

		return object;
	}

	GameObjectPtr 
	GamePrefabs::createModel(const std::string& path, bool cache) except
	{
		auto it = prefabs_.find(path);
		if (it != prefabs_.end())
			return (*it).second->clone();

		Model model(path);

		auto actor = GameObject::create(path);
		auto rootPath = runtime::string::directory(path);

		for (std::size_t i = 0; i < model.get<Model::material>().size(); i++)
		{
			auto mesh = model.get<Model::mesh>(i);
			auto materialProp = model.get<Model::material>(i);

			std::string name;
			materialProp->get(MATKEY_NAME, name);

			std::string textureName;
			materialProp->get(MATKEY_TEXTURE_DIFFUSE(0), textureName);

			math::float3 base;
			materialProp->get(MATKEY_COLOR_DIFFUSE, base);

			math::float3 ambient;
			materialProp->get(MATKEY_COLOR_AMBIENT, ambient);

			auto material = std::make_shared<video::GGXMaterial>();
			material->setAmbientColor(base);
			material->setBaseColor(math::float3::Zero);
			material->setTexture(GamePrefabs::instance()->createTexture(rootPath + textureName));

			auto object = GameObject::create(std::move(name));
			object->addComponent<MeshFilterComponent>(mesh);
			object->addComponent<MeshRendererComponent>(material);
			object->setParent(actor);
		}

		prefabs_[path] = actor->clone();

		return actor;
	}

	GameObjectPtr 
	GamePrefabs::createSprite(const std::string& image, std::uint32_t w, std::uint32_t h) except
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makePlane(w, h));
		object->addComponent<MeshRendererComponent>(std::make_shared<BasicMaterial>(this->createTexture(image)));

		return object;
	}
	
	GameObjectPtr 
	GamePrefabs::createSprite(const graphics::GraphicsTexturePtr& texture, std::uint32_t w, std::uint32_t h) except
	{
		auto object = GameObject::create();
		object->addComponent<MeshFilterComponent>(model::makePlane(w, h));
		object->addComponent<MeshRendererComponent>(std::make_shared<BasicMaterial>(texture));

		return object;
	}

	GameObjectPtr
	GamePrefabs::createSpriteSquare(std::uint32_t w, std::uint32_t h) except
	{
		return this->createSprite("sys:sprite/square.png", w, h);
	}

	graphics::GraphicsTexturePtr
	GamePrefabs::createTexture(const std::string& path, bool cache) except
	{
		assert(!path.empty());

		auto it = textureCaches_.find(path);
		if (it != textureCaches_.end())
			return (*it).second;

		vstream stream;
		if (!stream.open(path))
			throw runtime::runtime_error::create("Failed to open file :" + path);

		Image image;
		if (!image.load(stream))
			throw runtime::runtime_error::create("Unknown stream:" + path);

		GraphicsFormat format = GraphicsFormat::Undefined;
		switch (image.format())
		{
		case Format::BC1RGBUNormBlock: format = GraphicsFormat::BC1RGBUNormBlock; break;
		case Format::BC1RGBAUNormBlock: format = GraphicsFormat::BC1RGBAUNormBlock; break;
		case Format::BC1RGBSRGBBlock: format = GraphicsFormat::BC1RGBSRGBBlock; break;
		case Format::BC1RGBASRGBBlock: format = GraphicsFormat::BC1RGBASRGBBlock; break;
		case Format::BC3UNormBlock: format = GraphicsFormat::BC3UNormBlock; break;
		case Format::BC3SRGBBlock: format = GraphicsFormat::BC3SRGBBlock; break;
		case Format::BC4UNormBlock: format = GraphicsFormat::BC4UNormBlock; break;
		case Format::BC4SNormBlock: format = GraphicsFormat::BC4SNormBlock; break;
		case Format::BC5UNormBlock: format = GraphicsFormat::BC5UNormBlock; break;
		case Format::BC5SNormBlock: format = GraphicsFormat::BC5SNormBlock; break;
		case Format::BC6HUFloatBlock: format = GraphicsFormat::BC6HUFloatBlock; break;
		case Format::BC6HSFloatBlock: format = GraphicsFormat::BC6HSFloatBlock; break;
		case Format::BC7UNormBlock: format = GraphicsFormat::BC7UNormBlock; break;
		case Format::BC7SRGBBlock: format = GraphicsFormat::BC7SRGBBlock; break;
		case Format::R8G8B8UNorm: format = GraphicsFormat::R8G8B8UNorm; break;
		case Format::R8G8B8SRGB: format = GraphicsFormat::R8G8B8UNorm; break;
		case Format::R8G8B8A8UNorm: format = GraphicsFormat::R8G8B8A8UNorm; break;
		case Format::R8G8B8A8SRGB: format = GraphicsFormat::R8G8B8A8UNorm; break;
		case Format::B8G8R8UNorm: format = GraphicsFormat::B8G8R8UNorm; break;
		case Format::B8G8R8SRGB: format = GraphicsFormat::B8G8R8UNorm; break;
		case Format::B8G8R8A8UNorm: format = GraphicsFormat::B8G8R8A8UNorm; break;
		case Format::B8G8R8A8SRGB: format = GraphicsFormat::B8G8R8A8UNorm; break;
		case Format::R8UNorm: format = GraphicsFormat::R8UNorm; break;
		case Format::R8SRGB: format = GraphicsFormat::R8UNorm; break;
		case Format::R8G8UNorm: format = GraphicsFormat::R8G8UNorm; break;
		case Format::R8G8SRGB: format = GraphicsFormat::R8G8UNorm; break;
		case Format::R16SFloat: format = GraphicsFormat::R16SFloat; break;
		case Format::R16G16SFloat: format = GraphicsFormat::R16G16SFloat; break;
		case Format::R16G16B16SFloat: format = GraphicsFormat::R16G16B16SFloat; break;
		case Format::R16G16B16A16SFloat: format = GraphicsFormat::R16G16B16A16SFloat; break;
		case Format::R32SFloat: format = GraphicsFormat::R32SFloat; break;
		case Format::R32G32SFloat: format = GraphicsFormat::R32G32SFloat; break;
		case Format::R32G32B32SFloat: format = GraphicsFormat::R32G32B32SFloat; break;
		case Format::R32G32B32A32SFloat: format = GraphicsFormat::R32G32B32A32SFloat; break;
		default:
			throw runtime::runtime_error::create("This image type is not supported by this function:" + path);
		}

		GraphicsTextureDesc textureDesc;
		textureDesc.setSize(image.width(), image.height(), image.depth());
		textureDesc.setTexDim(GraphicsTextureDim::Texture2D);
		textureDesc.setTexFormat(format);
		textureDesc.setStream(image.data());
		textureDesc.setStreamSize(image.size());
		textureDesc.setMipBase(image.mipBase());
		textureDesc.setMipNums(image.mipLevel());
		textureDesc.setLayerBase(image.layerBase());
		textureDesc.setLayerNums(image.layerLevel());

		auto texture = RenderSystem::instance()->createTexture(textureDesc);
		if (!texture)
			return nullptr;

		if (cache)
			textureCaches_[path] = texture;

		return texture;
	}
}