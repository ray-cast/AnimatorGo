#ifndef OCTOON_VMD_H_
#define OCTOON_VMD_H_

#include <octoon/io/iostream.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector3.h>
#include <octoon/math/vector4.h>
#include <octoon/math/quat.h>
#include <octoon/animation/animator.h>
#include <optional>

namespace octoon
{
	namespace animation
	{
		typedef math::Vector2 VMD_Vector2;
		typedef math::Vector3 VMD_Vector3;
		typedef math::Vector4 VMD_Vector4;
		typedef math::Quaternion VMD_Quaternion;

		typedef math::Vector3 VMD_Color3;
		typedef math::Vector4 VMD_Color4;

		typedef char          VMD_char;
		typedef std::int8_t   VMD_int8_t;
		typedef std::uint8_t  VMD_uint8_t;
		typedef std::uint16_t VMD_uint16_t;
		typedef std::uint32_t VMD_uint32_t;

		typedef float VMD_Float;

#pragma pack(push)
#pragma pack(1)

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

		class VmdName
		{
		public:
			static std::string sjis2utf8(const std::string& sjis);
			static std::optional<std::string> load(io::istream& reader);
			static std::optional<std::string> load_fixed_utf8(io::istream& reader, std::size_t len);
			static std::optional<std::vector<std::string>> load_arrays(io::istream& reader);
		};

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

		class VMDHandler final
		{
		public:
			VMDHandler() noexcept;
			~VMDHandler() noexcept;

			bool doCanRead(io::istream& stream) const noexcept;
			bool doCanRead(const char* type) const noexcept;

			bool doLoad(io::istream& stream, Animator<float>& animation) noexcept;
			bool doSave(io::ostream& stream, const Animator<float>& animation) noexcept;

		private:
			VMDHandler(const VMDHandler&) = delete;
			VMDHandler& operator=(const VMDHandler&) = delete;
		};
	}
}

#endif