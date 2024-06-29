#include <octoon/texture_importer.h>
#include <octoon/asset_database.h>

namespace octoon
{
	OctoonImplementSubClass(TextureImporter, AssetImporter, "TextureImporter")

	TextureImporter::TextureImporter() noexcept
	{
	}

	TextureImporter::~TextureImporter()
	{
	}

	void
	TextureImporter::onImportAsset(AssetImporterContext& context) noexcept(false)
	{
		auto filepath = AssetDatabase::instance()->getAbsolutePath(context.getAssetPath());

		auto texture = std::make_shared<Texture>();
		if (texture->load(filepath))
		{
			texture->setName((char*)filepath.filename().u8string().c_str());

			auto metadata = context.getMetadata();
			if (metadata.is_object())
			{
				if (metadata.contains("mipmap"))
					texture->setMipLevel(metadata["mipmap"].get<nlohmann::json::number_integer_t>());

				if (metadata.contains("labels"))
				{
					std::vector<std::string> labels;
					for (auto& it : metadata["labels"])
						labels.push_back(it.get<std::string>());
					AssetDatabase::instance()->setLabels(texture, std::move(labels));
				}
			}
			else
			{
				auto ext = filepath.extension().u8string();
				for (auto& it : ext)
					it = (char)std::tolower(it);

				if (ext == u8".hdr")
					texture->setMipLevel(8);
			}

			texture->apply();

			context.setMainObject(texture);
		}
	}
}