#ifndef OCTOON_MODEL_H_
#define OCTOON_MODEL_H_

#include <octoon/model/model_loader.h>

namespace octoon
{
	namespace model
	{
		class OCTOON_EXPORT Model final
		{
		public:
			using mesh_array_t = std::vector<MeshPtr>;
			using bone_array_t = std::vector<BonePtr>;
			using ik_array_t = std::vector<IKAttrPtr>;
			using rigidbody_array_t = std::vector<RigidbodyPtr>;
			using joint_array_t = std::vector<JointPtr>;
			using material_array_t = std::vector<MaterialPtr>;
			using texture_array_t = std::vector<TexturePtr>;
			using animation_array_t = std::vector<AnimationPtr>;
			using light_array_t = std::vector<LightPropertyPtr>;
			using camera_array_t = std::vector<CameraPropertyPtr>;

			enum type_t
			{
				mesh,
				bone,
				ik,
				rigidbody,
				joint,
				material,
				texture,
				animation,
				light,
				camera
			};

		public:
			Model() noexcept;
			Model(istream& stream, const char* type = nullptr) noexcept;
			Model(const char* filepath, const char* type = nullptr) noexcept;
			Model(const std::string& filepath, const char* type = nullptr) noexcept;
			~Model() noexcept;

			void add(const MeshPtr& value) noexcept { _meshes.push_back(value); }
			void add(const BonePtr& value) noexcept { _bones.push_back(value); }
			void add(const IKAttrPtr& value) noexcept { _iks.push_back(value); }
			void add(const RigidbodyPtr& value) noexcept { _rigidbodys.push_back(value); }
			void add(const JointPtr& value) noexcept { _joints.push_back(value); }
			void add(const TexturePtr& value) noexcept { _textures.push_back(value); }
			void add(const MaterialPtr& value) noexcept { _materials.push_back(value); }
			void add(const AnimationPtr& value) noexcept { _animations.push_back(value); }
			void add(const LightPropertyPtr& value) noexcept { _lights.push_back(value); }
			void add(const CameraPropertyPtr& value) noexcept { _cameras.push_back(value); }

			void add(MeshPtr&& value) noexcept { _meshes.push_back(value); }
			void add(BonePtr&& value) noexcept { _bones.push_back(value); }
			void add(IKAttrPtr&& value) noexcept { _iks.push_back(value); }
			void add(RigidbodyPtr&& value) noexcept { _rigidbodys.push_back(value); }
			void add(JointPtr&& value) noexcept { _joints.push_back(value); }
			void add(TexturePtr&& value) noexcept { _textures.push_back(value); }
			void add(MaterialPtr&& value) noexcept { _materials.push_back(value); }
			void add(AnimationPtr&& value) noexcept { _animations.push_back(value); }
			void add(LightPropertyPtr&& value) noexcept { _lights.push_back(value); }
			void add(CameraPropertyPtr&& value) noexcept { _cameras.push_back(value); }

			void set(mesh_array_t& value) noexcept { _meshes = value; }
			void set(bone_array_t& value) noexcept { _bones = value; }
			void set(ik_array_t& value) noexcept { _iks = value; }
			void set(rigidbody_array_t& value) noexcept { _rigidbodys = value; }
			void set(joint_array_t& value) noexcept { _joints = value; }
			void set(material_array_t& value) noexcept { _materials = value; }
			void set(texture_array_t& value) noexcept { _textures = value; }
			void set(animation_array_t& value) noexcept { _animations = value; }
			void set(light_array_t& value) noexcept { _lights = value; }
			void set(camera_array_t& value) noexcept { _cameras = value; }

			void set(mesh_array_t&& value) noexcept { _meshes = std::move(value); }
			void set(bone_array_t&& value) noexcept { _bones = std::move(value); }
			void set(ik_array_t&& value) noexcept { _iks = std::move(value); }
			void set(rigidbody_array_t&& value) noexcept { _rigidbodys = std::move(value); }
			void set(joint_array_t&& value) noexcept { _joints = std::move(value); }
			void set(material_array_t&& value) noexcept { _materials = std::move(value); }
			void set(texture_array_t&& value) noexcept { _textures = std::move(value); }
			void set(animation_array_t&& value) noexcept { _animations = std::move(value); }
			void set(light_array_t&& value) noexcept { _lights = std::move(value); }
			void set(camera_array_t&& value) noexcept { _cameras = std::move(value); }

			template<type_t type, typename = std::enable_if_t<type == type_t::mesh>>
			const mesh_array_t& get() const { return _meshes; }

			template<type_t type, typename = std::enable_if_t<type == type_t::bone>>
			const bone_array_t& get() const { return _bones; }

			template<type_t type, typename = std::enable_if_t<type == type_t::ik>>
			const ik_array_t& get() const { return _iks; }

			template<type_t type, typename = std::enable_if_t<type == type_t::rigidbody>>
			const rigidbody_array_t& get() const { return _rigidbodys; }

			template<type_t type, typename = std::enable_if_t<type == type_t::joint>>
			const joint_array_t& get() const { return _joints; }

			template<type_t type, typename = std::enable_if_t<type == type_t::material>>
			const material_array_t& get() const { return _materials; }

			template<type_t type, typename = std::enable_if_t<type == type_t::texture>>
			const texture_array_t& get() const { return _textures; }

			template<type_t type, typename = std::enable_if_t<type == type_t::animation>>
			const animation_array_t& get() const { return _animations; }

			template<type_t type, typename = std::enable_if_t<type == type_t::light>>
			const light_array_t& get() const { return _lights; }

			template<type_t type, typename = std::enable_if_t<type == type_t::camera>>
			const camera_array_t& get() const { return _cameras; }

			template<type_t type, typename = std::enable_if_t<type == type_t::mesh>>
			const MeshPtr& get(std::size_t n) const { return _meshes[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::bone>>
			const BonePtr& get(std::size_t n) const { return _bones[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::ik>>
			const IKAttrPtr& get(std::size_t n) const { return _iks[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::rigidbody>>
			const RigidbodyPtr& get(std::size_t n) const { return _rigidbodys[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::joint>>
			const JointPtr& get(std::size_t n) const { return _joints[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::material>>
			const MaterialPtr& get(std::size_t n) const { return _materials[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::texture>>
			const TexturePtr& get(std::size_t n) const { return _textures[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::animation>>
			const AnimationPtr& get(std::size_t n) const { return _animations[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::light>>
			const LightPropertyPtr& get(std::size_t n) const { return _lights[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::camera>>
			const CameraPropertyPtr& get(std::size_t n) const { return _cameras[n]; }

			template<type_t type>
			constexpr std::enable_if_t<type == type_t::mesh, bool> empty() const { return _meshes.empty(); }

			template<type_t type>
			constexpr std::enable_if_t<type == type_t::bone, bool> empty() const { return _bones.empty(); }

			template<type_t type>
			constexpr std::enable_if_t<type == type_t::ik, bool> empty() const { return _iks.empty(); }

			template<type_t type>
			constexpr std::enable_if_t<type == type_t::rigidbody, bool> empty() const { return _rigidbodys.empty(); }

			template<type_t type>
			constexpr std::enable_if_t<type == type_t::joint, bool> empty() const { return _joints.empty(); }

			template<type_t type>
			constexpr std::enable_if_t<type == type_t::material, bool> empty() const { return _materials.empty(); }

			template<type_t type>
			constexpr std::enable_if_t<type == type_t::texture, bool> empty() const { return _textures.empty(); }

			template<type_t type>
			constexpr std::enable_if_t<type == type_t::animation, bool> empty() const { return _animations.empty(); }

			template<type_t type>
			constexpr std::enable_if_t<type == type_t::light, bool> empty() const { return _lights.empty(); }

			template<type_t type>
			constexpr std::enable_if_t<type == type_t::camera, bool> empty() const { return _cameras.empty(); }

			template<type_t type, typename = std::enable_if_t<type == type_t::mesh>>
			const MeshPtr& operator[](std::size_t n) const { return _meshes[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::bone>>
			const BonePtr& operator[](std::size_t n) const { return _bones[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::ik>>
			const IKAttrPtr& operator[](std::size_t n) const { return _iks[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::rigidbody>>
			const RigidbodyPtr& operator[](std::size_t n) const { return _rigidbodys[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::joint>>
			const JointPtr& operator[](std::size_t n) const { return _joints[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::material>>
			const MaterialPtr& operator[](std::size_t n) const { return _materials[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::texture>>
			const TexturePtr& operator[](std::size_t n) const { return _textures[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::animation>>
			const AnimationPtr& operator[](std::size_t n) const { return _animations[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::light>>
			const LightPropertyPtr& operator[](std::size_t n) const { return _lights[n]; }

			template<type_t type, typename = std::enable_if_t<type == type_t::camera>>
			const CameraPropertyPtr& operator[](std::size_t n) const { return _cameras[n]; }

			void clear() noexcept;

		public:
			bool load(istream& stream, const char* type = nullptr) noexcept;
			bool load(const char* filepath, const char* type = nullptr) noexcept;
			bool load(const std::string& filepath, const char* type = nullptr) noexcept;

			bool save(ostream& stream, const char* type = "obj") noexcept;
			bool save(const char* filepath, const char* type = "obj") noexcept;
			bool save(const std::string& filepath, const char* type = "obj") noexcept;

		private:
			Model(const Model&) noexcept = delete;
			Model& operator=(const Model&) noexcept = delete;

		private:
			std::string _name;

			mesh_array_t _meshes;
			bone_array_t _bones;
			ik_array_t _iks;
			rigidbody_array_t _rigidbodys;
			joint_array_t _joints;
			texture_array_t _textures;
			material_array_t _materials;
			animation_array_t _animations;
			light_array_t _lights;
			camera_array_t _cameras;
		};
	}
}
#endif
