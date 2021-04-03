#ifndef OCTOON_SHADER_H_
#define OCTOON_SHADER_H_

#include <octoon/runtime/platform.h>
#include <string>

namespace octoon
{
	class OCTOON_EXPORT Shader
	{
	public:
		Shader();
		Shader(const std::string& vs_, const std::string& fs_);

		void setQueue(std::string_view name) noexcept;
		const std::string& getQueue() const noexcept;

		void setLightMode(std::string_view name) noexcept;
		const std::string& getLightMode() const noexcept;

	public:
		std::string queue_;
		std::string lightMode;

		std::string vs;
		std::string fs;
	};
}

#endif