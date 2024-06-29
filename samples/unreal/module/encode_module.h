#ifndef UNREAL_ENCODE_MODULE_H_
#define UNREAL_ENCODE_MODULE_H_

#include <unreal_model.h>
#include <cstdint>

namespace unreal
{
	enum EncodeMode
	{
		H264,
		H265,
		Frame
	};

	enum VideoQuality
	{
		Low,
		Medium,
		High
	};

	class EncodeModule final : public UnrealModule
	{
	public:
		EncodeModule() noexcept;
		virtual ~EncodeModule() noexcept;

	public:
		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& writer) noexcept override;

		virtual void disconnect() noexcept override;

	private:
		EncodeModule(const EncodeModule&) = delete;
		EncodeModule& operator=(const EncodeModule&) = delete;

	public:
		MutableLiveData<float> crf;
		MutableLiveData<EncodeMode> encodeMode;
		MutableLiveData<VideoQuality> quality;
		MutableLiveData<std::uint32_t> frame_type;
		MutableLiveData<std::uint32_t> encode_speed;
	};
}

#endif