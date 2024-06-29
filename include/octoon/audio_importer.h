#ifndef OCTOON_AUDIO_IMPORTER_H_
#define OCTOON_AUDIO_IMPORTER_H_

#include <octoon/asset_importer.h>

namespace octoon
{
	class OCTOON_EXPORT AudioImporter final : public AssetImporter
	{
	public:
		AudioImporter() noexcept;
		~AudioImporter() noexcept;

		virtual void onImportAsset(AssetImporterContext& context) noexcept(false) override;

	private:
		AudioImporter(const AudioImporter&) = delete;
		AudioImporter& operator=(const AudioImporter&) = delete;
	};
}

#endif