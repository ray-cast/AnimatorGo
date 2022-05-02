#include "client_component.h"
#include "../flower_profile.h"

#pragma warning(push)
#pragma warning(disable:4458)
#pragma warning(disable:4267)
#pragma warning(disable:4244)
#include "../libs/sha1/sha1.hpp"
#pragma warning(pop)

#include <octoon/io/http_buf.h>
#include <octoon/perspective_camera_component.h>

namespace flower
{
	ClientComponent::ClientComponent() noexcept
	{
	}

	ClientComponent::~ClientComponent() noexcept
	{
	}

	void
	ClientComponent::login(const std::string& user, const std::string& pwd) noexcept(false)
	{
		auto& model = this->getModel();

		char hex1[SHA1_HEX_SIZE];
		char hex2[SHA1_HEX_SIZE];
		sha1(pwd.c_str()).finalize().print_hex(hex1);
		sha1(hex1).finalize().print_hex(hex2);

		std::ostringstream command;
		command << "https://" << model->host << ":" << std::to_string(model->port) << "/api/login";

		std::ostringstream data;
		data << R"({)";
		data << R"("user":")" << user << R"(",)";
		data << R"("pwd":")" << hex2 << R"(",)";
		data << R"("platform":)" << 0;
		data << R"(})";

		octoon::io::httpbuf request;
		if (request.post(command.str(), data.str(), "content-type:application/json", 5, model->ssl))
		{
			auto json = request.json();
			if (json.find("success") != json.end())
			{
				if ((bool)json["success"])
				{
					model->username = user;
					model->coin = json["coin"];
					model->vip = json["vip"];
					model->token = json["token"].get<octoon::runtime::json::string_t>();
				}
				else
				{
					throw std::runtime_error(json["message"].get<octoon::runtime::json::string_t>().c_str());
				}
			}
			else
			{
				throw std::runtime_error((const char*)u8"请求错误");
			}
		}
		else
		{
			throw std::runtime_error((const char*)u8"请求超时");
		}
	}

	void
	ClientComponent::loginOut() noexcept(false)
	{
		auto& model = this->getModel();
		if (!model->token.empty())
		{
			std::ostringstream command;
			command << "https://" << model->host << ":" << std::to_string(model->port) << "/api/loginOut";
			
			std::ostringstream data;
			data << R"({"token":")" << model->token << R"("})";

			octoon::io::httpbuf request;
			if (request.post(command.str(), data.str(), "content-type:application/json", 5, model->ssl))
			{
				auto json = request.json();
				if (json.find("success") != json.end())
				{
					if ((bool)json["success"])
					{
						model->coin = 0;
						model->username.clear();
						model->password.clear();
						model->token.clear();
					}
					else
					{
						throw std::runtime_error(json["message"].get<octoon::runtime::json::string_t>().c_str());
					}
				}
				else
				{
					throw std::runtime_error((const char*)u8"请求错误");
				}
			}
			else
			{
				throw std::runtime_error((const char*)u8"请求超时");
			}
		}
	}

	void
	ClientComponent::signUp(const std::string& user, const std::string& pwd) noexcept(false)
	{
		auto& model = this->getModel();

		char hex1[SHA1_HEX_SIZE];
		char hex2[SHA1_HEX_SIZE];
		sha1(pwd.c_str()).finalize().print_hex(hex1);
		sha1(hex1).finalize().print_hex(hex2);

		std::ostringstream command;
		command << "https://" << model->host << ":" << std::to_string(model->port) << "/api/register";

		std::ostringstream data;
		data << R"({)";
		data << R"("user":")" << user << R"(",)";
		data << R"("pwd":")" << hex2 << R"(")";
		data << R"(})";

		octoon::io::httpbuf request;
		if (request.post(command.str(), data.str(), "content-type:application/json", 5, model->ssl))
		{
			auto json = request.json();
			if (json.find("success") != json.end())
			{
				if (!(bool)json["success"])
				{
					throw std::runtime_error(json["message"].get<octoon::runtime::json::string_t>().c_str());
				}
			}
			else
			{
				throw std::runtime_error((const char*)u8"请求错误");
			}
		}
		else
		{
			throw std::runtime_error((const char*)u8"请求超时");
		}
	}

	void
	ClientComponent::forget(const std::string& email) noexcept(false)
	{
	}

	void
	ClientComponent::refresh() noexcept(false)
	{
		auto& model = this->getModel();
		if (!model->token.empty())
		{
			std::ostringstream command;
			command << "https://" << model->host << ":" << std::to_string(model->port) << "/api/select?";
			command << "token=" << model->token;

			octoon::io::httpbuf request;
			if (request.get(command.str(), 5, model->ssl))
			{
				auto json = request.json();
				if (json.find("success") != json.end())
				{
					if ((bool)json["success"])
					{
						model->vip = json["vip"];
						model->coin = json["coin"];
					}
					else
					{
						throw std::runtime_error(json["message"].get<octoon::runtime::json::string_t>().c_str());
					}
				}
				else
				{
					throw std::runtime_error((const char*)u8"请求错误");
				}
			}
			else
			{
				throw std::runtime_error((const char*)u8"请求超时");
			}
		}
	}

	void
	ClientComponent::coin(std::int32_t coin) noexcept(false)
	{
		auto& model = this->getModel();
		if (!model->token.empty())
		{
			std::ostringstream command;
			command << "https://" << model->host << ":" << std::to_string(model->port) << "/api/coin";

			std::ostringstream data;
			data << R"({)";
			data << R"("token":")" << model->token << R"(",)";
			data << R"("coin":")" << std::to_string(coin) << R"(")";
			data << R"(})";

			octoon::io::httpbuf request;
			if (request.post(command.str(), data.str(), "content-type:application/json", 5, model->ssl))
			{
				auto json = request.json();
				if (json.find("success") != json.end())
				{
					if ((bool)json["success"])
						model->coin = json["coin"];
					else
						throw std::runtime_error(json["message"].get<octoon::runtime::json::string_t>().c_str());
				}
				else
				{
					throw std::runtime_error((const char*)u8"请求错误");
				}
			}
			else
			{
				throw std::runtime_error((const char*)u8"请求超时");
			}
		}
	}

	std::uint32_t
	ClientComponent::invoice(const std::string& tradeNo) noexcept(false)
	{
		auto& model = this->getModel();
		if (!model->token.empty())
		{
			std::ostringstream command;
			command << "https://" << model->host << ":" << std::to_string(model->port) << "/api/invoice?";
			command << "tradeNo=" << tradeNo;

			octoon::io::httpbuf request;
			if (request.get(command.str(), 5, model->ssl))
			{
				auto json = request.json();
				if (json.find("success") != json.end())
				{
					if ((bool)json["success"])
						return (int)json["status"];
					else
						throw std::runtime_error(json["message"].get<octoon::runtime::json::string_t>().c_str());
				}
				else
				{
					throw std::runtime_error((const char*)u8"请求错误");
				}
			}
			else
			{
				throw std::runtime_error((const char*)u8"请求超时");
			}
		}

		return false;
	}

	std::future<octoon::runtime::json>
	ClientComponent::alipay(std::uint32_t type) noexcept(false)
	{
		return std::async(std::launch::async, [=]() -> octoon::runtime::json
		{
			auto& model = this->getModel();
			if (!model->token.empty())
			{
				std::ostringstream command;
				command << "https://" << model->host << ":" << std::to_string(model->port) << "/api/pay";

				std::ostringstream data;
				data << R"({)";
				data << R"("token":")" << model->token << R"(",)";
				data << R"("type":")" << std::to_string(type) << R"(")";
				data << R"(})";

				octoon::io::httpbuf request;
				if (request.post(command.str(), data.str(), "content-type:application/json", 5, model->ssl))
				{
					return request.json();
				}
				else
				{
					throw std::runtime_error((const char*)u8"请求超时");
				}
			}

			return octoon::runtime::json();
		});
	}

	std::optional<std::string>
	ClientComponent::version() noexcept(false)
	{
		auto& model = this->getModel();
		std::ostringstream command;
		command << "https://" << model->host << ":" << std::to_string(model->port) << "/api/version";
		octoon::io::httpbuf request;
		if (request.get(command.str(), 0, model->ssl))
		{
			auto json = request.json();
			if ((bool)json["success"])
				return json["version"].get<octoon::runtime::json::string_t>();
			else
				throw std::runtime_error(json["message"].get<octoon::runtime::json::string_t>().c_str());
		}
		else
		{
			throw std::runtime_error((const char*)u8"请求超时");
		}
	}

	void 
	ClientComponent::onEnable() noexcept
	{
		auto& model = this->getModel();
		if (!model->token.empty())
		{
			std::ostringstream command;
			command << "https://" << model->host << ":" << std::to_string(model->port) << "/api/select?";
			command << "token=" << model->token;

			octoon::io::httpbuf request;
			if (request.get(command.str(), 5, model->ssl))
			{
				auto json = request.json();
				if ((bool)json["success"])
				{
					model->vip = json["vip"];
					model->coin = json["coin"];
				}
				else
				{
					model->token.clear();
				}
			}
			else
			{
				model->token.clear();
			}
		}
	}
	
	void 
	ClientComponent::onDisable() noexcept
	{
	}

	void
	ClientComponent::onPostProcess() noexcept
	{
	}
}