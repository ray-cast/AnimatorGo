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

		failure::failure(const char* msg, const char* stack, const error_code& code) noexcept
			: message_(msg)
			, stack_(stack)
			, code_(code)
		{
		}

		failure::failure(const std::string& msg, const error_code& code) noexcept
			: message_(msg)
			, code_(code)
		{
		}

		failure::failure(const std::string& msg, const std::string& stack, const error_code& code) noexcept
			: message_(msg)
			, stack_(stack)
			, code_(code)
		{
		}

		failure::failure(failure&& move) noexcept
			: message_(std::move(move.message_))
			, stack_(std::move(move.stack_))
			, code_(std::move(move.code_))
			, info_(std::move(move.info_))
		{
		}

		failure::~failure() noexcept
		{
		}

		const char*
		failure::message() const noexcept
		{
			return message_.c_str();
		}

		const char*
		failure::stack() const noexcept
		{
			return stack_.c_str();
		}

		const char*
		failure::what() const noexcept
		{
			return info_.c_str();
		}

		const error_code&
		failure::code() const noexcept
		{
			return (code_);
		}
	}
}