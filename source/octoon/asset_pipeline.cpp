#include <octoon/asset_pipeline.h>
#include <octoon/asset_manager.h>
#include <octoon/runtime/md5.h>
#include <octoon/runtime/guid.h>
#include <octoon/texture_importer.h>
#include <octoon/vmd_importer.h>
#include <octoon/pmx_importer.h>
#include <octoon/obj_importer.h>
#include <octoon/ass_importer.h>
#include <octoon/fbx_importer.h>
#include <octoon/audio_importer.h>
#include <octoon/material_importer.h>
#include <octoon/prefab_importer.h>
#include <octoon/alembic_importer.h>
#include <octoon/asset_database.h>
#include <octoon/mesh_animation_component.h>

#include <fstream>

namespace octoon
{
	AssetPipeline::AssetPipeline(const std::u8string& name) noexcept
		: name_(name)
	{
	}

	AssetPipeline::~AssetPipeline() noexcept
	{
		this->close();
	}

	void
	AssetPipeline::open(const std::filesystem::path& diskPath) noexcept(false)
	{
		this->close();

		this->rootPath_ = diskPath;

		if (!diskPath.empty())
		{
			std::ifstream ifs(std::filesystem::path(diskPath).append("manifest.json"), std::ios_base::binary);
			if (ifs)
			{
				auto assetDb = nlohmann::json::parse(ifs);

				for (auto it = assetDb.begin(); it != assetDb.end(); ++it)
				{
					auto uuid = it.value();
					auto path = std::filesystem::path((char8_t*)it.key().c_str());
					assetPaths_.insert(path);
					AssetDatabase::instance()->importAsset(path);
				}
			}
		}
	}

	void
	AssetPipeline::close() noexcept
	{
		rootPath_.clear();
	}

	const std::u8string&
	AssetPipeline::getName() const noexcept
	{
		return name_;
	}

	void
	AssetPipeline::importAsset(const std::filesystem::path& diskPath, const std::filesystem::path& relativePath) noexcept(false)
	{
		auto rootPath = relativePath.parent_path();
		auto absolutePath = this->getAbsolutePath(relativePath);

		try
		{
			this->createFolder(rootPath);

			std::filesystem::copy_file(diskPath, absolutePath, std::filesystem::copy_options::overwrite_existing);
			std::filesystem::permissions(absolutePath, std::filesystem::perms::owner_write);

			this->assetPaths_.insert(relativePath);
			AssetManager::instance()->createMetadataAtPath(relativePath);

			auto ext = diskPath.extension().u8string();
			for (auto& it : ext)
				it = (char)std::tolower(it);

			if (ext == u8".pmx")
			{
				auto pmx = std::make_shared<octoon::PMX>();
				if (octoon::PMX::load(diskPath, *pmx))
				{
					std::map<std::filesystem::path, std::wstring> diskPaths;
					for (auto& texture : pmx->textures)
					{
						auto fullpath = diskPath.parent_path().append(texture.name);
						if (std::filesystem::exists(fullpath))
							diskPaths[fullpath] = texture.name;
					}

					for (auto& path : diskPaths)
						this->importAsset(path.first, std::filesystem::path(rootPath).append(path.second));
				}
			}
			else if (ext == u8".fbx")
			{
				auto dependencies = octoon::FBXImporter::getDependencies(diskPath);

				std::map<std::filesystem::path, std::wstring> diskPaths;
				for (auto& it : dependencies)
				{
					auto fullpath = std::filesystem::path(diskPath.parent_path()).append(it.wstring());
					if (std::filesystem::exists(fullpath))
						diskPaths[fullpath] = it.wstring();
				}

				for (auto& path : diskPaths)
					this->importAsset(path.first, std::filesystem::path(rootPath).append(path.second));
			}
		}
		catch (const std::exception& e)
		{
			if (std::filesystem::exists(absolutePath))
				std::filesystem::remove(absolutePath);

			if (std::filesystem::exists(absolutePath.u8string() + u8".meta"))
				std::filesystem::remove(absolutePath.u8string() + u8".meta");

			throw e;
		}
	}

	void
	AssetPipeline::createAsset(const std::shared_ptr<const Texture>& asset, const std::filesystem::path& relativePath) noexcept(false)
	{
		assert(asset && !relativePath.empty());

		try
		{
			auto absolutePath = this->getAbsolutePath(relativePath);
			auto extension = absolutePath.extension().u8string();

			if (asset->save(absolutePath, (char*)extension.substr(1).c_str()))
			{
				auto uuid = MD5(std::filesystem::path(relativePath).make_preferred().u8string()).toString();

				nlohmann::json metadata;
				metadata["uuid"] = uuid;
				metadata["name"] = asset->getName();
				metadata["suffix"] = (char*)extension.c_str();
				metadata["mipmap"] = asset->getMipLevel();

				std::ofstream ifs(absolutePath.u8string() + u8".meta", std::ios_base::binary);
				if (ifs)
				{
					auto dump = metadata.dump();
					ifs.write(dump.c_str(), dump.size());
					ifs.close();
				}

				AssetDatabase::instance()->importAsset(relativePath);
				AssetManager::instance()->setAssetPath(asset, relativePath);

				this->assetPaths_.insert(relativePath);
			}
			else
			{
				throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");
			}
		}
		catch (const std::exception& e)
		{
			this->deleteAsset(relativePath);
			throw e;
		}
	}

	void
	AssetPipeline::createAsset(const std::shared_ptr<const Animation>& asset, const std::filesystem::path& relativePath) noexcept(false)
	{
		assert(asset && !relativePath.empty());

		try
		{
			std::ofstream stream(this->getAbsolutePath(relativePath), io::ios_base::binary);
			if (stream)
			{
				VMDImporter::save(stream, *asset);
				AssetDatabase::instance()->importAsset(relativePath);
				AssetManager::instance()->setAssetPath(asset, relativePath);

				this->assetPaths_.insert(relativePath);
			}
			else
			{
				throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");
			}
		}
		catch (const std::exception& e)
		{
			this->deleteAsset(relativePath);
			throw e;
		}
	}

	void
	AssetPipeline::createAsset(const std::shared_ptr<const Material>& asset, const std::filesystem::path& relativePath) noexcept(false)
	{
		assert(asset && !relativePath.empty());

		try
		{
			std::ofstream ifs(this->getAbsolutePath(relativePath), std::ios_base::binary);
			if (ifs)
			{
				nlohmann::json mat;
				mat["type"] = asset->type_name();
				mat["name"] = asset->getName();
				mat["blendEnable"] = asset->getBlendEnable();
				mat["blendOp"] = asset->getBlendOp();
				mat["blendSrc"] = asset->getBlendSrc();
				mat["blendDest"] = asset->getBlendDest();
				mat["blendAlphaOp"] = asset->getBlendAlphaOp();
				mat["blendAlphaSrc"] = asset->getBlendAlphaSrc();
				mat["blendAlphaDest"] = asset->getBlendAlphaDest();
				mat["colorWriteMask"] = asset->getColorWriteMask();
				mat["depthEnable"] = asset->getDepthEnable();
				mat["depthBiasEnable"] = asset->getDepthBiasEnable();
				mat["depthBoundsEnable"] = asset->getDepthBoundsEnable();
				mat["depthClampEnable"] = asset->getDepthClampEnable();
				mat["depthWriteEnable"] = asset->getDepthWriteEnable();
				mat["depthMin"] = asset->getDepthMin();
				mat["depthMax"] = asset->getDepthMax();
				mat["depthBias"] = asset->getDepthBias();
				mat["depthSlopeScaleBias"] = asset->getDepthSlopeScaleBias();
				mat["stencilEnable"] = asset->getStencilEnable();
				mat["scissorTestEnable"] = asset->getScissorTestEnable();

				for (auto& it : asset->getMaterialParams())
				{
					switch (it.type)
					{
					case PropertyTypeInfo::PropertyTypeInfoFloat:
						mat[it.key] = asset->get<math::float1>(it.key);
						break;
					case PropertyTypeInfo::PropertyTypeInfoFloat2:
						mat[it.key] = asset->get<math::float2>(it.key).to_array();
						break;
					case PropertyTypeInfo::PropertyTypeInfoFloat3:
						mat[it.key] = asset->get<math::float3>(it.key).to_array();
						break;
					case PropertyTypeInfo::PropertyTypeInfoFloat4:
						mat[it.key] = asset->get<math::float4>(it.key).to_array();
						break;
					case PropertyTypeInfo::PropertyTypeInfoString:
						mat[it.key] = asset->get<std::string>(it.key);
						break;
					case PropertyTypeInfo::PropertyTypeInfoBool:
						mat[it.key] = asset->get<bool>(it.key);
						break;
					case PropertyTypeInfo::PropertyTypeInfoInt:
						mat[it.key] = asset->get<int>(it.key);
						break;
					case PropertyTypeInfo::PropertyTypeInfoTexture:
					{
						auto texture = asset->get<std::shared_ptr<Texture>>(it.key);
						if (texture)
						{
							if (!AssetDatabase::instance()->contains(texture))
							{
								auto texturePath = std::filesystem::path("Assets/Textures").append(make_guid() + ".png");
								this->createFolder(std::filesystem::path("Assets/Textures"));
								this->createAsset(texture, texturePath);
								mat[it.key] = AssetDatabase::instance()->getAssetGuid(texturePath);
							}
							else
							{
								mat[it.key] = AssetDatabase::instance()->getAssetGuid(texture);
							}
						}
					}
					break;
					default:
						break;
					}
				}

				auto dump = mat.dump();
				ifs.write(dump.c_str(), dump.size());
				ifs.close();

				AssetDatabase::instance()->importAsset(relativePath);
				AssetManager::instance()->setAssetPath(asset, relativePath);

				this->assetPaths_.insert(relativePath);
			}
			else
			{
				throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");
			}
		}
		catch (const std::exception& e)
		{
			this->deleteAsset(relativePath);
			throw e;
		}
	}

	void
	AssetPipeline::createAsset(const std::shared_ptr<const GameObject>& asset, const std::filesystem::path& relativePath) noexcept(false)
	{
		assert(asset && !relativePath.empty());

		try
		{
			auto modelPath = AssetDatabase::instance()->getAssetPath(asset);
			if (modelPath.is_absolute())
			{
				this->importAsset(modelPath, relativePath);
				AssetManager::instance()->setAssetPath(asset, relativePath);
			}
		}
		catch (const std::exception& e)
		{
			this->deleteAsset(relativePath);
			throw e;
		}
	}

	void
	AssetPipeline::createPrefab(const std::shared_ptr<const GameObject>& asset, const std::filesystem::path& relativePath) noexcept(false)
	{
		assert(asset && !relativePath.empty());

		try
		{
			std::ofstream ifs(this->getAbsolutePath(relativePath), std::ios_base::binary);
			if (ifs)
			{
				nlohmann::json prefab;
				asset->save(prefab);

				auto dump = prefab.dump();
				ifs.write(dump.c_str(), dump.size());
				ifs.close();

				AssetDatabase::instance()->importAsset(relativePath);
				AssetManager::instance()->setAssetPath(asset, relativePath);

				this->assetPaths_.insert(relativePath);
			}
			else
			{
				throw std::runtime_error(std::string("Creating prefab at path ") + (char*)relativePath.u8string().c_str() + " failed.");
			}
		}
		catch (const std::exception& e)
		{
			this->deleteAsset(relativePath);
			throw e;
		}
	}

	void
	AssetPipeline::deleteAsset(const std::filesystem::path& relativePath) noexcept(false)
	{
		auto absolutePath = this->getAbsolutePath(relativePath);
		if (std::filesystem::is_directory(absolutePath))
		{
			std::filesystem::remove_all(absolutePath);
			AssetManager::instance()->removeMetadataAtPath(relativePath);
		}
		else
		{
			if (std::filesystem::exists(absolutePath))
				std::filesystem::remove(absolutePath);

			AssetManager::instance()->removeMetadataAtPath(relativePath);
		}
	}

	void
	AssetPipeline::createFolder(const std::filesystem::path& assetFolder_) noexcept(false)
	{
		if (!assetFolder_.empty())
		{
			auto relativePath = std::filesystem::path(assetFolder_).make_preferred();
			auto absolutePath = this->getAbsolutePath(relativePath);
			auto path = relativePath.wstring();

			std::filesystem::create_directories(std::filesystem::path(absolutePath));

			auto offset = path.find_first_of(std::filesystem::path::preferred_separator);
			if (offset > 0 && offset < path.size())
			{
				auto folder = path.substr(0, offset);
				if (folder == L"Assets")
					offset = path.find_first_of(std::filesystem::path::preferred_separator, offset + 1);
				else if (folder == L"Packages")
				{
					offset = path.find_first_of(std::filesystem::path::preferred_separator, offset + 1);
					offset = path.find_first_of(std::filesystem::path::preferred_separator, offset + 1);
				}
			}

			while (offset > 0 && offset < path.size())
			{
				auto metaPath = path.substr(0, offset);
				this->assetPaths_.insert(metaPath);
				AssetManager::instance()->createMetadataAtPath(metaPath);
				offset = path.find_first_of(std::filesystem::path::preferred_separator, offset + 1);
			}

			this->assetPaths_.insert(relativePath);
			AssetManager::instance()->createMetadataAtPath(relativePath);
		}
		else
		{
			throw std::runtime_error(std::string("Creating asset at path ") + (char*)assetFolder_.u8string().c_str() + " failed.");
		}
	}

	void
	AssetPipeline::deleteFolder(const std::filesystem::path& relativePath) noexcept(false)
	{
		if (!relativePath.empty())
		{
			auto folderPath = this->getAbsolutePath(relativePath);
			if (std::filesystem::exists(folderPath))
			{
				this->assetPaths_.erase(std::find(this->assetPaths_.begin(), this->assetPaths_.end(), relativePath));
				std::filesystem::remove_all(folderPath);
				AssetManager::instance()->removeMetadataAtPath(relativePath);
			}
		}
		else
		{
			throw std::runtime_error(std::string("Creating asset at path ") + (char*)relativePath.u8string().c_str() + " failed.");
		}
	}

	bool
	AssetPipeline::isValidPath(const std::filesystem::path& assetPath) const noexcept
	{
		if (name_.empty())
			return assetPath.is_absolute();
		else
		{
			auto path = std::filesystem::path(assetPath).u8string();
			for (auto& it : path)
			{
				if (it == '\\')
					it = '/';
			}

			auto length = name_.length();
			if (path.length() > length)
				return std::strncmp((char*)path.data(), (char*)name_.data(), length) == 0 ? true : false;

			return false;
		}
	}

	std::filesystem::path
	AssetPipeline::getAbsolutePath(const std::filesystem::path& assetPath) const noexcept
	{
		return std::filesystem::path(this->rootPath_).append(this->getRelativePath(assetPath).wstring());
	}

	std::filesystem::path
	AssetPipeline::getRelativePath(const std::filesystem::path& assetPath) const noexcept(false)
	{
		if (assetPath.is_absolute())
			return assetPath;
		else
		{
			auto path = std::filesystem::path(assetPath).u8string();
			for (auto& it : path)
			{
				if (it == '\\')
					it = '/';
			}

			auto length = name_.length();
			if (path.size() > length)
			{
				if (name_ == path.substr(0, length))
					return path.substr(length);
			}

			throw std::runtime_error(std::string("Invalid path ") + (char*)assetPath.u8string().c_str() + " failed.");
		}
	}

	void
	AssetPipeline::saveAssets() noexcept(false)
	{
		if (!this->getName().empty())
		{
			std::ofstream ifs(std::filesystem::path(rootPath_).append("manifest.json"), std::ios_base::binary);
			if (ifs)
			{
				nlohmann::json assetDb;

				for (auto& it : assetPaths_)
				{
					auto path = it.u8string();
					if (std::filesystem::exists(this->getAbsolutePath(it)))
						assetDb[(char*)path.c_str()] = AssetDatabase::instance()->getAssetGuid(it);
				}

				auto dump = assetDb.dump();
				ifs.write(dump.c_str(), dump.size());
				ifs.close();
			}
		}
	}

	std::shared_ptr<Object>
	AssetPipeline::loadAssetAtPath(const std::filesystem::path& path) noexcept(false)
	{
		auto ext = path.extension().u8string();
		for (auto& it : ext)
			it = (char)std::tolower(it);

		std::shared_ptr<AssetImporter> assetImporter;
		if (ext == u8".vmd")
			assetImporter = std::make_shared<VMDImporter>();
		else if (ext == u8".hdr" || ext == u8".bmp" || ext == u8".tga" || ext == u8".jpg" || ext == u8".png" || ext == u8".jpeg" || ext == u8".dds")
			assetImporter = std::make_shared<TextureImporter>();
		else if (ext == u8".pmx")
			assetImporter = std::make_shared<PMXImporter>();
		else if (ext == u8".obj")
			assetImporter = std::make_shared<OBJImporter>();
		else if (ext == u8".ogg" || ext == u8".wav" || ext == u8".flac" || ext == u8".mp3")
			assetImporter = std::make_shared<AudioImporter>();
		else if (ext == u8".fbx")
			assetImporter = std::make_shared<FBXImporter>();
		else if (ext == u8".mat")
			assetImporter = std::make_shared<MaterialImporter>();
		else if (ext == u8".abc")
			assetImporter = std::make_shared<AlembicImporter>();
		else if (ext == u8".prefab")
			assetImporter = std::make_shared<PrefabImporter>();

		if (assetImporter)
		{
			auto context = std::make_shared<AssetImporterContext>(path);
			assetImporter->onImportAsset(*context);

			auto mainObject = context->getMainObject();
			if (mainObject)
			{
				AssetManager::instance()->setAssetPath(mainObject, context->getAssetPath());

				for (auto& asset : context->getObjects())
				{
					if (AssetDatabase::instance()->getAssetPath(asset).empty())
						AssetManager::instance()->setAssetPath(asset, context->getAssetPath());

					AssetManager::instance()->addObjectToAsset(asset, context->getAssetPath());
				}

				AssetDatabase::instance()->importAsset(path);
				return mainObject;
			}
		}

		return nullptr;
	}
}