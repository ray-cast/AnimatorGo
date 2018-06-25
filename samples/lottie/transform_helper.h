#ifndef TRANSFORM_HELPER_H_
#define TRANSFORM_HELPER_H_

#include <octoon/runtime/json.h>
#include <octoon/model/keyframe.h>

class TransformHelper
{
public:
	using json = octoon::runtime::json;

	using float1 = octoon::math::float1;
	using float2 = octoon::math::float2;
	using float3 = octoon::math::float3;
	using float4 = octoon::math::float4;

	octoon::model::Keyframes<float3> pos;
	octoon::model::Keyframes<float3> scale;
	octoon::model::Keyframes<float3> rotation;
	octoon::model::Keyframes<float1> opacity;

	TransformHelper()
	{
	}

	TransformHelper(const json& j, float fps)
	{
		this->prepare(j, fps);
	}

	void prepare(const json& j, float fps)
	{
		auto& pa = j["p"]["a"];
		auto& sa = j["s"]["a"];
		auto& ra = j["r"]["a"];
		auto& oa = j["o"]["a"];
		auto& pk = j["p"]["k"];
		auto& sk = j["s"]["k"];
		auto& rk = j["r"]["k"];
		auto& ok = j["o"]["k"];

		if (pa.get<json::number_unsigned_t>())
		{
			for (auto it = pk.begin(); it != pk.end(); ++it)
			{
				if ((it + 2) == pk.end())
				{
					auto& s = (*it)["s"];
					auto& e = (*it)["e"];

					pos.emplace_back((*it)["t"].get<json::number_float_t>() / fps, float3(s[0].get<json::number_float_t>(), s[1].get<json::number_float_t>(), s[2].get<json::number_float_t>()));
					pos.emplace_back((*(it + 1))["t"].get<json::number_float_t>() / fps, float3(e[0].get<json::number_float_t>(), e[1].get<json::number_float_t>(), e[2].get<json::number_float_t>()));
					break;
				}
				else
				{
					auto& s = (*it)["s"];
					pos.emplace_back((*it)["t"].get<json::number_float_t>() / fps, float3(s[0].get<json::number_float_t>(), s[1].get<json::number_float_t>(), s[2].get<json::number_float_t>()));
				}
			}
		}
		else
		{
			pos.emplace_back(0.0f, float3(pk[0].get<json::number_float_t>(), pk[1].get<json::number_float_t>(), pk[2].get<json::number_float_t>()));
		}
		
		if (sa.get<json::number_unsigned_t>())
		{
			for (auto it = sk.begin(); it != sk.end(); ++it)
			{
				if ((it + 2) == sk.end())
				{
					auto& s = (*it)["s"];
					auto& e = (*it)["e"];

					scale.emplace_back((*it)["t"].get<json::number_float_t>() / fps, float3(s[0].get<json::number_float_t>() / 100.0f, s[1].get<json::number_float_t>() / 100.0f, s[2].get<json::number_float_t>() / 100.0f));
					scale.emplace_back((*(it + 1))["t"].get<json::number_float_t>() / fps, float3(e[0].get<json::number_float_t>() / 100.0f, e[1].get<json::number_float_t>() / 100.0f, e[2].get<json::number_float_t>() / 100.0f));
					break;
				}
				else
				{
					auto& s = (*it)["s"];
					scale.emplace_back((*it)["t"].get<json::number_float_t>() / fps, float3(s[0].get<json::number_float_t>(), s[1].get<json::number_float_t>(), s[2].get<json::number_float_t>()));
				}
			}
		}
		else
		{
			scale.emplace_back(0.0f, float3(sk[0].get<json::number_float_t>() / 100.0f, sk[1].get<json::number_float_t>() / 100.0f, sk[2].get<json::number_float_t>() / 100.0f));
		}

		if (ra.get<json::number_unsigned_t>())
		{
			for (auto it = rk.begin(); it != rk.end(); ++it)
			{
				if ((it + 2) == rk.end())
				{
					auto& s = (*it)["s"];
					auto& e = (*it)["e"];

					rotation.emplace_back((*it)["t"].get<json::number_float_t>() / fps, octoon::math::radians(float3(0.0f, 0.0f, s[0].get<json::number_float_t>())));
					rotation.emplace_back((*(it + 1))["t"].get<json::number_float_t>() / fps, octoon::math::radians(float3(0.0f, 0.0f, e[0].get<json::number_float_t>())));
					break;
				}
				else
				{
					auto& s = (*it)["s"];
					rotation.emplace_back((*it)["t"].get<json::number_float_t>() / fps, octoon::math::radians(float3(0.0f, 0.0f, s[0].get<json::number_float_t>())));
				}
			}
		}
		else
		{
			rotation.emplace_back(0.0f, octoon::math::radians(float3(0.0f, 0.0f, rk.get<json::number_float_t>())));
		}

		if (oa.get<json::number_unsigned_t>())
		{
			for (auto it = ok.begin(); it != ok.end(); ++it)
			{
				if ((it + 2) == ok.end())
				{
					auto& s = (*it)["s"];
					auto& e = (*it)["e"];

					opacity.emplace_back((*it)["t"].get<json::number_float_t>() / fps, s.get<json::number_unsigned_t>() / 100.0f);
					opacity.emplace_back((*(it + 1))["t"].get<json::number_float_t>() / fps, e.get<json::number_unsigned_t>() / 100.0f);
					break;
				}
				else
				{
					auto& s = (*it)["s"];
					opacity.emplace_back((*it)["t"].get<json::number_float_t>() / fps, s.get<json::number_unsigned_t>() / 100.0f);
				}
			}
		}
		else
		{
			opacity.emplace_back(0.0f, ok.get<json::number_unsigned_t>() / 100.0f);
		}
	}
};

#endif