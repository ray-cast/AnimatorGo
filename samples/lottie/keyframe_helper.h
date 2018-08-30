#ifndef KEYFRAME_HELPER_H_
#define KEYFRAME_HELPER_H_

#include <octoon/runtime/json.h>
#include <octoon/model/keyframe.h>

class KeyframeHelper
{
public:
	using json = octoon::runtime::json;

	using float1 = octoon::math::float1;
	using float2 = octoon::math::float2;
	using float3 = octoon::math::float3;
	using float4 = octoon::math::float4;

	using float1s = octoon::model::Keyframes<float1>;
	using float2s = octoon::model::Keyframes<float2>;
	using float3s = octoon::model::Keyframes<float3>;
	using float4s = octoon::model::Keyframes<float4>;

	KeyframeHelper() noexcept
	{
	}

	static float1s preparefloat1(const json& j, float weight = 1.0f)
	{
		float1s key;

		if (!j.is_object())
		{
			key.emplace_back(0.0f, float1(j.get<json::number_float_t>() * weight));
			return key;
		}

		auto& k = j["k"];

		if (j["a"].get<json::number_unsigned_t>())
		{
			for (auto it = k.begin(); it != k.end(); ++it)
			{
				if ((it + 2) == k.end())
				{
					auto& s = (*it)["s"];
					auto& e = (*it)["e"];

					auto sTime = (*it)["t"].get<json::number_float_t>();
					auto eTime = (*(it + 1))["t"].get<json::number_float_t>();

					key.emplace_back(sTime, float1(s[0].get<json::number_float_t>() * weight));
					key.emplace_back(eTime, float1(e[0].get<json::number_float_t>() * weight));
					break;
				}
				else
				{
					auto& s = (*it)["s"];
					key.emplace_back((*it)["t"].get<json::number_float_t>(), float1(s[0].get<json::number_float_t>() * weight));
				}
			}
		}
		else
		{
			key.emplace_back(0.0f, float1(k.get<json::number_float_t>() * weight));
		}

		return key;
	}

	static float2s preparefloat2(const json& j, float weight = 1.0f)
	{
		float2s key;

		auto& k = j["k"];

		if (j["a"].get<json::number_unsigned_t>())
		{
			for (auto it = k.begin(); it != k.end(); ++it)
			{
				if ((it + 2) == k.end())
				{
					auto& s = (*it)["s"];
					auto& e = (*it)["e"];

					auto sTime = (*it)["t"].get<json::number_float_t>();
					auto eTime = (*(it + 1))["t"].get<json::number_float_t>();

					key.emplace_back(sTime, float2(s[0].get<json::number_float_t>() * weight, s[1].get<json::number_float_t>() * weight));
					key.emplace_back(eTime, float2(e[0].get<json::number_float_t>() * weight, e[1].get<json::number_float_t>() * weight));
					break;
				}
				else
				{
					auto& s = (*it)["s"];
					key.emplace_back((*it)["t"].get<json::number_float_t>(), float2(s[0].get<json::number_float_t>() * weight, s[1].get<json::number_float_t>() * weight));
				}
			}
		}
		else
		{
			key.emplace_back(0.0f, float2(k[0].get<json::number_float_t>() * weight, k[1].get<json::number_float_t>() * weight));
		}

		return key;
	}

	static float3s preparefloat3(const json& j, float weight = 1.0f)
	{
		float3s key;

		auto& k = j["k"];

		if (j["a"].get<json::number_unsigned_t>())
		{
			for (auto it = k.begin(); it != k.end(); ++it)
			{
				if ((it + 2) == k.end())
				{
					auto& s = (*it)["s"];
					auto& e = (*it)["e"];

					auto sTime = (*it)["t"].get<json::number_float_t>();
					auto eTime = (*(it + 1))["t"].get<json::number_float_t>();

					key.emplace_back(sTime, float3(s[0].get<json::number_float_t>() * weight, s[1].get<json::number_float_t>() * weight, s[2].get<json::number_float_t>() * weight));
					key.emplace_back(eTime, float3(e[0].get<json::number_float_t>() * weight, e[1].get<json::number_float_t>() * weight, e[2].get<json::number_float_t>() * weight));
					break;
				}
				else
				{
					auto& s = (*it)["s"];
					key.emplace_back((*it)["t"].get<json::number_float_t>(), float3(s[0].get<json::number_float_t>() * weight, s[1].get<json::number_float_t>() * weight, s[2].get<json::number_float_t>() * weight));
				}
			}
		}
		else
		{
			key.emplace_back(0.0f, float3(k[0].get<json::number_float_t>() * weight, k[1].get<json::number_float_t>() * weight, k[2].get<json::number_float_t>() * weight));
		}

		return key;
	}

	static float4s preparefloat4(const json& j, float weight = 1.0f)
	{
		float4s key;

		auto& k = j["k"];

		if (j["a"].get<json::number_unsigned_t>())
		{
			for (auto it = k.begin(); it != k.end(); ++it)
			{
				if ((it + 2) == k.end())
				{
					auto& s = (*it)["s"];
					auto& e = (*it)["e"];

					auto sTime = (*it)["t"].get<json::number_float_t>();
					auto eTime = (*(it + 1))["t"].get<json::number_float_t>();

					key.emplace_back(sTime, float4(s[0].get<json::number_float_t>() * weight, s[1].get<json::number_float_t>() * weight, s[2].get<json::number_float_t>() * weight, s[3].get<json::number_float_t>() * weight));
					key.emplace_back(eTime, float4(e[0].get<json::number_float_t>() * weight, e[1].get<json::number_float_t>() * weight, e[2].get<json::number_float_t>() * weight, e[3].get<json::number_float_t>() * weight));
					break;
				}
				else
				{
					auto& s = (*it)["s"];
					key.emplace_back((*it)["t"].get<json::number_float_t>(), float4(s[0].get<json::number_float_t>() * weight, s[1].get<json::number_float_t>() * weight, s[2].get<json::number_float_t>() * weight, s[3].get<json::number_float_t>() * weight));
				}
			}
		}
		else
		{
			key.emplace_back(0.0f, float4(k[0].get<json::number_float_t>() * weight, k[1].get<json::number_float_t>() * weight, k[2].get<json::number_float_t>() * weight, k[3].get<json::number_float_t>() * weight));
		}

		return key;
	}
};

#endif