#include <octoon/runtime/except.h>

namespace octoon
{
	namespace runtime
	{
		error_code::error_code() noexcept
		{
		}

		error_code::error_code(int_type code) noexcept
		{
			this->assign(code);
		}

		error_code&
		error_code::assign(int_type value) noexcept
		{
			val_ = value;
			return *this;
		}

		error_code::int_type
		error_code::value() const noexcept
		{
			return val_;
		}

		failure::failure(const error_code& code) noexcept
			: code_(code)
		{
		}

		failure::failure(const error_code::int_type& code) noexcept
			: code_(code)
		{
		}

		failure::failure(const char* msg, const error_code& code) noexcept
			: message_(msg)
			, code_(code)
		{
		}

		failure::failure(const std::string& msg, const error_code& code) noexcept
			: message_(msg)
			, code_(code)
		{
		}

		failure::failure(std::string&& msg, const error_code& code) noexcept
			: message_(std::move(msg))
			, code_(code)
		{
		}

		failure::failure(std::string&& msg, error_code&& code) noexcept
			: message_(std::move(msg))
			, code_(std::move(code))
		{
		}

		failure::failure(failure&& move) noexcept
			: message_(std::move(move.message_))
			, code_(std::move(move.code_))
		{
		}

		failure::~failure() noexcept
		{
		}

		const std::string&
		failure::message() const noexcept
		{
			return message_;
		}

		const char*
		failure::what() const noexcept
		{
			return message_.c_str();
		}

		const error_code&
		failure::code() const noexcept
		{
			return (code_);
		}
	}
}