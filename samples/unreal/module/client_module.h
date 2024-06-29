#ifndef UNREAL_CLIENT_MODULE_H_
#define UNREAL_CLIENT_MODULE_H_

#include <unreal_model.h>

namespace unreal
{
	class ClientModule final : public UnrealModule
	{
	public:
		ClientModule() noexcept;
		virtual ~ClientModule() noexcept;

		bool isLogin() const noexcept;

	public:
		virtual void reset() noexcept override;

		virtual void load(nlohmann::json& reader) noexcept override;
		virtual void save(nlohmann::json& reader) noexcept override;

	private:
		ClientModule(const ClientModule&) = delete;
		ClientModule& operator=(const ClientModule&) = delete;

	public:
		bool autoLogin;
		std::string ssl;
		std::string host;
		std::string token;
		std::string username;
		std::string password;
		std::string version;
		std::uint32_t port;
		std::uint32_t coin;
		std::uint32_t vip;
	};
}

#endif