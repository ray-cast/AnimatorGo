#include <octoon/game_prefabs.h>
#include <octoon/hal/graphics.h>
#include <octoon/image/image.h>
#include <octoon/io/vstream.h>

#include <octoon/video/render_system.h>
#include <octoon/video/ggx_material.h>
#include <octoon/video/basic_material.h>
#include <octoon/video/line_material.h>

#include <octoon/model/model.h>
#include <octoon/model/material.h>
#include <octoon/model/text_meshing.h>
#include <octoon/model/ik.h>
#include <octoon/model/rigidbody.h>

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
#include <octoon/skinned_joint_renderer_component.h>
#include <octoon/guizmo_component.h>
#include <octoon/rotation_limit_component.h>
#include <octoon/rigidbody_component.h>
#include <octoon/sphere_collider_component.h>
#include <octoon/box_collider_component.h>
#include <octoon/capsule_collider_component.h>
#include <octoon/configurable_joint_component.h>
#include <octoon/offline_mesh_renderer_component.h>
#include <octoon/offline_skinned_mesh_renderer_component.h>
#include <octoon/rotation_link_component.h>
#include <octoon/rotation_link_limit_component.h>

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
		Model model(path);

		GameObjects bones;
		if (!this->createBones(model, bones))
			return false;

		if (!this->createSolver(model, bones))
			return false;

		GameObjects rigidbodys;
		if (!this->createRigidbodys(model, bones, rigidbodys))
			return false;

		GameObjectPtr actor;
		if (!this->createMeshes(model, actor, bones, path))
			return false;

		return actor;
	}

	GameObjectPtr
	GamePrefabs::createOfflineModel(const std::string& path, bool cache) except
	{
		Model model(path);

		GameObjects bones;
		if (!this->createBones(model, bones))
			return false;

		if (!this->createSolver(model, bones))
			return false;

		GameObjects rigidbodys;
		if (!this->createRigidbodys(model, bones, rigidbodys))
			return false;

		GameObjectPtr actor;
		if (!this->createOfflineMeshes(model, actor, bones, path))
			return false;

		return actor;
	}

	bool
	GamePrefabs::createBones(const Model& model, GameObjects& bones) noexcept
	{
		auto material = std::make_shared<video::BasicMaterial>();
		material->setBaseColor(math::float4(0.4, 0.9, 0.4, 1.0));

		for (auto& it : model.get<Model::bone>())
		{
			auto object = GameObject::create(it->getName());
			/*object->addComponent<MeshFilterComponent>(model::makeCube(0.2f, 0.2f, 0.2f));
			object->addComponent<MeshRendererComponent>(material);*/

			bones.emplace_back(object);
		}

		for (std::size_t i = 0; i < model.get<Model::bone>().size(); i++)
		{
			auto it = model.get<Model::bone>(i);
			auto parent = it->getParent();
			bones[i]->setParent(parent >= 0 && parent < bones.size() ? bones[parent] : nullptr);
			bones[i]->getComponent<TransformComponent>()->setTranslate(it->getPosition());

			auto additiveParent = it->getAdditiveParent();
			if (additiveParent >= 0 && additiveParent < bones.size())
			{
				bones[i]->addComponent<RotationLinkLimitComponent>(it->getAdditiveMoveRatio(), it->getAdditiveRotationRatio(), it->getAdditiveUseLocal());

				auto parentController = bones[additiveParent]->getComponent<RotationLinkComponent>();
				if (parentController)
					parentController->addBone(bones[i]);
				else
					bones[additiveParent]->addComponent<RotationLinkComponent>(bones[i]);
			}
		}

		return true;
	}

	bool
	GamePrefabs::createRigidbodys(const model::Model& model, GameObjects& bones, GameObjects& rigidbodys) noexcept
	{
		for (auto& it : model.get<Model::rigidbody>())
		{
			auto gameObject = std::make_shared<GameObject>();
			gameObject->setName(it->name);
			gameObject->setLayer(it->group);
			gameObject->setParent(bones[it->bone]);
			gameObject->getComponent<TransformComponent>()->setTranslate(it->position);
			gameObject->getComponent<TransformComponent>()->setQuaternion(math::Quaternion(it->rotation));

			if (it->shape == ShapeType::ShapeTypeCircle)
				gameObject->addComponent(std::make_shared<SphereColliderComponent>(it->scale.x));
			else if (it->shape == ShapeType::ShapeTypeSquare)
				gameObject->addComponent(std::make_shared<BoxColliderComponent>(it->scale));
			else if (it->shape == ShapeType::ShapeTypeCapsule)
				gameObject->addComponent(std::make_shared<CapsuleColliderComponent>(it->scale.x, it->scale.y));

			auto component = std::make_shared<RigidbodyComponent>();
			component->setName(it->name);
			component->setMass(it->mass);
			component->setRestitution(it->elasticity);
			component->setStaticFriction(it->friction);
			component->setDynamicFriction(it->friction);
			//component->setLinearDamping(it->movementDecay);
			//component->setAngularDamping(it->rotationDecay);
			component->setIsKinematic(it->physicsOperation == 0);

			gameObject->addComponent(component);

			rigidbodys.push_back(std::move(gameObject));
		}

		return true;
	}

	bool 
	GamePrefabs::createSolver(const model::Model& model, GameObjects& bones) noexcept
	{
		for (auto& it : model.get<Model::ik>())
		{
			auto iksolver = std::make_shared<CCDSolverComponent>();
			iksolver->setTarget(bones[it->targetBoneIndex]);
			iksolver->setIterations(it->iterations);

			for (auto& child : it->child)
			{
				if (child.rotateLimited)
					bones[child.boneIndex]->addComponent<RotationLimitComponent>(-child.angleRadian, child.angleRadian, child.minimumRadian, child.maximumRadian);

				iksolver->addBone(bones[child.boneIndex]);
			}

			bones[it->boneIndex]->addComponent(std::move(iksolver));
		}

		return true;
	}

	bool
	GamePrefabs::createMeshes(const model::Model& model, GameObjectPtr& actor, const GameObjects& bones, const std::string& path) noexcept
	{
		video::Materials materials;
		if (!this->createMaterials(model, materials, "file:" + runtime::string::directory(path)))
			return false;

		actor = GameObject::create(runtime::string::filename(path.c_str()));
		actor->addComponent<AnimatorComponent>(bones);

		for (std::size_t i = 0; i < model.get<Model::material>().size(); i++)
		{
			auto mesh = model.get<Model::mesh>(i);

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
				smr->setMaterial(materials[i]);
				smr->setTransforms(bones);

				auto mat = std::make_shared<LineMaterial>(1.0f);
				mat->setColor(math::float3(0.4, 0.9, 0.4));

				auto sjr = std::make_shared<SkinnedJointRendererComponent>();
				sjr->setMaterial(mat);
				sjr->setTransforms(bones);

				object->addComponent(smr);
				object->addComponent(sjr);
			}
		}

		return true;
	}

	bool
	GamePrefabs::createOfflineMeshes(const model::Model& model, GameObjectPtr& actor, const GameObjects& bones, const std::string& path) noexcept
	{
		video::Materials materials;
		if (!this->createMaterials(model, materials, "file:" + runtime::string::directory(path)))
			return false;

		actor = GameObject::create(runtime::string::filename(path.c_str()));
		actor->addComponent<AnimatorComponent>(bones);

		for (std::size_t i = 0; i < model.get<Model::material>().size(); i++)
		{
			auto mesh = model.get<Model::mesh>(i);

			auto object = GameObject::create(mesh->getName());
			object->setParent(actor);
			object->addComponent<MeshFilterComponent>(mesh);

			if (bones.empty())
				object->addComponent<OfflineMeshRendererComponent>(materials[i]);
			else
				object->addComponent<OfflineSkinnedMeshRendererComponent>(materials[i], bones);
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
			it->get(MATKEY_TEXTURE_DIFFUSE, textureName);
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
	GamePrefabs::createTexture(const std::string& path, bool cache) noexcept
	{
		assert(!path.empty());

		try
		{
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
			textureDesc.setName(path);
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
		catch (...)
		{
			return nullptr;
		}
	}
}