#include <octoon/audio_importer.h>
#include <octoon/audio/ogg_stream.h>
#include <octoon/audio/mp3_stream.h>
#include <octoon/audio/wav_stream.h>
#include <octoon/audio/flac_stream.h>
#include <octoon/asset_database.h>

namespace octoon
{
	AudioImporter::AudioImporter() noexcept
	{
	}

	AudioImporter::~AudioImporter() noexcept
	{
	}

	void
	AudioImporter::onImportAsset(AssetImporterContext& context) noexcept(false)
	{
		auto filepath = AssetDatabase::instance()->getAbsolutePath(context.getAssetPath());
		auto ext = filepath.extension().u8string();
		for (auto& it : ext)
			it = (char)std::tolower(it);

		if (ext == u8".ogg")
		{
			auto reader = std::make_shared<OggAudioReader>(filepath);
			if (reader->is_open())
				context.setMainObject(reader);
		}
		else if (ext == u8".mp3")
		{
			auto reader = std::make_shared<Mp3AudioReader>(filepath);
			if (reader->is_open())
				context.setMainObject(reader);
		}
		else if (ext == u8".wav")
		{
			auto reader = std::make_shared<WavAudioReader>(filepath);
			if (reader->is_open())
				context.setMainObject(reader);
		}
		else if (ext == u8".flac")
		{
			auto reader = std::make_shared<FlacAudioReader>(filepath);
			if (reader->is_open())
				context.setMainObject(reader);
		}
	}
}