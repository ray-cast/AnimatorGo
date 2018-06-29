#ifndef SOLID_HELPER_H_
#define SOLID_HELPER_H_

#include "keyframe_helper.h"

class SoildHelper
{
public:
	using json = octoon::runtime::json;

	using float1 = octoon::math::float1;
	using float2 = octoon::math::float2;
	using float3 = octoon::math::float3;
	using float4 = octoon::math::float4;

	std::uint32_t sw;
	std::uint32_t sh;
	float3 color;

	SoildHelper()
	{
	}

	SoildHelper(const json& j)
	{
		this->prepare(j);
	}

	std::vector<char> str2hex(const char* in, std::size_t size)
	{
		auto  _str2hex = [](char in, char& out) -> bool 
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
		};

		std::size_t i = 0;
		std::vector<char> out;

		while (i < size)
		{
			char high, low;
			if (!_str2hex(in[i], high)) { i++; continue; }
			if (!_str2hex(in[i + 1], low)) { i++; continue; }

			out.push_back(high << 4 | low);
			i += 2;
		}

		return out;
	}

	void prepare(const json& layer)
	{
		sw = layer["sw"].get <json::number_unsigned_t>();
		sh = layer["sh"].get <json::number_unsigned_t>();
		auto sc = layer["sc"].get <json::string_t>();

		if (sc.front() == '#')
		{
			auto c = str2hex(sc.data(), sc.size());
			if (c.size() == 3)
			{
				color.x = c[0] / 255.0f;
				color.y = c[1] / 255.0f;
				color.z = c[2] / 255.0f;
			}
		}
	}
};

#endif