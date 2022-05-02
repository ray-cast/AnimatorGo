#ifndef FLOWER_CLIENT_COMPONENT_H_
#define FLOWER_CLIENT_COMPONENT_H_

#include <optional>
#include <future>
#include "../flower_component.h"
#include "../module/client_module.h"

namespace flower
{
	class ClientComponent final : public RabbitComponent<ClientModule>
	{
	public:
		ClientComponent() noexcept;
		virtual ~ClientComponent() noexcept;

		void login(const std::string& user, const std::string& pwd) noexcept(false);
		void loginOut() noexcept(false);

		void signUp(const std::string& user, const std::string& pwd) noexcept(false);
		void forget(const std::string& email) noexcept(false);

		void refresh() noexcept(false);
		void coin(std::int32_t coin) noexcept(false);

		std::uint32_t invoice(const std::string& tradeNo) noexcept(false);
		std::future<octoon::runtime::json> alipay(std::uint32_t type) noexcept(false);

		std::optional<std::string> version() noexcept(false);

		virtual const std::type_info& type_info() const noexcept
		{
			return typeid(ClientComponent);
		}

	private:
		void onEnable() noexcept override;
		void onDisable() noexcept override;

		void onPostProcess() noexcept override;

	private:
		ClientComponent(const ClientComponent&) = delete;
		ClientComponent& operator=(const ClientComponent&) = delete;
	};
}

#endif