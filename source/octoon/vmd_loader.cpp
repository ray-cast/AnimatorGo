#include <octoon/vmd_loader.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <octoon/math/vector4.h>
#include <octoon/math/quat.h>
#include <octoon/runtime/except.h>
#include <iconv.h>
#include <map>

namespace octoon
{
	typedef char          VMD_char;
	typedef std::int8_t   VMD_int8_t;
	typedef std::uint8_t  VMD_uint8_t;
	typedef std::uint16_t VMD_uint16_t;
	typedef std::uint32_t VMD_uint32_t;

	typedef float VMD_Float;

#pragma pack(push)
#pragma pack(1)

	struct VMD_Vector2
	{
		float x;
		float y;
	};

	struct VMD_Vector3
	{
		float x;
		float y;
		float z;
	};

	struct VMD_Vector4
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct VMD_Quaternion
	{
		float x;
		float y;
		float z;
		float w;
	};

	struct VMD_Header
	{
		VMD_char magic[30];
		VMD_char name[20];
	};

	struct VMDMotion
	{
		VMD_char name[15];
		VMD_uint32_t frame;
		VMD_Vector3 location;
		VMD_Quaternion rotate;
		VMD_uint8_t interpolation[64];
	};

	struct VMDMorph
	{
		VMD_char name[15];
		VMD_uint32_t frame;
		VMD_Float weight;
	};

	struct VMDCamera
	{
		VMD_uint32_t frame;
		VMD_Float length;
		VMD_Vector3 location;
		VMD_Vector3 rotation;
		VMD_uint8_t interpolation[24];
		VMD_uint32_t viewingAngle;
		VMD_uint8_t perspective;
	};

	struct VMDLight
	{
		VMD_uint32_t frame;
		VMD_Vector3 rgb;
		VMD_Vector3 location;
	};

	struct VMDSelfShadow
	{
		VMD_uint32_t frame;
		VMD_uint8_t mode; // 00-02
		VMD_Float distance; // 0.1 - (dist * 0.000001)
	};

#pragma pack(pop)

	struct VMD
	{
		VMD_Header Header;

		VMD_uint32_t NumMotion;
		VMD_uint32_t NumMorph;
		VMD_uint32_t NumCamera;
		VMD_uint32_t NumLight;
		VMD_uint32_t NumSelfShadow;

		std::vector<VMDMotion> MotionLists;
		std::vector<VMDMorph> MorphLists;
		std::vector<VMDCamera> CameraLists;
		std::vector<VMDLight> LightLists;
		std::vector<VMDSelfShadow> SelfShadowLists;
	};

	std::string sjis2utf8(const std::string& sjis)
	{
		std::size_t in_size = sjis.size();
		std::size_t out_size = sjis.size() * 2;

		auto inbuf = std::make_unique<char[]>(in_size);
		auto outbuf = std::make_unique<char[]>(out_size);
		char* in = inbuf.get();
		char* out = outbuf.get();

		std::memcpy(in, sjis.c_str(), in_size);

		iconv_t ic = nullptr;

		try
		{
			ic = iconv_open("utf-8", "SJIS");
			iconv(ic, &in, &in_size, &out, &out_size);
			iconv_close(ic);
		}
		catch (const std::exception&)
		{
			iconv_close(ic);
		}

		return std::string(outbuf.get());
	}

	VMDLoader::VMDLoader() noexcept
	{
	}

	VMDLoader::~VMDLoader() noexcept
	{
	}

	bool
	VMDLoader::doCanRead(io::istream& stream) noexcept
	{
		static_assert(sizeof(VMDMotion) == 111, "");
		static_assert(sizeof(VMDMorph) == 23, "");
		static_assert(sizeof(VMDCamera) == 61, "");
		static_assert(sizeof(VMDLight) == 28, "");
		static_assert(sizeof(VMDSelfShadow) == 9, "");

		VMD_Header hdr;

		if (stream.read((char*)&hdr, sizeof(hdr)))
		{
			if (std::strncmp(hdr.magic, "Vocaloid Motion Data 0002", 30) == 0)
				return true;
		}

		return false;
	}

	bool
	VMDLoader::doCanRead(const char* type) noexcept
	{
		return std::strncmp(type, "vmd", 3) == 0;
	}

	animation::Animation<float>
	VMDLoader::load(io::istream& stream) noexcept(false)
	{
		VMD vmd;
		if (!stream.read((char*)&vmd.Header, sizeof(vmd.Header))) {
			throw runtime::runtime_error::create(R"(Cannot read property "Header" from stream)");
		}

		if (!stream.read((char*)&vmd.NumMotion, sizeof(vmd.NumMotion))) {
			throw runtime::runtime_error::create(R"(Cannot read property "NumMotion" from stream)");
		}

		if (vmd.NumMotion > 0)
		{
			vmd.MotionLists.resize(vmd.NumMotion);

			if (!stream.read((char*)vmd.MotionLists.data(), sizeof(VMDMotion) * vmd.NumMotion)) {
				throw runtime::runtime_error::create(R"(Cannot read property "VMDMotion" from stream)");
			}
		}

		if (!stream.read((char*)&vmd.NumMorph, sizeof(vmd.NumMorph))) {
			throw runtime::runtime_error::create(R"(Cannot read property "NumMorph" from stream)");
		}

		if (vmd.NumMorph > 0)
		{
			vmd.MorphLists.resize(vmd.NumMorph);

			if (!stream.read((char*)vmd.MorphLists.data(), sizeof(VMDMorph) * vmd.NumMorph)) {
				throw runtime::runtime_error::create(R"(Cannot read property "VMDMorph" from stream)");
			}
		}

		if (!stream.read((char*)&vmd.NumCamera, sizeof(vmd.NumCamera))) {
			throw runtime::runtime_error::create(R"(Cannot read property "NumCamera" from stream)");
		}

		if (vmd.NumCamera > 0)
		{
			vmd.CameraLists.resize(vmd.NumCamera);

			if (!stream.read((char*)vmd.CameraLists.data(), sizeof(VMDCamera) * vmd.NumCamera)) {
				throw runtime::runtime_error::create(R"(Cannot read property "VMDCamera" from stream)");
			}
		}

		if (!stream.read((char*)&vmd.NumLight, sizeof(vmd.NumLight))) {
			throw runtime::runtime_error::create(R"(Cannot read property "NumLight" from stream)");
		}

		if (vmd.NumLight > 0)
		{
			vmd.LightLists.resize(vmd.NumLight);

			if (!stream.read((char*)vmd.LightLists.data(), sizeof(VMDLight) * vmd.NumLight)) {
				throw runtime::runtime_error::create(R"(Cannot read property "VMDLight" from stream)");
			}
		}

		if (!stream.read((char*)&vmd.NumSelfShadow, sizeof(vmd.NumSelfShadow))) {
			throw runtime::runtime_error::create(R"(Cannot read property "NumSelfShadow" from stream)");
		}

		if (vmd.NumSelfShadow > 0)
		{
			vmd.SelfShadowLists.resize(vmd.NumSelfShadow);

			if (!stream.read((char*)vmd.SelfShadowLists.data(), sizeof(VMDSelfShadow) * vmd.NumSelfShadow)) {
				throw runtime::runtime_error::create(R"(Cannot read property "VMDSelfShadow" from stream)");
			}
		}

		std::map<std::string, animation::AnimationClip<float>> motions;

		for (auto& it : vmd.MotionLists)
		{
			if (motions.find(it.name) == motions.end())
				motions[it.name].setName(sjis2utf8(vmd.Header.name));

			auto& clip = motions[it.name];
			clip.getCurve("Position.X").insert(animation::Keyframe<float, float>((float)it.frame, it.location.x));
			clip.getCurve("Position.Y").insert(animation::Keyframe<float, float>((float)it.frame, it.location.y));
			clip.getCurve("Position.Z").insert(animation::Keyframe<float, float>((float)it.frame, it.location.z));
			clip.getCurve("Rotation.X").insert(animation::Keyframe<float, float>((float)it.frame, it.rotate.x));
			clip.getCurve("Rotation.Y").insert(animation::Keyframe<float, float>((float)it.frame, it.rotate.y));
			clip.getCurve("Rotation.Z").insert(animation::Keyframe<float, float>((float)it.frame, it.rotate.z));
			clip.getCurve("Rotation.W").insert(animation::Keyframe<float, float>((float)it.frame, it.rotate.w));
		}

		animation::Animation animation;
		animation.setName(sjis2utf8(vmd.Header.name));
		for (auto& it : motions)
			animation.addClip(it.second);
		return animation;
	}

	void
	VMDLoader::save(io::ostream& stream, const animation::Animation<float>& animation) noexcept(false)
	{
		VMD vmd;
		std::memset(&vmd.Header, 0, sizeof(vmd.Header));
		std::memcpy(vmd.Header.magic, "Vocaloid Motion Data 0002", 15);

		stream.write((char*)&vmd.Header, sizeof(vmd.Header));
	}
}