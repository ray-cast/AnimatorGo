#include <octoon/material/shader.h>

namespace octoon::material
{
	Shader::Shader()
		: lightMode("Forward")
		, queue_("Opaque")
	{
	}

	Shader::Shader(const std::string & vs_, const std::string & fs_)
		: vs(vs_)
		, fs(fs_)
	{
	}

	void
	Shader::setQueue(std::string_view name) noexcept
	{
		this->queue_ = name;
	}

	const std::string&
	Shader::getQueue() const noexcept
	{
		return this->queue_;
	}

	void
	Shader::setLightMode(std::string_view name) noexcept
	{
		this->lightMode = name;
	}

	const std::string&
	Shader::getLightMode() const noexcept
	{
		return this->lightMode;
	}
}