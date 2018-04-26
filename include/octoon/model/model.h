#ifndef OCTOON_MODEL_H_
#define OCTOON_MODEL_H_

#include <octoon/io/fstream.h>
#include <octoon/model/modtypes.h>
#include <octoon/model/loader.h>

#include <string>

namespace octoon
{
	namespace model
	{
		class Model final
		{
		public:
			typedef std::shared_ptr<ModelLoader>      MyLoader;

			typedef std::vector<MeshPropertyPtr>      MeshList;
			typedef std::vector<BonePtr>              BoneList;
			typedef std::vector<IKAttrPtr>            IKList;
			typedef std::vector<RigidbodyPropertyPtr> RigidbodyList;
			typedef std::vector<JointPropertyPtr>     JointList;
			typedef std::vector<MaterialPropertyPtr>  MaterialList;
			typedef std::vector<TexturePropertyPtr>   TextureList;
			typedef std::vector<AnimationPropertyPtr> AnimList;
			typedef std::vector<LightPropertyPtr>     LightList;
			typedef std::vector<CameraPropertyPtr>    CameraList;

		public:
			Model() noexcept;
			~Model() noexcept;

			void addMesh(MeshPropertyPtr& mesh)            noexcept;
			void addBone(BonePtr& bone)					   noexcept;
			void addIK(IKAttrPtr& ik)                      noexcept;
			void addRigidbody(RigidbodyPropertyPtr& body)  noexcept;
			void addJoint(JointPropertyPtr& joint)         noexcept;
			void addTexture(TexturePropertyPtr& texture)   noexcept;
			void addMaterial(MaterialPropertyPtr& mat)     noexcept;
			void addAnimtion(AnimationPropertyPtr& anim)   noexcept;
			void addLight(LightPropertyPtr& light)         noexcept;
			void addCamera(CameraPropertyPtr& camera)      noexcept;

			void addMesh(MeshPropertyPtr&& mesh)           noexcept;
			void addBone(BonePtr&& bone)				   noexcept;
			void addIK(IKAttrPtr&& ik)                     noexcept;
			void addRigidbody(RigidbodyPropertyPtr&& body) noexcept;
			void addJoint(JointPropertyPtr&& joint)        noexcept;
			void addTexture(TexturePropertyPtr&& texture)  noexcept;
			void addMaterial(MaterialPropertyPtr&& mat)    noexcept;
			void addAnimtion(AnimationPropertyPtr&& anim)  noexcept;
			void addLight(LightPropertyPtr&& light)        noexcept;
			void addCamera(CameraPropertyPtr&& camera)     noexcept;

			MeshList&      getMeshsList()     noexcept;
			BoneList&      getBonesList()     noexcept;
			IKList&        getIKList()        noexcept;
			RigidbodyList& getRigidbodyList() noexcept;
			JointList&     getJointList()     noexcept;
			TextureList&   getTexturesList()  noexcept;
			MaterialList&  getMaterialsList() noexcept;
			AnimList&      getAnimationList() noexcept;
			LightList&     getLightList()     noexcept;
			CameraList&    getCameraList()    noexcept;

			void setDirectory(const std::string& name) noexcept;
			const std::string&  getDirectory() const noexcept;

			const std::string&   getName()          const noexcept;
			const MeshList&      getMeshsList()     const noexcept;
			const BoneList&      getBonesList()     const noexcept;
			const IKList&        getIKList()        const noexcept;
			const RigidbodyList& getRigidbodyList() const noexcept;
			const JointList&     getJointList()     const noexcept;
			const TextureList&   getTexturesList()  const noexcept;
			const MaterialList&  getMaterialsList() const noexcept;
			const AnimList&      getAnimationList() const noexcept;
			const LightList&     getLightList()     const noexcept;
			const CameraList&    getCameraList()    const noexcept;

			bool hasMeshes()     const noexcept;
			bool hasBones()      const noexcept;
			bool hasIKs()        const noexcept;
			bool hasMaterials()  const noexcept;
			bool hasLights()     const noexcept;
			bool hasTextures()   const noexcept;
			bool hasCameras()    const noexcept;
			bool hasAnimations() const noexcept;

			void applyProcess(int flags) noexcept;

			void clear() noexcept;

			bool load(io::fstream& file, const char* type = nullptr) noexcept;
			bool save(io::fstream& file, const char* type = "pmx") noexcept;

			bool emptyLoader() const noexcept;
			bool addLoader(MyLoader loader) noexcept;
			bool removeLoader(MyLoader loader) noexcept;
			bool find(const char* type, MyLoader& loader) const noexcept;
			bool find(io::istream& file, MyLoader& loader) const noexcept;
			bool find(io::istream& file, const char* type, MyLoader& loader) const noexcept;

		private:
			Model& operator=(const Model&) noexcept = delete;
			Model(const Model&) noexcept = delete;

		private:
			std::string _name;
			std::string _directory;
			MeshList _meshes;
			BoneList _bones;
			IKList _iks;
			RigidbodyList _rigidbodys;
			JointList _joints;
			TextureList _textures;
			MaterialList _materials;
			AnimList _animations;
			LightList _lights;
			CameraList _cameras;
			std::vector<MyLoader> _loaders;
		};
	}
}
#endif // !OCTOON_MODEL_H_
