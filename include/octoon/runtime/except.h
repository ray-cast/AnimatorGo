#ifndef OCTOON_EXCEPT_H_
#define OCTOON_EXCEPT_H_

#include <octoon/runtime/platform.h>
#include <string>

namespace octoon
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
		exception(std::string_view msg, error_code&& code) noexcept;
		exception(std::string_view msg, const error_code& code) noexcept;
		exception(exception&& move) noexcept;
		virtual ~exception() noexcept;

		static std::string name(std::string_view ename, const error_code& id)
		{
			return "[octoon.exception." + std::string(ename) + "." + std::to_string(id.value()) + "] ";
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
		static invalid_iterator create(std::string_view what_arg, const error_code& code = error_code::none)
		{
			std::string w = exception::name("invalid_iterator", code) + std::string(what_arg);
			return invalid_iterator(w, code);
		}

	private:
		invalid_iterator(std::string_view what_arg, const error_code& code)
			: exception(what_arg, code)
		{}
	};

	class type_error final : public exception
	{
	public:
		static type_error create(std::string_view what_arg, const error_code& code = error_code::none)
		{
			std::string w = exception::name("type_error", code) + std::string(what_arg);
			return type_error(w, code);
		}

	private:
		type_error(std::string_view what_arg, const error_code& code)
			: exception(what_arg, code)
		{}
	};

	class out_of_range : public exception
	{
	public:
		static out_of_range create(std::string_view what_arg = "out_of_range", const error_code& code = error_code::none)
		{
			std::string w = exception::name("out_of_range", code) + std::string(what_arg);
			return out_of_range(w, code);
		}

	private:
		out_of_range(std::string_view what_arg, const error_code& code)
			: exception(what_arg, code)
		{}
	};

	class runtime_error : public exception
	{
	public:
		static runtime_error create(std::string_view what_arg, const error_code& code = error_code::none)
		{
			std::string w = exception::name("runtime_error", code) + std::string(what_arg);
			return runtime_error(w, code);
		}

	private:
		runtime_error(std::string_view what_arg, const error_code& code)
			: exception(what_arg, code)
		{}
	};

	class null_reference : public exception
	{
	public:
		static null_reference create(std::string_view what_arg = "null_reference", const error_code& code = error_code::none)
		{
			std::string w = exception::name("null_reference", code) + std::string(what_arg);
			return null_reference(w, code);
		}

	private:
		null_reference(std::string_view what_arg, const error_code& code)
			: exception(what_arg, code)
		{}
	};

	class not_implemented : public exception
	{
	public:
		static not_implemented create(std::string_view what_arg = "Not implemented yet", const error_code& code = error_code::none)
		{
			std::string w = exception::name("not_implemented", code) + std::string(what_arg);
			return not_implemented(w, code);
		}

	private:
		not_implemented(std::string_view what_arg, const error_code& code)
			: exception(what_arg, code)
		{}
	};
}

#endif