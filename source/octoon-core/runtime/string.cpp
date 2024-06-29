#include <octoon/runtime/string.h>
#include <cmath>
#include <locale>
#include <limits.h>
#include <cstring>

// the operation specific directory separator
#ifndef __WINDOWS__
#   define SEPARATOR '/'
#else
#   define  SEPARATOR '\\'
#endif

// maximum path length
// XXX http://insanecoding.blogspot.com/2007/11/pathmax-simply-isnt.html
#ifdef PATH_MAX
#    define PATHLIMIT PATH_MAX
#else
#    define PATHLIMIT 4096
#endif

#ifndef MAX_PATH
#   define MAX_PATH 4096
#endif

namespace octoon
{
	namespace runtime
	{
		namespace string
		{
			const float fast_atof_table[16] =
			{
				// we write [16] here instead of [] to work around a swig bug
				0.f,
				0.1f,
				0.01f,
				0.001f,
				0.0001f,
				0.00001f,
				0.000001f,
				0.0000001f,
				0.00000001f,
				0.000000001f,
				0.0000000001f,
				0.00000000001f,
				0.000000000001f,
				0.0000000000001f,
				0.00000000000001f,
				0.000000000000001f
			};

			const long long itoa10_table[] =
			{
				1,
				100,
				10000,
				1000000,
				1000000000,
				100000000000,
				10000000000000,
				1000000000000000,
				1000000000000000000,
			};

			const unsigned long long uitoa10_table[] =
			{
				1,
				100,
				10000,
				1000000,
				1000000000,
				100000000000,
				10000000000000,
				1000000000000000,
				1000000000000000000,
			};

			char* strcpy(char* dest, const char* source)
			{
				return ::strcpy(dest, source);
			}

			wchar_t* strcpy(wchar_t* dest, const wchar_t* source)
			{
				return ::wcscpy(dest, source);
			}

			char* strncpy(char* s1, const char* s2, std::size_t cnt)
			{
				return ::strncpy(s1, s2, cnt);
			}

			wchar_t* strncpy(wchar_t* s1, const wchar_t* s2, std::size_t cnt)
			{
				return ::wcsncpy(s1, s2, cnt);
			}

			char* strstr(char* dest, const char * source)
			{
				return ::strstr(dest, source);
			}

			wchar_t* strstr(wchar_t * dest, const wchar_t * source)
			{
				return ::wcsstr(dest, source);
			}

			std::size_t strlen(const char* str)
			{
				return ::strlen(str);
			}

			std::size_t strlen(const wchar_t* str)
			{
				return ::wcslen(str);
			}

			int strcmp(const char *s1, const char *s2)
			{
				return ::strcmp(s1, s2);
			}

			int strcmp(const wchar_t *s1, const wchar_t *s2)
			{
				return ::wcscmp(s1, s2);
			}

			int strcmp(const std::string& a, const std::string& b)
			{
				int i = (int)b.length() - (int)a.length();
				return (i > 0 ? i : strcmp(a.c_str(), b.c_str()));
			}

			int strcmp(const std::wstring& a, const std::wstring& b)
			{
				int i = (int)b.length() - (int)a.length();
				return (i > 0 ? i : strcmp(a.c_str(), b.c_str()));
			}

			int strncmp(const char* s1, const char* s2, std::size_t cnt)
			{
				return ::strncmp(s1, s2, cnt);
			}

			int strncmp(const wchar_t* s1, const wchar_t* s2, std::size_t cnt)
			{
				return ::wcsncmp(s1, s2, cnt);
			}

			int stricmp(const std::string& a, const std::string& b)
			{
				int i = (int)b.length() - (int)a.length();
				return (i > 0 ? i : stricmp(a.c_str(), b.c_str()));
			}

			int stricmp(const std::wstring& a, const std::wstring& b)
			{
				int i = (int)b.length() - (int)a.length();
				return (i > 0 ? i : stricmp(a.c_str(), b.c_str()));
			}

			int stricmp(const char *s1, const char *s2)
			{
#if defined _VISUAL_STUDIO_
				return ::_stricmp(s1, s2);
#elif defined __GNUWIN__
				return ::strcasecmp(s1, s2);
#else
				char c1, c2;

				if (s1 == s2)
					return 0;

				do
				{
					c1 = tolower(*s1++);
					c2 = tolower(*s2++);
				} while (c1 && (c1 == c2));

				return c1 - c2;
#endif
			}

			int stricmp(const wchar_t *s1, const wchar_t *s2)
			{
#if defined _VISUAL_STUDIO_
				return ::_wcsicmp(s1, s2);
#elif defined __GNUWIN__
				return ::wcsicmp(s1, s2);
#else
				wint_t c1, c2;

				if (s1 == s2)
					return 0;

				do
				{
					c1 = tolower(*s1++);
					c2 = tolower(*s2++);
				} while (c1 && (c1 == c2));

				return c1 - c2;
#endif
			}

			int strnicmp(const char *s1, const char *s2, std::size_t n)
			{
#if defined _VISUAL_STUDIO_
				return ::_strnicmp(s1, s2, n);
#elif defined __GNUWIN__
				return ::strncasecmp(s1, s2, n);
#else
				char c1, c2;

				if (s1 == s2)
					return 0;

				std::size_t p = 0;
				do
				{
					if (p++ >= n)
						return 0;

					c1 = tolower(*s1++);
					c2 = tolower(*s2++);
				} while (c1 && (c1 == c2));

				return c1 - c2;
#endif
			}

			int strnicmp(const wchar_t *s1, const wchar_t *s2, std::size_t n)
			{
#if defined _VISUAL_STUDIO_
				return ::_wcsnicmp(s1, s2, n);
#else
				std::wint_t c1, c2;
				std::size_t p = 0;

				if (s1 == s2)
					return 0;

				do
				{
					if (p++ >= n)
						return 0;

					c1 = tolower(*s1++);
					c2 = tolower(*s2++);
				} while (c1 && (c1 == c2));

				return c1 - c2;
#endif
			}

			char* strcat(char *s1, const char *s2)
			{
				return std::strcat(s1, s2);
			}

			wchar_t* strcat(wchar_t *s1, const wchar_t *s2)
			{
				return std::wcscat(s1, s2);
			}

			char* strcat(std::string& a, const std::string& b)
			{
				a += b;
				return (char*)a.data();
			}

			wchar_t* strcat(std::wstring& a, const std::wstring& b)
			{
				a += b;
				return (wchar_t*)a.data();
			}

			template <typename char_type, typename value_type>
			std::size_t itoa10real_move(char_type* out, std::size_t max, value_type number)
			{
				assert(nullptr != out);

				// write the unary minus to indicate we have a negative number
				std::size_t written = 1u;
				if (number < 0 && written < max)
				{
					*out++ = '-';
					++written;
					number = -number;
				}

				// We begin with the largest number that is not zero.
				auto cur = (value_type)itoa10_table[sizeof(value_type)];
				bool mustPrint = false;
				while (written < max)
				{
					auto digit = number / cur;
					if (mustPrint || digit > 0 || 1 == cur)
					{
						// print all future zeroes from now
						mustPrint = true;

						*out++ = static_cast<char_type>('0' + digit);

						++written;
						number -= digit * cur;
						if (1 == cur) {
							break;
						}
					}
					cur /= 10;
				}

				// append a terminal zero
				*out++ = '\0';
				return written - 1;
			}

			template <typename char_type, typename value_type>
			std::size_t uitoa10real_move(char_type* out, std::size_t max, value_type number)
			{
				assert(nullptr != out);

				// write the unary minus to indicate we have a negative number
				std::size_t written = 1u;

				// We begin with the largest number that is not zero.
				auto cur = (value_type)uitoa10_table[sizeof(value_type)];
				bool mustPrint = false;
				while (written < max)
				{
					auto digit = number / cur;
					if (mustPrint || digit > 0 || 1 == cur)
					{
						// print all future zeroes from now
						mustPrint = true;

						*out++ = static_cast<char_type>('0' + digit);

						++written;
						number -= digit * cur;
						if (1 == cur) {
							break;
						}
					}
					cur /= 10;
				}

				// append a terminal zero
				*out++ = '\0';
				return written - 1;
			}

			std::size_t itoa10(char* out, std::size_t max, char number)
			{
				return itoa10real_move(out, max, number);
			}

			std::size_t itoa10(wchar_t* out, std::size_t max, char number)
			{
				return itoa10real_move(out, max, number);
			}

			std::size_t itoa10(char* out, std::size_t max, unsigned char number)
			{
				return uitoa10real_move(out, max, number);
			}

			std::size_t itoa10(wchar_t* out, std::size_t max, unsigned char number)
			{
				return uitoa10real_move(out, max, number);
			}

			std::size_t itoa10(char* out, std::size_t max, short number)
			{
				return itoa10real_move(out, max, number);
			}

			std::size_t itoa10(wchar_t* out, std::size_t max, short number)
			{
				return itoa10real_move(out, max, number);
			}

			std::size_t itoa10(char* out, std::size_t max, unsigned short number)
			{
				return uitoa10real_move(out, max, number);
			}

			std::size_t itoa10(wchar_t* out, std::size_t max, unsigned short number)
			{
				return uitoa10real_move(out, max, number);
			}

			std::size_t itoa10(char* out, std::size_t max, int number)
			{
				return itoa10real_move(out, max, number);
			}

			std::size_t itoa10(wchar_t* out, std::size_t max, int number)
			{
				return itoa10real_move(out, max, number);
			}

			std::size_t itoa10(char* out, std::size_t max, unsigned int number)
			{
				return uitoa10real_move(out, max, number);
			}

			std::size_t itoa10(wchar_t* out, std::size_t max, unsigned int number)
			{
				return uitoa10real_move(out, max, number);
			}

			std::size_t itoa10(char* out, std::size_t max, long number)
			{
				return itoa10real_move(out, max, number);
			}

			std::size_t itoa10(wchar_t* out, std::size_t max, long number)
			{
				return itoa10real_move(out, max, number);
			}

			std::size_t itoa10(char* out, std::size_t max, unsigned long number)
			{
				return uitoa10real_move(out, max, number);
			}

			std::size_t itoa10(wchar_t* out, std::size_t max, unsigned long number)
			{
				return uitoa10real_move(out, max, number);
			}

			std::size_t itoa10(char* out, std::size_t max, long long number)
			{
				return itoa10real_move(out, max, number);
			}

			std::size_t itoa10(wchar_t* out, std::size_t max, long long number)
			{
				return itoa10real_move(out, max, number);
			}

			std::size_t itoa10(char* out, std::size_t max, unsigned long long number)
			{
				return uitoa10real_move(out, max, number);
			}

			std::size_t itoa10(wchar_t* out, std::size_t max, unsigned long long number)
			{
				return uitoa10real_move(out, max, number);
			}

			std::size_t strtoul10(const char* in, const char** out)
			{
				std::size_t value = 0;

				bool running = true;
				while (running)
				{
					if (*in < '0' || *in > '9')
						break;

					value = (value * 10) + (*in - '0');
					++in;
				}

				if (out) *out = in;

				return value;
			}

			std::size_t strtoul8(const char* in, const char** out)
			{
				unsigned int value = 0;

				bool running = true;
				while (running)
				{
					if (*in < '0' || *in > '7')
						break;

					value = (value << 3) + (*in - '0');
					++in;
				}
				if (out)*out = in;
				return value;
			}

			std::size_t strtoul16(const char* in, const char** out)
			{
				unsigned int value = 0;

				bool running = true;
				while (running)
				{
					if (*in >= '0' && *in <= '9')
					{
						value = (value << 4u) + (*in - '0');
					}
					else if (*in >= 'A' && *in <= 'F')
					{
						value = (value << 4u) + (*in - 'A') + 10;
					}
					else if (*in >= 'a' && *in <= 'f')
					{
						value = (value << 4u) + (*in - 'a') + 10;
					}
					else break;
					++in;
				}
				if (out)*out = in;
				return value;
			}

			std::intptr_t strtol10(const char* in, const char** out = 0)
			{
				bool inv = (*in == '-');
				if (inv || *in == '+')
					++in;

				std::intptr_t value = strtoul10(in, out);
				if (inv) value = -value;

				return value;
			}

			std::size_t HexDigitToDecimal(char in)
			{
				unsigned int out = UINT_MAX;
				if (in >= '0' && in <= '9')
					out = (unsigned int)in - '0';

				else if (in >= 'a' && in <= 'f')
					out = 10u + in - 'a';

				else if (in >= 'A' && in <= 'F')
					out = 10u + in - 'A';

				// return value is UINT_MAX if the input is not a hex digit
				return out;
			}

			std::uint8_t HexOctetToDecimal(const char* in)
			{
				return (std::uint8_t)(((std::uint8_t)HexDigitToDecimal(in[0]) << 4) + (std::uint8_t)HexDigitToDecimal(in[1]));
			}

			// ------------------------------------------------------------------------------------
			// Parse a C++-like integer literal - hex and oct prefixes.
			// 0xNNNN - hex
			// 0NNN   - oct
			// NNN    - dec
			// ------------------------------------------------------------------------------------
			std::size_t strtoul_cppstyle(const char* in, const char** out = 0)
			{
				if ('0' == in[0])
				{
					return 'x' == in[1] ? strtoul16(in + 2, out) : strtoul8(in + 1, out);
				}
				return strtoul10(in, out);
			}

			// ------------------------------------------------------------------------------------
			// Special version of the function, providing higher accuracy and safety
			// It is mainly used by fast_atof to prevent ugly and unwanted integer overflows.
			// ------------------------------------------------------------------------------------
			std::uint64_t strtoul10_64(const char* in, const char** out = 0, unsigned int* max_inout = 0)
			{
				unsigned int cur = 0;
				uint64_t value = 0;

				bool running = true;
				while (running)
				{
					if (*in < '0' || *in > '9')
						break;

					const uint64_t new_value = (value * 10) + (*in - '0');

					if (new_value < value) /* numeric overflow, we rely on you */
						return value;

					value = new_value;

					++in;
					++cur;

					if (max_inout && *max_inout == cur) {
						if (out) { /* skip to end */
							while (*in >= '0' && *in <= '9')
								++in;
							*out = in;
						}

						return value;
					}
				}

				if (out)
					*out = in;

				if (max_inout)
					*max_inout = cur;

				return value;
			}

			std::size_t ftoa10(char* out, std::size_t max, float number)
			{
				int decPoint = 0;

				// write the unary minus to indicate we have a negative number
				std::size_t written = 1u;

				if (number < 0.0f && written < max)
				{
					*out++ = '-';
					++written;
					number = -number;
				}

				if (number == 0.0f)
				{
					*out++ = '0';
					*out++ = '\0';
					return written;
				}

				// calculating decimal point position.
				while (number >= 10.0f)
				{
					decPoint++;
					number /= 10;
				}

				std::size_t loopMax = max < 6 ? max : 6;

				for (std::size_t loop = 0; loop < loopMax; loop++)
				{
					if (number == 0.0f)
					{
						if (loop == 1)
						{
							*out++ = '0';
							written++;
						}
						break;
					}

					char tmp = (char)number;
					number -= tmp;
					number *= 10.0f;

					*out++ = '0' + tmp;

					if (loop == 0)
					{
						*out++ = '.';
					}

					written++;
				}

				if (decPoint > 0)
				{
					char buf[PATHLIMIT];
					auto nums = itoa10(buf, PATHLIMIT, decPoint);
					if (written + nums + 2 < max)
					{
						*out++ = 'e';
						*out++ = '+';
						std::memcpy(out, buf, nums);
						out += nums;

						written += nums + 2;
					}
				}

				// append a terminal zero
				*out++ = '\0';

				return written;
			}

			std::size_t dtoa10(char* out, std::size_t max, double number)
			{
				int decPoint = 0;

				// write the unary minus to indicate we have a negative number
				std::size_t written = 1u;

				if (number < 0 && written < max)
				{
					*out++ = '-';
					++written;
					number = -number;
				}

				if (number == 0.0)
				{
					*out++ = '0';
					*out++ = '\0';
					return written;
				}

				// calculating decimal point position.
				while (number >= 10.0)
				{
					decPoint++;
					number /= 10;
				}

				std::size_t loopMax = max < 6 ? max : 6;

				for (std::size_t loop = 0; loop < loopMax; loop++)
				{
					if (number == 0.0)
					{
						if (loop == 1)
						{
							*out++ = '0';
							written++;
						}
						break;
					}

					char tmp = (char)number;
					number -= tmp;
					number *= 10.0;

					*out++ = '0' + tmp;

					if (loop == 0)
					{
						*out++ = '.';
					}

					written++;
				}

				if (decPoint > 0)
				{
					char buf[PATHLIMIT];
					auto nums = itoa10(buf, PATHLIMIT, decPoint);
					if (written + nums + 2 < max)
					{
						*out++ = 'e';
						*out++ = '+';
						std::memcpy(out, buf, nums);
						out += nums;

						written += nums + 2;
					}
				}

				// append a terminal zero
				*out++ = '\0';

				return written;
			}

#define FAST_ATOF_RELAVANT_DECIMALS 15

			template <typename Real>
			inline const char* fast_atoreal_move(const char* c, Real& out)
			{
				Real f;

				bool inv = (*c == '-');
				if (inv || *c == '+') {
					++c;
				}

				f = static_cast<Real>(strtoul10_64(c, &c));
				if (*c == '.' || (c[0] == ',' && c[1] >= '0' && c[1] <= '9')) // allow for commas, too
				{
					++c;

					// NOTE: The original implementation is highly inaccurate here. The precision of a single
					// IEEE 754 float is not high enough, everything behind the 6th digit tends to be more
					// inaccurate than it would need to be. Casting to double seems to solve the problem.
					// strtol_64 is used to prevent integer overflow.

					// Another fix: this tends to become 0 for long numbers if we don't limit the maximum
					// number of digits to be read. FAST_ATOF_RELAVANT_DECIMALS can be a value between
					// 1 and 15.
					unsigned int diff = FAST_ATOF_RELAVANT_DECIMALS;
					double pl = static_cast<double>(strtoul10_64(c, &c, &diff));

					pl *= fast_atof_table[diff];
					f += static_cast<Real>(pl);
				}

				// A major 'E' must be allowed. Necessary for proper reading of some DXF files.
				// Thanks to Zhao Lei to point out that this if() must be outside the if (*c == '.' ..)
				if (*c == 'e' || *c == 'E') {
					++c;
					const bool einv = (*c == '-');
					if (einv || *c == '+') {
						++c;
					}

					// The reason float constants are used here is that we've seen cases where compilers
					// would perform such casts on compile-time constants at runtime, which would be
					// bad considering how frequently fast_atoreal_move<float> is called in Assimp.
					Real exp = static_cast<Real>(strtoul10_64(c, &c));
					if (einv) {
						exp = -exp;
					}
					f *= pow(static_cast<Real>(10.0f), exp);
				}

				if (inv) {
					f = -f;
				}
				out = f;
				return c;
			}

			float fast_atof(const char* c)
			{
				float ret;
				fast_atoreal_move<float>(c, ret);
				return ret;
			}

			float fast_atof(const char* c, const char** cout)
			{
				float ret;
				*cout = fast_atoreal_move<float>(c, ret);

				return ret;
			}

			float fast_atof(const char** inout)
			{
				float ret;
				*inout = fast_atoreal_move<float>(*inout, ret);

				return ret;
			}

			double fast_atod(const char* c)
			{
				double ret;
				fast_atoreal_move<double>(c, ret);
				return ret;
			}

			double fast_atod(const char* c, const char** cout)
			{
				double ret;
				*cout = fast_atoreal_move<double>(c, ret);

				return ret;
			}

			double fast_atod(const char** inout)
			{
				double ret;
				*inout = fast_atoreal_move<double>(*inout, ret);

				return ret;
			}

			bool isHex(char in)
			{
				return (in >= '0' && in <= '9') || (in >= 'a' && in <= 'f') || (in >= 'A' && in <= 'F');
			}

			char tolower(char in)
			{
				return (in >= 'A' && in <= 'Z') ? (in + 0x20) : in;
			}

			char toUpper(char in)
			{
				return (in >= 'a' && in <= 'z') ? (in - 0x20) : in;
			}

			bool isUpper(char in)
			{
				return (in >= 'A' && in <= 'Z');
			}

			bool isLower(char in)
			{
				return (in >= 'a' && in <= 'z');
			}

			bool isSpace(char in)
			{
				return (in == ' ' || in == '\t');
			}

			bool isSeparator(char in)
			{
				return (in == '/' || in == '\\');
			}

			bool isNewLine(char in)
			{
				return (in == '\n' || in == '\r' || in == '\f' || in == '\v');
			}

			bool isLineEnd(char in)
			{
				return (in == '\r' || in == '\n' || in == '\0');
			}

			bool isNumeric(char in)
			{
				return (in >= '0' && in <= '9') || '-' == in || '+' == in;
			}

			bool isSpaceOrNewLine(char in)
			{
				return isSpace(in) || isNewLine(in);
			}

			bool isOperatorSymbol(char c)
			{
				switch (c)
				{
				case ';':
				case ':':
				case '(':
				case ')':
				case '[':
				case ']':
				case '{':
				case '}':
				case '-':
				case '+':
				case '*':
				case '/':
				case '?':
				case '!':
				case ',':
				case '=':
				case '.':
				case '<': case '>':
					return true;
				}
				return false;
			}

			bool isEndOfStream(char* it, char* end)
			{
				if (it == end)
					return true;
				else
					end--;
				return (it == end);
			}

			bool isHex(wchar_t in)
			{
				return (in >= '0' && in <= '9') || (in >= 'a' && in <= 'f') || (in >= 'A' && in <= 'F');
			}

			wchar_t tolower(wchar_t in)
			{
				return (wchar_t)((in >= 'A' && in <= 'Z') ? (in + 0x20) : in);
			}

			wchar_t toUpper(wchar_t in)
			{
				return (wchar_t)((in >= 'a' && in <= 'z') ? (in - 0x20) : in);
			}

			bool isUpper(wchar_t in)
			{
				return (in >= 'A' && in <= 'Z');
			}

			bool isLower(wchar_t in)
			{
				return (in >= 'a' && in <= 'z');
			}

			bool isSpace(wchar_t in)
			{
				return (in == ' ' || in == '\t');
			}

			bool isSeparator(wchar_t in)
			{
				return (in == '/' || in == '\\');
			}

			bool isNewLine(wchar_t in)
			{
				return (in == '\n' || in == '\f' || in == '\r');
			}

			bool isLineEnd(wchar_t in)
			{
				return (in == '\r' || in == '\n' || in == '\0');
			}

			bool isNumeric(wchar_t in)
			{
				return (in >= '0' && in <= '9') || '-' == in || '+' == in;
			}

			bool isOperatorSymbol(wchar_t c)
			{
				switch (c)
				{
				case ';':
				case ':':
				case '(': case ')':
				case '[': case ']':
				case '{': case '}':
				case '-': case '+':
				case '*': case '/':
				case '?':
				case '!':
				case ',':
				case '=':
				case '.':
				case '<': case '>':
					return true;
				}
				return false;
			}

			bool isSpaceOrNewLine(wchar_t in)
			{
				return isSpace(in) || isLineEnd(in);
			}

			bool isEndOfStream(wchar_t* it, wchar_t* end)
			{
				if (it == end)
					return true;
				else
					end--;
				return (it == end);
			}

			bool skipSpaces(const char* in, const char** out)
			{
				while (isSpace(*in))
					in++;

				*out = in;
				return !isLineEnd(*in);
			}

			bool skipSpaces(const char** inout)
			{
				return skipSpaces(*inout, inout);
			}

			char* skipSpaces(char* it, char* end, std::size_t& num)
			{
				std::size_t cnt = 0;

				while (it != end && isSpace(*it))
				{
					++it;
					++cnt;
				}

				num = cnt;

				return it;
			}

			char* skipLine(char* it, char* end, std::size_t& num)
			{
				std::size_t cnt = 0;

				while (!isEndOfStream(it, end) && !isNewLine(*it))
				{
					++it;
					++cnt;
				}

				while (it != end && isNewLine(*it))
				{
					++it;
					++cnt;
				}

				num = cnt;

				return it;
			}

			char* skipSeparator(char* in)
			{
				while (*in != 0 || *in != '/' || *in != '\\')
					in++;

				return in;
			}

			bool skipSpacesAndLineEnd(const char* in, const char** out)
			{
				while (isSpaceOrNewLine(*in))
					++in;

				*out = in;
				return *in != '\0';
			}

			bool skipSpacesAndLineEnd(const wchar_t* in, const wchar_t** out)
			{
				while (isSpaceOrNewLine(*in))
					++in;

				*out = in;
				return *in != '\0';
			}

			bool skipSpacesAndLineEnd(const char* in, const char** out, std::size_t numLines)
			{
				while (isSpaceOrNewLine(*in))
				{
					if (*in == '\n' || *in == '\v' || *in == '\f')
						++numLines;
					++in;
				}

				*out = in;
				return *in != '\0';
			}

			bool skipSpacesAndLineEnd(const wchar_t* in, const wchar_t** out, std::size_t numLines)
			{
				while (isSpaceOrNewLine(*in))
				{
					if (*in == '\n' || *in == '\v' || *in == '\f')
						++numLines;
					++in;
				}

				*out = in;
				return *in != '\0';
			}

			bool skipSpacesAndLineEnd(const char** inout)
			{
				return skipSpacesAndLineEnd(*inout, inout);
			}

			bool skipSpacesAndLineEnd(const wchar_t** inout)
			{
				return skipSpacesAndLineEnd(*inout, inout);
			}

			bool getNextLine(const char*& buffer, char out[4096])
			{
				if ('\0' == *buffer)return false;

				char* _out = out;
				char* const end = _out + 4096;
				while (!isLineEnd(*buffer) && _out < end)
					*_out++ = *buffer++;
				*_out = '\0';

				while (isLineEnd(*buffer) && '\0' != *buffer)++buffer;
				return true;
			}

			bool tokenMatch(char*& in, const char* token, std::size_t len)
			{
				if (!strncmp(token, in, len) && isSpaceOrNewLine(in[len]))
				{
					in += len + 1;
					return true;
				}
				return false;
			}

			bool tokenMatchI(const char*& in, const char* token, std::size_t len)
			{
				if (!strnicmp(token, in, len) && isSpaceOrNewLine(in[len]))
				{
					in += len + 1;
					return true;
				}
				return false;
			}

			std::string directory(const std::string& path)
			{
				auto begin = path.rbegin();
				auto end = path.rend();

				std::size_t length = 0;

				for (; begin != end; ++begin)
				{
					if (isSeparator(*begin))
						break;

					length++;
				}

				return std::string(path.c_str(), path.size() - length);
			}

			std::wstring directory(const std::wstring& path)
			{
				auto begin = path.rbegin();
				auto end = path.rend();

				std::size_t length = 0;

				for (; begin != end; ++begin)
				{
					if (isSeparator(*begin))
						break;

					length++;
				}

				return std::wstring(path.c_str(), path.size() - length);
			}

#if __WINDOWS__
			std::string filename(std::string::const_pointer path)
			{
				char drive[3];
				char dir[MAX_PATH];
				char filename[MAX_PATH];
				char ext[MAX_PATH];
				::_splitpath_s(path, drive, 3, dir, MAX_PATH, filename, MAX_PATH, ext, MAX_PATH);

				return filename;
			}
#endif

#if __WINDOWS__
			std::wstring filename(std::wstring::const_pointer path)
			{
				wchar_t drive[3];
				wchar_t dir[MAX_PATH];
				wchar_t filename[MAX_PATH];
				wchar_t ext[MAX_PATH];
				::_wsplitpath_s(path, drive, 3, dir, MAX_PATH, filename, MAX_PATH, ext, MAX_PATH);

				return filename;
			}
#endif

			std::size_t ext_name(const char* in, char* out, std::size_t maxLength)
			{
				auto ext = std::strrchr(in, '.');
				if (ext)
				{
					ext++;

					std::size_t count = maxLength;
					while (count > 1 && *ext != '\0')
					{
						*out++ = *ext++;
						count--;
					}

					*out++ = '\0';
					return maxLength - count;
				}

				return 0;
			}

			std::size_t ext_name(const wchar_t* in, wchar_t* out, std::size_t maxLength)
			{
				auto ext = std::wcschr(in, L'.');
				if (ext)
				{
					std::size_t count = maxLength;
					while (count > 1 && *ext != L'\0')
					{
						*out++ = *ext++;
						count--;
					}

					*out++ = '\0';
					return maxLength - count;
				}

				return 0;
			}

			std::size_t toUnixPath(const char*in, char* out, std::size_t maxLength)
			{
				std::size_t count = 0;

				while (in[count])
				{
					if (isSeparator(in[count]))
						out[count++] = '/';
					else
						out[count++] = in[count];
				}

				out[count] = 0;

				return count;
			}

			std::size_t toUnixPath(const wchar_t*in, wchar_t* out, std::size_t maxLength)
			{
				std::size_t count = 0;

				while (in[count])
				{
					if (isSeparator(in[count]))
						out[count++] = L'/';
					else
						out[count++] = in[count];
				}

				out[count] = 0;

				return count;
			}

			void skipToken(const char*& in)
			{
				skipSpaces(&in);
				while (!isSpaceOrNewLine(*in))++in;
			}

			std::string getNextToken(const char*& in)
			{
				skipSpacesAndLineEnd(&in);
				const char* cur = in;
				while (!isSpaceOrNewLine(*in))++in;
				return std::string(cur, (size_t)(in - cur));
			}

			std::wstring getNextToken(const wchar_t*& in)
			{
				skipSpacesAndLineEnd(&in);
				const wchar_t* cur = in;
				while (!isSpaceOrNewLine(*in))++in;
				return std::wstring(cur, (size_t)(in - cur));
			}

			std::string toLower(const std::string& _input)
			{
				std::string result;
				result.resize(_input.size());
				static std::locale sLocale("");
				for (unsigned int i = 0; i < _input.size(); ++i)
					result[i] = std::tolower(_input[i], sLocale);
				return result;
			}

			std::wstring toLower(const std::wstring& _input)
			{
				std::wstring result;
				result.resize(_input.size());
				static std::locale sLocale("");
				for (unsigned int i = 0; i < _input.size(); ++i)
					result[i] = std::tolower(_input[i], sLocale);
				return result;
			}

			void split(std::vector<std::string>& result, const std::string& _source, const std::string& _delims)
			{
				std::size_t start = _source.find_first_not_of(_delims);
				while (start != _source.npos)
				{
					std::size_t end = _source.find_first_of(_delims, start);
					if (end != _source.npos)
						result.push_back(_source.substr(start, end - start));
					else
					{
						result.push_back(_source.substr(start));
						break;
					}
					start = _source.find_first_not_of(_delims, end + 1);
				}
			}

			void split(std::vector<std::wstring>& result, const std::wstring& _source, const std::wstring& _delims)
			{
				std::size_t start = _source.find_first_not_of(_delims);
				while (start != _source.npos)
				{
					std::size_t end = _source.find_first_of(_delims, start);
					if (end != _source.npos)
						result.push_back(_source.substr(start, end - start));
					else
					{
						result.push_back(_source.substr(start));
						break;
					}
					start = _source.find_first_not_of(_delims, end + 1);
				}
			}

			std::string ltrim(const std::string& str, char ch)
			{
				std::size_t len = str.size();
				for (std::size_t i = 0; i < len; i++)
				{
					if (str[i] != ch)
						break;
				}

				return str.substr(len);
			}

			std::wstring ltrim(const std::wstring& str, wchar_t ch)
			{
				std::size_t len = str.size();
				for (std::size_t i = 0; i < len; i++)
				{
					if (str[i] != ch)
						break;
				}

				return str.substr(len);
			}

			std::string rtrim(const std::string& str, char ch)
			{
				std::size_t len = str.size();
				for (std::size_t i = len - 1; i > 0; i++)
				{
					if (str[i] != ch)
						break;
				}

				return str.substr(0, len + 1);
			}

			std::wstring rtrim(const std::wstring& str, wchar_t ch)
			{
				std::size_t len = str.size();
				for (std::size_t i = len - 1; i > 0; i++)
				{
					if (str[i] != ch)
						break;
				}

				return str.substr(0, len + 1);
			}

			std::string trim(const std::string& str, char ch)
			{
				std::size_t len = str.size();

				std::size_t off1 = 0;
				std::size_t off2 = len - 1;
				for (; off1 < len; off1++)
				{
					if (str[off1] != ch)
						break;
				}

				for (; off2 > 0; off2--)
				{
					if (str[off2] != ch)
						break;
				}

				if ((off2 + 1) > off1)
					return str.substr(off1, (off2 + 1) - off1);
				else
					return str;
			}

			std::wstring trim(const std::wstring& str, wchar_t ch)
			{
				std::size_t len = str.size();

				std::size_t off1 = 0;
				std::size_t off2 = len - 1;
				for (; off1 < len; off1++)
				{
					if (str[off1] != ch)
						break;
				}

				for (; off2 > 0; off2--)
				{
					if (str[off2] != ch)
						break;
				}

				if ((off2 + 1) > off1)
					return str.substr(off1, (off2 + 1) - off1);
				else
					return str;
			}

			std::size_t hex2str(std::string& out, const char* in, std::size_t size, std::size_t split)
			{
				out.resize(size * 3 + size / split);

				std::size_t i = 0, j = 0, k = 1;
				for (; i < size; i++, k++)
				{
					std::uint8_t ch = in[i];
					out[j] = "0123456789abcdef"[(ch >> 4) & 0x0f];
					out[j + 1] = "0123456789abcdef"[ch & 0x0f];
					out[j + 2] = ' ';

					if (k == split)
					{
						out[j + 3] = '\n';
						k = 0;
						j += 4;
					}
					else
					{
						j += 3;
					}
				}

				return i;
			}

			bool _str2hex(char in, char& out)
			{
				if (in < '0' || in > 'f')
					return false;
				if (in > '9' && in < 'a')
					return false;

				out = in;
				if (out >= 'a')
					out -= ('a' - 10);
				else
					out -= '0';
				return true;
			}

			std::size_t str2hex(std::vector<char>& out, const char* in, std::size_t size)
			{
				std::size_t i = 0;
				while (i < size)
				{
					char high, low;
					if (!_str2hex(in[i], high)) { i++; continue; }
					if (!_str2hex(in[i + 1], low)) { i++; continue; }

					out.push_back(high << 4 | low);
					i += 2;
				}

				return out.size();
			}
		}
	}
}