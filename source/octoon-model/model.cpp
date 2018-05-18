#include <octoon/model/model.h>
#include <octoon/model/modall.h>

#include <octoon/io/fstream.h>

namespace octoon
{
	namespace model
	{
		Model::Model() noexcept
		{
		}

		Model::~Model() noexcept
		{
			this->clear();
		}

		void Model::clear() noexcept
		{
			_name.clear();
			_meshes.clear();
			_textures.clear();
			_materials.clear();
			_animations.clear();
			_lights.clear();
			_cameras.clear();
		}

		void Model::addMesh(MeshPtr& mesh) noexcept
		{
			_meshes.push_back(mesh);
		}

		void Model::addBone(BonePtr& bone) noexcept
		{
			_bones.push_back(bone);
		}

		void Model::addIK(IKAttrPtr& ik) noexcept
		{
			_iks.push_back(ik);
		}

		void Model::addRigidbody(RigidbodyPropertyPtr& body) noexcept
		{
			_rigidbodys.push_back(body);
		}

		void Model::addJoint(JointPropertyPtr& joint) noexcept
		{
			_joints.push_back(joint);
		}

		void Model::addTexture(TexturePropertyPtr& texture) noexcept
		{
			_textures.push_back(texture);
		}

		void Model::addMaterial(MaterialPropertyPtr& mat) noexcept
		{
			_materials.push_back(mat);
		}

		void Model::addAnimtion(AnimationPropertyPtr& anim) noexcept
		{
			_animations.push_back(anim);
		}

		void Model::addLight(LightPropertyPtr& light) noexcept
		{
			_lights.push_back(light);
		}

		void Model::addCamera(CameraPropertyPtr& camera) noexcept
		{
			_cameras.push_back(camera);
		}

		void Model::addMesh(MeshPtr&& mesh) noexcept
		{
			_meshes.push_back(std::move(mesh));
		}

		void Model::addBone(BonePtr&& bone) noexcept
		{
			_bones.push_back(std::move(bone));
		}

		void Model::addIK(IKAttrPtr&& ik) noexcept
		{
			_iks.push_back(std::move(ik));
		}

		void Model::addRigidbody(RigidbodyPropertyPtr&& body) noexcept
		{
			_rigidbodys.push_back(std::move(body));
		}

		void Model::addJoint(JointPropertyPtr&& joint) noexcept
		{
			_joints.push_back(std::move(joint));
		}

		void Model::addTexture(TexturePropertyPtr&& texture) noexcept
		{
			_textures.push_back(std::move(texture));
		}

		void Model::addMaterial(MaterialPropertyPtr&& mat) noexcept
		{
			_materials.push_back(std::move(mat));
		}

		void Model::addAnimtion(AnimationPropertyPtr&& anim) noexcept
		{
			_animations.push_back(std::move(anim));
		}

		void Model::addLight(LightPropertyPtr&& light) noexcept
		{
			_lights.push_back(std::move(light));
		}

		void Model::addCamera(CameraPropertyPtr&& camera) noexcept
		{
			_cameras.push_back(std::move(camera));
		}

		Model::MeshList& Model::getMeshsList() noexcept
		{
			return _meshes;
		}

		Model::BoneList& Model::getBonesList() noexcept
		{
			return _bones;
		}

		Model::IKList& Model::getIKList() noexcept
		{
			return _iks;
		}

		Model::RigidbodyList& Model::getRigidbodyList() noexcept
		{
			return _rigidbodys;
		}

		Model::JointList& Model::getJointList() noexcept
		{
			return _joints;
		}

		Model::TextureList& Model::getTexturesList() noexcept
		{
			return _textures;
		}

		Model::MaterialList& Model::getMaterialsList() noexcept
		{
			return _materials;
		}

		Model::AnimList& Model::getAnimationList() noexcept
		{
			return _animations;
		}

		Model::LightList& Model::getLightList() noexcept
		{
			return _lights;
		}

		Model::CameraList& Model::getCameraList() noexcept
		{
			return _cameras;
		}

		const std::string& Model::getName() const noexcept
		{
			return _name;
		}

		void Model::setDirectory(const std::string& name) noexcept
		{
			_directory = name;
		}

		const std::string& Model::getDirectory() const noexcept
		{
			return _directory;
		}

		const Model::MeshList& Model::getMeshsList() const noexcept
		{
			return _meshes;
		}

		const Model::BoneList& Model::getBonesList() const noexcept
		{
			return _bones;
		}

		const Model::IKList& Model::getIKList() const noexcept
		{
			return _iks;
		}

		const Model::RigidbodyList& Model::getRigidbodyList() const noexcept
		{
			return _rigidbodys;
		}

		const Model::JointList& Model::getJointList() const noexcept
		{
			return _joints;
		}

		const Model::TextureList& Model::getTexturesList() const noexcept
		{
			return _textures;
		}

		const Model::MaterialList& Model::getMaterialsList() const noexcept
		{
			return _materials;
		}

		const Model::AnimList& Model::getAnimationList() const noexcept
		{
			return _animations;
		}

		const Model::LightList& Model::getLightList() const noexcept
		{
			return _lights;
		}

		const Model::CameraList& Model::getCameraList() const noexcept
		{
			return _cameras;
		}

		bool Model::hasMeshes() const noexcept
		{
			return !_meshes.empty();
		}

		bool Model::hasBones() const noexcept
		{
			return !_bones.empty();
		}

		bool Model::hasIKs() const noexcept
		{
			return !_iks.empty();
		}

		bool Model::hasMaterials() const noexcept
		{
			return !_materials.empty();
		}

		bool Model::hasLights() const noexcept
		{
			return !_lights.empty();
		}

		bool Model::hasTextures() const noexcept
		{
			return !_textures.empty();
		}

		bool Model::hasCameras() const noexcept
		{
			return !_cameras.empty();
		}

		bool Model::hasAnimations() const noexcept
		{
			return !_animations.empty();
		}

		void Model::applyProcess(int) noexcept
		{
		}

		bool
		Model::load(istream& stream, const char* type) noexcept
		{
			if (stream.good())
			{
				ModelLoaderPtr impl = findHandler(stream, type);
				if (impl)
				{
					if (impl->doLoad(stream, *this))
						return true;
				}
			}

			return false;
		}

		bool
		Model::load(const char* filepath, const char* type) noexcept
		{
			io::ifstream stream(filepath);
			return this->load(stream, type);
		}

		bool
		Model::load(const std::string& filepath, const char* type) noexcept
		{
			io::ifstream stream(filepath);
			return this->load(stream, type);
		}

		bool
		Model::save(ostream& stream, const char* type) noexcept
		{
			if (stream.good())
			{
				ModelLoaderPtr impl = findHandler(type);
				if (impl)
				{
					if (impl->doSave(stream, *this))
						return true;
				}
			}

			return false;
		}

		bool
		Model::save(const char* filepath, const char* type) noexcept
		{
			io::ofstream stream(filepath, io::ios_base::in | io::ios_base::out);
			return this->save(stream, type);
		}

		bool
		Model::save(const std::string& filepath, const char* type) noexcept
		{
			io::ofstream stream(filepath, io::ios_base::in | io::ios_base::out);
			return this->save(stream, type);
		}
	}
}