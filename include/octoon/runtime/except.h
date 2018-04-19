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
			virtual const char* what() const noexcept override;
			virtual const error_code& code() const noexcept;

		protected:
			exception(const char* msg, const error_code& code) noexcept;
			exception(const std::string& msg, const error_code& code) noexcept;
			exception(std::string&& msg, const error_code& code) noexcept;
			exception(std::string&& msg, error_code&& code) noexcept;
			exception(exception&& move) noexcept;
			virtual ~exception() noexcept;

			static std::string name(const std::string& ename, const error_code& id)
			{
				return "[octoon.exception." + ename + "." + std::to_string(id.value()) + "] ";
			}

		private:
			exception(const exception&) noexcept = delete;
			exception& operator=(const exception&) noexcept = delete;

		private:
			std::string message_;
			error_code code_;
		};

		class invalid_iterator final : public exception
		{
		public:
			static invalid_iterator create(const std::string& what_arg, const error_code& code = error_code::none)
			{
				std::string w = exception::name("invalid_iterator", code) + what_arg;
				return invalid_iterator(w, code);
			}

		private:
			invalid_iterator(const std::string& what_arg, const error_code& code)
				: exception(what_arg, code)
			{}
		};

		class type_error final : public exception
		{
		public:
			static type_error create(const std::string& what_arg, const error_code& code = error_code::none)
			{
				std::string w = exception::name("type_error", code) + what_arg;
				return type_error(w, code);
			}

		private:
			type_error(const std::string& what_arg, const error_code& code)
				: exception(what_arg, code)
			{}
		};

		class out_of_range : public exception
		{
		public:
			static out_of_range create(const std::string& what_arg, const error_code& code = error_code::none)
			{
				std::string w = exception::name("out_of_range", code) + what_arg;
				return out_of_range(w, code);
			}

		private:
			out_of_range(const std::string& what_arg, const error_code& code)
				: exception(what_arg, code)
			{}
		};

		class runtime_error : public exception
		{
		public:
			static runtime_error create(const std::string& what_arg, const error_code& code = error_code::none)
			{
				std::string w = exception::name("runtime_error", code) + what_arg;
				return runtime_error(w, code);
			}

		private:
			runtime_error(const std::string& what_arg, const error_code& code)
				: exception(what_arg, code)
			{}
		};

		class null_reference : public exception
		{
		public:
			static null_reference create(const std::string& what_arg, const error_code& code = error_code::none)
			{
				std::string w = exception::name("null_reference", code) + what_arg;
				return null_reference(w, code);
			}

		private:
			null_reference(const std::string& what_arg, const error_code& code)
				: exception(what_arg, code)
			{}
		};
	}
}

#endif