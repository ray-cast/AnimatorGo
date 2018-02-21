#ifndef OCTOON_EXCEPT_H_
#define OCTOON_EXCEPT_H_

#include <octoon/runtime/platform.h>
#include <string>

namespace octoon
{
	namespace runtime
	{
		class OCTOON_EXPORT error_code final
		{
		public:
			typedef std::size_t int_type;
			enum : int_type { none = 0 };

			error_code() noexcept;
			error_code(int_type code) noexcept;

			error_code& assign(int_type value) noexcept;

			int_type value() const noexcept;

		private:

			int_type val_;
		};

		class OCTOON_EXPORT exception : public std::exception
		{
		public:
			exception() noexcept = default;
			virtual ~exception() = default;

			virtual const char* what() const noexcept = 0;
			virtual const error_code& code() const noexcept = 0;

		private:
			exception(const exception&) noexcept = delete;
			exception& operator=(const exception&) noexcept = delete;
		};

		class OCTOON_EXPORT failure : public exception
		{
		public:
			failure(const error_code& code) noexcept;
			failure(const error_code::int_type& code) noexcept;
			failure(const char* msg, const error_code& code = error_code::none) noexcept;
			failure(const char* msg, const char* stack, const error_code& code = error_code::none) noexcept;
			failure(const std::string& msg, const error_code& code = error_code::none) noexcept;
			failure(const std::string& msg, const std::string& stack, const error_code& code = error_code::none) noexcept;
			failure(failure&& move) noexcept;
			virtual ~failure() noexcept;

			const char* message() const noexcept;
			const char* stack() const noexcept;
			const char* what() const noexcept override;

			const error_code& code() const noexcept override;

		private:
			failure(const failure&) noexcept = delete;
			failure& operator=(const failure&) noexcept = delete;

		private:
			std::string info_;
			std::string message_;
			std::string stack_;
			error_code code_;
		};
	}
}

#endif