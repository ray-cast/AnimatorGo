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

		exception::exception(std::string_view msg, error_code&& code) noexcept
			: message_(std::move(msg))
			, code_(std::move(code))
		{
		}

		exception::exception(std::string_view msg, const error_code& code) noexcept
			: message_(msg)
			, code_(code)
		{
		}

		exception::exception(exception&& move) noexcept
			: message_(std::move(move.message_))
			, code_(std::move(move.code_))
		{
		}

		exception::~exception() noexcept
		{
		}

		const char*
		exception::what() const noexcept
		{
			return message_.c_str();
		}

		const error_code&
		exception::code() const noexcept
		{
			return (code_);
		}
	}
}