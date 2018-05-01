#ifndef OCTOON_STRING_H_
#define OCTOON_STRING_H_

#include <vector>
#include <string>

#include <octoon/runtime/platform.h>

#ifdef __UNICODE__
#   include <wchar.h>
#   include <wctype.h>
#endif

#if defined(__GNUWIN__)
#   include <strings.h>
#endif

namespace octoon
{
	namespace runtime
	{
		namespace string
		{
			OCTOON_EXPORT int strcmp(const char *s1, const char *s2);
			OCTOON_EXPORT int strcmp(const wchar_t *s1, const wchar_t *s2);
			OCTOON_EXPORT int strcmp(const std::string& a, const std::string& b);
			OCTOON_EXPORT int strcmp(const std::wstring& a, const std::wstring& b);

			OCTOON_EXPORT int stricmp(const char *s1, const char *s2);
			OCTOON_EXPORT int stricmp(const wchar_t *s1, const wchar_t *s2);
			OCTOON_EXPORT int stricmp(const std::string& a, const std::string& b);
			OCTOON_EXPORT int stricmp(const std::wstring& a, const std::wstring& b);
			OCTOON_EXPORT int strnicmp(const char *s1, const char *s2, std::size_t n);
			OCTOON_EXPORT int strnicmp(const wchar_t *s1, const wchar_t *s2, std::size_t n);

			OCTOON_EXPORT char* strcat(char *s1, const char *s2);
			OCTOON_EXPORT char* strcat(std::string& a, const std::string& b);
			OCTOON_EXPORT wchar_t* strcat(wchar_t *s1, const wchar_t *s2);
			OCTOON_EXPORT wchar_t* strcat(std::wstring& a, const std::wstring& b);

			OCTOON_EXPORT char* strstr(char* dest, const char* source);
			OCTOON_EXPORT char* strcpy(char* dest, const char* source);
			OCTOON_EXPORT char* strncpy(char* s1, const char* s2, std::size_t cnt);
			OCTOON_EXPORT wchar_t* strcpy(wchar_t* dest, const wchar_t* source);
			OCTOON_EXPORT wchar_t* strncpy(wchar_t* s1, const wchar_t* s2, std::size_t cnt);
			OCTOON_EXPORT wchar_t* strstr(wchar_t * dest, const wchar_t * source);
			OCTOON_EXPORT std::size_t strlen(const char* str);
			OCTOON_EXPORT std::size_t strlen(const wchar_t* str);
			OCTOON_EXPORT int strncmp(const char* s1, const char* s2, std::size_t cnt);
			OCTOON_EXPORT int strncmp(const wchar_t* s1, const wchar_t* s2, std::size_t cnt);

			OCTOON_EXPORT std::size_t itoa10(char* out, std::size_t max, char number);
			OCTOON_EXPORT std::size_t itoa10(char* out, std::size_t max, unsigned char number);
			OCTOON_EXPORT std::size_t itoa10(char* out, std::size_t max, short number);
			OCTOON_EXPORT std::size_t itoa10(char* out, std::size_t max, unsigned short number);
			OCTOON_EXPORT std::size_t itoa10(char* out, std::size_t max, int number);
			OCTOON_EXPORT std::size_t itoa10(char* out, std::size_t max, unsigned int number);
			OCTOON_EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, int number);
			OCTOON_EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, unsigned int number);
			OCTOON_EXPORT std::size_t itoa10(char* out, std::size_t max, long number);
			OCTOON_EXPORT std::size_t itoa10(char* out, std::size_t max, unsigned long number);
			OCTOON_EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, long number);
			OCTOON_EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, unsigned long number);
			OCTOON_EXPORT std::size_t itoa10(char* out, std::size_t max, long long number);
			OCTOON_EXPORT std::size_t itoa10(char* out, std::size_t max, unsigned long long number);
			OCTOON_EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, long long number);
			OCTOON_EXPORT std::size_t itoa10(wchar_t* out, std::size_t max, unsigned long long number);

			OCTOON_EXPORT std::size_t ftoa10(char* out, std::size_t max, float number);
			OCTOON_EXPORT std::size_t dtoa10(char* out, std::size_t max, double number);

			OCTOON_EXPORT float fast_atof(const char* c);
			OCTOON_EXPORT float fast_atof(const char* c, const char** cout);
			OCTOON_EXPORT float fast_atof(const char** inout);
			OCTOON_EXPORT double fast_atod(const char* c);
			OCTOON_EXPORT double fast_atod(const char* c, const char** cout);
			OCTOON_EXPORT double fast_atod(const char** inout);

			OCTOON_EXPORT std::size_t strtoul8(const char* in, const char** out);
			OCTOON_EXPORT std::size_t strtoul10(const char* in, const char** out);
			OCTOON_EXPORT std::size_t strtoul16(const char* in, const char** out);
			OCTOON_EXPORT std::intptr_t strtol10(const char* in, const char** out);
			OCTOON_EXPORT std::size_t strtoul_cppstyle(const char* in, const char** out);
			OCTOON_EXPORT std::uint64_t strtoul10_64(const char* in, const char** out, unsigned int* max_inout);
			OCTOON_EXPORT std::size_t HexDigitToDecimal(char in);
			OCTOON_EXPORT std::uint8_t HexOctetToDecimal(const char* in);

			OCTOON_EXPORT char tolower(char in);
			OCTOON_EXPORT char toUpper(char in);
			OCTOON_EXPORT wchar_t tolower(wchar_t in);
			OCTOON_EXPORT wchar_t toUpper(wchar_t in);
			OCTOON_EXPORT std::string toLower(const std::string& _input);
			OCTOON_EXPORT std::wstring toLower(const std::wstring& _input);
			OCTOON_EXPORT std::size_t toUnixPath(const char*in, char* out, std::size_t maxLength);
			OCTOON_EXPORT std::size_t toUnixPath(const wchar_t*in, wchar_t* out, std::size_t maxLength);

			OCTOON_EXPORT bool isHex(char in);
			OCTOON_EXPORT bool isUpper(char in);
			OCTOON_EXPORT bool isLower(char in);
			OCTOON_EXPORT bool isSpace(char in);
			OCTOON_EXPORT bool isSeparator(char in);
			OCTOON_EXPORT bool isNewLine(char in);
			OCTOON_EXPORT bool isLineEnd(char in);
			OCTOON_EXPORT bool isNumeric(char in);
			OCTOON_EXPORT bool isSpaceOrNewLine(char in);
			OCTOON_EXPORT bool isOperatorSymbol(char c);
			OCTOON_EXPORT bool isEndOfStream(char* it, char* end);

			OCTOON_EXPORT bool isHex(wchar_t in);
			OCTOON_EXPORT bool isUpper(wchar_t in);
			OCTOON_EXPORT bool isLower(wchar_t in);
			OCTOON_EXPORT bool isSpace(wchar_t in);
			OCTOON_EXPORT bool isSeparator(wchar_t in);
			OCTOON_EXPORT bool isNewLine(wchar_t in);
			OCTOON_EXPORT bool isLineEnd(wchar_t in);
			OCTOON_EXPORT bool isNumeric(wchar_t in);
			OCTOON_EXPORT bool isSpaceOrNewLine(wchar_t in);
			OCTOON_EXPORT bool isOperatorSymbol(wchar_t c);
			OCTOON_EXPORT bool isEndOfStream(wchar_t* it, wchar_t* end);

			OCTOON_EXPORT bool skipSpaces(const char* in, const char** out);
			OCTOON_EXPORT bool skipSpaces(const char** inout);
			OCTOON_EXPORT char* skipSpaces(char* it, char* end, std::size_t& num);
			OCTOON_EXPORT char* skipLine(char* it, char* end, std::size_t& num);
			OCTOON_EXPORT char* skipSeparator(char* in);

			OCTOON_EXPORT bool skipSpacesAndLineEnd(const char* in, const char** out);
			OCTOON_EXPORT bool skipSpacesAndLineEnd(const wchar_t* in, const wchar_t** out);
			OCTOON_EXPORT bool skipSpacesAndLineEnd(const char* in, const char** out, std::size_t numLines);
			OCTOON_EXPORT bool skipSpacesAndLineEnd(const wchar_t* in, const wchar_t** out, std::size_t numLines);
			OCTOON_EXPORT bool skipSpacesAndLineEnd(const char** inout);
			OCTOON_EXPORT bool skipSpacesAndLineEnd(const wchar_t** inout);

			OCTOON_EXPORT void skipToken(const char*& in);
			OCTOON_EXPORT bool getNextLine(const char*& buffer, char out[4096]);
			OCTOON_EXPORT bool tokenMatch(char*& in, const char* token, std::size_t len);
			OCTOON_EXPORT bool tokenMatchI(const char*& in, const char* token, std::size_t len);
			OCTOON_EXPORT std::string getNextToken(const char*& in);
			OCTOON_EXPORT std::wstring getNextToken(const wchar_t*& in);

			OCTOON_EXPORT std::string directory(const std::string& path);
			OCTOON_EXPORT std::wstring directory(const std::wstring& path);

			OCTOON_EXPORT std::string filename(std::string::const_pointer path);
			OCTOON_EXPORT std::wstring filename(std::wstring::const_pointer path);

			OCTOON_EXPORT std::size_t ext_name(const char*in, char* out, std::size_t maxLength);
			OCTOON_EXPORT std::size_t ext_name(const wchar_t*in, wchar_t* out, std::size_t maxLength);

			OCTOON_EXPORT void split(std::vector<std::string>& result, const std::string& _source, const std::string& _delims = "\t\n ");
			OCTOON_EXPORT void split(std::vector<std::wstring>& result, const std::wstring& _source, const std::wstring& _delims = L"\t\n ");

			OCTOON_EXPORT std::string ltrim(const std::string& str, char ch);
			OCTOON_EXPORT std::wstring ltrim(const std::wstring& str, wchar_t ch);

			OCTOON_EXPORT std::string rtrim(const std::string& str, char ch);
			OCTOON_EXPORT std::wstring rtrim(const std::wstring& str, wchar_t ch);

			OCTOON_EXPORT std::string trim(const std::string& str, char ch);
			OCTOON_EXPORT std::wstring trim(const std::wstring& str, wchar_t ch);

			OCTOON_EXPORT std::size_t hex2str(std::string& out, const char* in, std::size_t size, std::size_t split);
			OCTOON_EXPORT std::size_t str2hex(std::vector<char>& out, const char* in, std::size_t size);
		}
	}
}

#endif
