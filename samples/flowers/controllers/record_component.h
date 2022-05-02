#ifndef FLOWER_RECORD_COMPONENT_H_
#define FLOWER_RECORD_COMPONENT_H_

#include <flower_component.h>
#include <module/canvas_module.h>

typedef struct OIDNFilterImpl* OIDNFilter;
typedef struct OIDNDeviceImpl* OIDNDevice;
typedef struct OIDNBufferImpl* OIDNBuffer;

namespace flower
{
	class RecordComponent final : public RabbitComponent<RecordModule>
	{
	public:
		RecordComponent() noexcept;
		~RecordComponent() noexcept;

		void captureImage(std::string_view filepath) noexcept;

		bool startRecord(std::string_view filepath) noexcept;
		void stopRecord() noexcept;

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(RecordComponent);
		}

	private:
		virtual void onEnable() noexcept override;
		virtual void onDisable() noexcept override;

		virtual void onResize(const octoon::input::InputEvent& event) noexcept override;

		virtual void onRecord() noexcept;

	private:
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