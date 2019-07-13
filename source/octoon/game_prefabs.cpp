#include <octoon/game_prefabs.h>
#include <octoon/hal/graphics.h>
#include <octoon/image/image.h>
#include <octoon/io/vstream.h>

#include <octoon/video/render_system.h>
#include <octoon/video/ggx_material.h>
#include <octoon/video/basic_material.h>

#include <octoon/model/model.h>
#include <octoon/model/property.h>
#include <octoon/model/text_meshing.h>
#include <octoon/model/ik.h>

#include <octoon/camera_component.h>
#include <octoon/ortho_camera_component.h>
#include <octoon/perspective_camera_component.h>
#include <octoon/film_camera_component.h>
#include <octoon/transform_component.h>
#include <octoon/first_person_camera_component.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/text_component.h>
#include <octoon/solver_component.h>
#include <octoon/animator_component.h>
#include <octoon/skinned_mesh_renderer_component.h>

#include <octoon/runtime/except.h>
#include <octoon/runtime/string.h>

using namespace octoon::io;
using namespace octoon::image;
using namespace octoon::hal;
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
	GamePrefabs::createOrthoCamera() except
	{
		auto camera = GameObject::create("MainCamera");
		camera->addComponent<OrthoCameraComponent>(0.0f, 1.0f, 0.0f, 1.0f);
		camera->getComponent<OrthoCameraComponent>()->setCameraType(video::CameraType::Main);
		camera->getComponent<TransformComponent>()->setTranslate(math::float3(0.0f, 0.0f, -10.0f));
		camera->addComponent<FirstPersonCameraComponent>();

		return camera;
	}

	GameObjectPtr
	GamePrefabs::createPerspectiveCamera(float fov) except
	{
		auto object = GameObject::create("MainCamera");
		object->getComponent<TransformComponent>()->setTranslate(math::float3(0, 1, -10));
		object->addComponent<FirstPersonCameraComponent>();

		auto camera = object->addComponent<PerspectiveCameraComponent>(fov);
		camera->setCameraType(video::CameraType::Main);

		return object;
	}

	GameObjectPtr
	GamePrefabs::createFilmCamera(float canvasWidth, float zoom) except
	{
		auto object = GameObject::create("MainCamera");
		object->getComponent<TransformComponent>()->setTranslate(math::float3(0, 1, -10));
		object->addComponent<FirstPersonCameraComponent>();

		auto camera = object->addComponent<FilmCameraComponent>();
		camera->setCameraType(video::CameraType::Main);
		camera->setCanvasWidth(canvasWidth);
		camera->setZoom(zoom);

		return object;
	}

	GameObjectPtr
	GamePrefabs::createCircle(float radius, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeCircle(radius, segments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createPlane(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createPlane(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane(width, height, depth, widthSegments, heightSegments, depthSegments, u, v, udir, vdir));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createPlaneWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments, std::uint8_t u, std::uint8_t v, float udir, float vdir, bool clear) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlaneWireframe(width, height, depth, widthSegments, heightSegments, depthSegments, u, v, udir, vdir, clear));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createFloor(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeFloor(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createNoise(float width, float height, std::uint32_t widthSegments, std::uint32_t heightSegments) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeNoise(width, height, widthSegments, heightSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createCube(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeCube(width, height, depth, widthSegments, heightSegments, depthSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createCubeWireframe(float width, float height, float depth, std::uint32_t widthSegments, std::uint32_t heightSegments, std::uint32_t depthSegments) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeCubeWireframe(width, height, depth, widthSegments, heightSegments, depthSegments));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createRing(float innerRadius, float outerRadius, std::uint32_t thetaSegments, std::uint32_t phiSegments, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeRing(innerRadius, outerRadius, thetaSegments, phiSegments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createSphere(float radius, std::uint32_t widthSegments, std::uint32_t heightSegments, float phiStart, float phiLength, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeSphere(radius, widthSegments, heightSegments, phiStart, phiLength, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createVolumes(float fovy, float znear, float zfar) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeVolumes(fovy, znear, zfar));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createCone(float radius, float height, std::uint32_t segments, float thetaStart, float thetaLength) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeCone(radius, height, segments, thetaStart, thetaLength));
		object->addComponent<MeshRendererComponent>(std::make_shared<GGXMaterial>());
		return object;
	}

	GameObjectPtr
	GamePrefabs::createText(const wchar_t* text, std::uint16_t fontsize, const char* fontPath) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makeMesh(model::makeTextContours(text, { fontPath, fontsize })));
		object->addComponent<MeshRendererComponent>(std::make_shared<BasicMaterial>());

		return object;
	}

	GameObjectPtr
	GamePrefabs::createText(const char* u8str, std::uint16_t fontsize, const char* fontPath) noexcept
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<TextComponent>(u8str)->setTextMeshing(std::make_shared<TextMeshing>(fontPath, fontsize));
		object->addComponent<MeshRendererComponent>(std::make_shared<BasicMaterial>());

		return object;
	}

	GameObjectPtr
	GamePrefabs::createModel(const std::string& path, bool cache) except
	{
		auto it = prefabs_.find(path);
		if (it != prefabs_.end())
			return (*it).second->clone();

		auto stream = io::ifstream(path);
		if (!stream)
			return nullptr;

		Model model(path);

		GameObjects bones;
		if (!this->createBones(model, bones))
			return false;

		video::Materials materials;
		if (!this->createMaterials(model, materials, "file:" + runtime::string::directory(path)))
			return false;

		math::float4x4s bindposes;
		for (auto& bone : model.get<Model::bone>())
		{
			float4x4 bindpose;
			bindpose.makeTranslate(-bone->getPosition());
			bindposes.push_back(bindpose);
		}

		auto actor = GameObject::create(runtime::string::filename(path.c_str()));
		actor->getComponent<TransformComponent>()->setQuaternion(math::Quaternion(math::float3::UnitZ, math::radians(180)));
		actor->addComponent<AnimatorComponent>(bones);

		for (std::size_t i = 0; i < model.get<Model::material>().size(); i++)
		{
			auto mesh = model.get<Model::mesh>(i);
			mesh->setBindposes(bindposes);

			auto object = GameObject::create(mesh->getName());
			object->setParent(actor);
			object->addComponent<MeshFilterComponent>(mesh);

			if (bones.empty())
			{
				object->addComponent<MeshRendererComponent>(materials[i]);
			}
			else
			{
				auto smr = std::make_shared<SkinnedMeshRendererComponent>();
				smr->setMaterial(std::move(materials[i]));
				smr->setTransforms(bones);

				object->addComponent(smr);
			}
		}

		prefabs_[path] = actor;

		return actor;
	}

	bool
	GamePrefabs::createBones(const Model& model, GameObjects& bones) noexcept
	{
		for (auto& it : model.get<Model::bone>())
			bones.push_back(std::make_shared<GameObject>(it->getName()));

		for (std::size_t i = 0; i < model.get<Model::bone>().size(); i++)
		{
			auto it = model.get<Model::bone>(i);
			auto parent = it->getParent();
			bones[i]->setParent(parent >= 0 && parent < bones.size() ? bones[parent] : nullptr);
			bones[i]->getComponent<TransformComponent>()->setTranslate(it->getPosition());
		}

		for (auto& it : model.get<Model::ik>())
		{
			auto iksolver = std::make_shared<SolverComponent>();
			iksolver->setTargetBone(bones[it->targetBoneIndex]);
			iksolver->setIterations(it->iterations);
			iksolver->setChainLength(it->chainLength);

			for (auto& child : it->child)
			{
				auto bone = std::make_shared<SolverTarget>();
				bone->bone = bones[child.boneIndex];
				bone->angleWeight = child.angleWeight;
				bone->rotateLimited = child.rotateLimited;
				bone->minimumDegrees = child.minimumDegrees;
				bone->maximumDegrees = child.maximumDegrees;

				iksolver->addBone(bone);
			}

			bones[it->boneIndex]->addComponent(std::move(iksolver));
		}

		return true;
	}

	bool
	GamePrefabs::createMaterials(const model::Model& model, video::Materials& materials, const std::string& rootPath) noexcept
	{
		for (auto& it : model.get<Model::material>())
		{
			std::string name;
			std::string textureName;

			math::float3 base;
			math::float3 ambient;

			it->get(MATKEY_NAME, name);
			it->get(MATKEY_TEXTURE_DIFFUSE(0), textureName);
			it->get(MATKEY_COLOR_DIFFUSE, base);			
			it->get(MATKEY_COLOR_AMBIENT, ambient);

			auto material = std::make_shared<video::BasicMaterial>();
			material->setBaseColor(math::float4(base, 1.0));

			if (!textureName.empty())
				material->setTexture(GamePrefabs::instance()->createTexture(rootPath + textureName));

			materials.push_back(material);
		}

		return true;
	}

	GameObjectPtr
	GamePrefabs::createSprite(const std::string& image, float w, float h) except
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane(w, h));
		object->addComponent<MeshRendererComponent>(std::make_shared<BasicMaterial>(this->createTexture(image)));

		return object;
	}

	GameObjectPtr
	GamePrefabs::createSprite(const hal::GraphicsTexturePtr& texture) except
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane((float)texture->getTextureDesc().getWidth(), (float)texture->getTextureDesc().getHeight()));
		object->addComponent<MeshRendererComponent>(std::make_shared<BasicMaterial>(texture));

		return object;
	}

	GameObjectPtr
	GamePrefabs::createSprite(const hal::GraphicsTexturePtr& texture, float w, float h) except
	{
		auto object = GameObject::create("GameObject");
		object->addComponent<MeshFilterComponent>(model::makePlane(w, h));
		object->addComponent<MeshRendererComponent>(std::make_shared<BasicMaterial>(texture));

		return object;
	}

	GameObjectPtr
	GamePrefabs::createSpriteSquare(float w, float h) except
	{
		return this->createSprite("sys:sprite/square.png", w, h);
	}

	hal::GraphicsTexturePtr
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