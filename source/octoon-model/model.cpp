#include <octoon/model/model.h>
#include <octoon/model/modall.h>

#include <octoon/io/fstream.h>

#include <string>

namespace octoon
{
	namespace model
	{
		Model::Model() noexcept
		{
		}

		Model::Model(istream& stream, const char* type) noexcept
		{
			this->load(stream, type);
		}

		Model::Model(const char* filepath, const char* type) noexcept
		{
			this->load(filepath, type);
		}

		Model::Model(const std::string& filepath, const char* type) noexcept
		{
			this->load(filepath, type);
		}

		Model::~Model() noexcept
		{
			this->clear();
		}

		void
		Model::clear() noexcept
		{
			_name.clear();
			_meshes.clear();
			_bones.clear();
			_iks.clear();
			_rigidbodys.clear();
			_joints.clear();
			_textures.clear();
			_materials.clear();
			_animations.clear();
			_lights.clear();
			_cameras.clear();
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