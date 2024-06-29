#ifndef UNREAL_RECORD_COMPONENT_H_
#define UNREAL_RECORD_COMPONENT_H_

#include <unreal_component.h>
#include "module/record_module.h"
#include <filesystem>

typedef struct OIDNFilterImpl* OIDNFilter;
typedef struct OIDNDeviceImpl* OIDNDevice;
typedef struct OIDNBufferImpl* OIDNBuffer;

namespace unreal
{
	class RecordComponent final : public UnrealComponent<RecordModule>
	{
	public:
		RecordComponent() noexcept;
		~RecordComponent() noexcept;

		void captureImage(const std::filesystem::path& filepath) noexcept;

		bool startRecord(const std::filesystem::path& filepath) noexcept;
		void stopRecord() noexcept(false);

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(RecordComponent);
		}

	private:
		virtual void onInit() noexcept override;

		virtual void onEnable() noexcept override;
		virtual void onDisable() noexcept override;

		virtual void onRecord() noexcept(false);

	private:
		void setupDenoise() noexcept;
		void shutdownDenoise() noexcept;

		void captureImage() noexcept;

	private:
		float time_;
		float timeCount_;

		OIDNDevice device_;
		OIDNFilter filter_;

		OIDNBuffer oidnColorBuffer_;
		OIDNBuffer oidnNormalBuffer_;
		OIDNBuffer oidnAlbedoBuffer_;
		OIDNBuffer oidnOutputBuffer_;

		std::vector<octoon::math::float3> colorBuffer_;
		std::vector<octoon::math::float3> normalBuffer_;
		std::vector<octoon::math::float3> albedoBuffer_;
		std::vector<octoon::math::float3> denoiseBuffer_;
	};
}

#endif