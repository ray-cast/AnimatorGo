#ifndef UNREAL_H265_COMPONENT_H_
#define UNREAL_H265_COMPONENT_H_

#include "module/encode_module.h"
#include "unreal_component.h"
#include <octoon/math/vector3.h>
#include <filesystem>

struct x265_param;
struct x265_encoder;
struct x265_picture;

namespace unreal
{
	class H265Component final : public UnrealComponent<EncodeModule>
	{
	public:
		H265Component() noexcept;

		bool create(const std::filesystem::path& filepath) noexcept(false);
		void close() noexcept;

		void write(const octoon::math::Vector3* data) noexcept(false);

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(H265Component);
		}

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

	private:
		void convert(float* rgb, int w, int h, std::uint8_t* yuvBuf) noexcept;

	private:
		H265Component(const H265Component&) = delete;
		H265Component& operator=(const H265Component&) = delete;

	private:
		std::uint32_t width_;
		std::uint32_t height_;

		x265_param* param_;
		x265_picture* picture_;
		x265_encoder* encoder_;

		std::unique_ptr<char[]> enc_;
		std::unique_ptr<char[]> scratch_;
		std::unique_ptr<std::uint8_t[]> buf_;

		std::filesystem::path filepath_;
		std::shared_ptr<std::ostream> ostream_;
	};
}

#endif