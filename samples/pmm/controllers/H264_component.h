#ifndef OCTOON_H264_COMPONENT_H_
#define OCTOON_H264_COMPONENT_H_

#include <octoon/game_component.h>
#include <octoon/io/iostream.h>

#include "module/h264_module.h"
#include "mysticLit_component.h"

typedef struct OIDNFilterImpl* OIDNFilter;
typedef struct OIDNDeviceImpl* OIDNDevice;
typedef struct OIDNBufferImpl* OIDNBuffer;

namespace MysticLit
{
	class H264Component final : public MysticLitComponent<H264Module>
	{
	public:
		H264Component() noexcept;

		void setActive(bool active) noexcept override;
		bool getActive() const noexcept override;

		void setTimeStep(float timeStep) noexcept;
		float getTimeStep() const noexcept;

		void update() noexcept;

		void play() noexcept;
		void capture(const std::shared_ptr<std::ofstream>& stream) noexcept;

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

	private:
		void capture() noexcept;

		static void rgb2yuv(float* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept;
		static void rgb2yuv(std::uint8_t* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept;

	private:
		H264Component(const H264Component&) = delete;
		H264Component& operator=(const H264Component&) = delete;

	private:
		bool active_;

		float time_;
		float timeStep_;

		std::size_t width_;
		std::size_t height_;

		OIDNFilter filter_;
		OIDNDevice device_;

		OIDNBuffer oidnColorBuffer_;
		OIDNBuffer oidnNormalBuffer_;
		OIDNBuffer oidnAlbedoBuffer_;
		OIDNBuffer oidnOutputBuffer_;

		std::unique_ptr<char[]> enc_;
		std::unique_ptr<char[]> scratch_;
		std::unique_ptr<std::uint8_t[]> buf_;

		std::shared_ptr<std::ofstream> ostream_;
	};
}

#endif