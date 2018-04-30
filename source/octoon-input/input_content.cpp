#include <octoon/input/input_content.h>

namespace octoon
{
	namespace input
	{
		InputOp
		operator+(InputKey::Code code1, InputKey::Code code2)
		{
			InputContent c1{ InputContent::Keyboard, (std::uint16_t)code1 };
			InputContent c2{ InputContent::Keyboard, (std::uint16_t)code2 };
			return { c1, c2 };
		}

		InputOp
		operator+(InputKey::Code code1, InputButton::Code code2)
		{
			InputContent c1{ InputContent::Keyboard, (std::uint16_t)code1 };
			InputContent c2{ InputContent::Mouse, (std::uint16_t)code2 };
			return { c1, c2 };
		}

		InputOp
		operator+(InputButton::Code code1, InputKey::Code code2)
		{
			InputContent c1{ InputContent::Mouse, (std::uint16_t)code1 };
			InputContent c2{ InputContent::Keyboard, (std::uint16_t)code2 };
			return { c1, c2 };
		}

		InputOp
		operator+(InputButton::Code code1, InputButton::Code code2)
		{
			InputContent c1{ InputContent::Mouse, (std::uint16_t)code1 };
			InputContent c2{ InputContent::Mouse, (std::uint16_t)code2 };
			return { c1, c2 };
		}

		InputOp
		operator+(InputOp codes, InputKey::Code code)
		{
			codes.emplace_back(InputContent::Keyboard, (std::uint16_t)code);
			return codes;
		}

		InputOp
		operator+(InputOp codes, InputButton::Code code)
		{
			codes.emplace_back(InputContent::Mouse, (std::uint16_t)code);
			return codes;
		}
	}
}