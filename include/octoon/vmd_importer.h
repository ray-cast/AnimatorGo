#ifndef OCTOON_VMD_IMPORTER_H_
#define OCTOON_VMD_IMPORTER_H_

#include <octoon/asset_importer.h>
#include <octoon/animation/animation.h>
#include <filesystem>

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

		VMD_Vector2() = default;
		VMD_Vector2(float xx, float yy) : x(xx), y(yy) {};
	};

	struct VMD_Vector3
	{
		float x;
		float y;
		float z;

		VMD_Vector3() = default;
		VMD_Vector3(float xx, float yy, float zz) : x(xx), y(yy), z(zz) {};
		VMD_Vector3(const math::float3& v) : x(v.x), y(v.y), z(v.z) {};
	};

	struct VMD_Vector4
	{
		float x;
		float y;
		float z;
		float w;

		VMD_Vector4() = default;
		VMD_Vector4(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {};
		VMD_Vector4(const math::float4 & v) : x(v.x), y(v.y), z(v.z), w(v.w) {};
	};

	struct VMD_Quaternion
	{
		float x;
		float y;
		float z;
		float w;

		VMD_Quaternion() = default;
		VMD_Quaternion(float xx, float yy, float zz, float ww) : x(xx), y(yy), z(zz), w(ww) {};
		VMD_Quaternion(const math::Quaternion & v) : x(v.x), y(v.y), z(v.z), w(v.w) {};
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
		VMD_Vector3 translate;
		VMD_Quaternion rotate;
		VMD_int8_t interpolation_x[4];
		VMD_int8_t interpolation_y[4];
		VMD_int8_t interpolation_z[4];
		VMD_int8_t interpolation_rotation[4];
		VMD_int8_t interpolation[48];
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
		VMD_Float distance;
		VMD_Vector3 location;
		VMD_Vector3 rotation;
		VMD_int8_t interpolation_x[4];
		VMD_int8_t interpolation_y[4];
		VMD_int8_t interpolation_z[4];
		VMD_int8_t interpolation_rotation[4];
		VMD_int8_t interpolation_distance[4];
		VMD_int8_t interpolation_angleview[4];
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

	class OCTOON_EXPORT VMD final
	{
	public:
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

		void load(std::istream& stream) noexcept(false);
		void save(std::ostream& stream) noexcept(false);

		bool load(const std::filesystem::path& filepath) noexcept(false);
		bool save(const std::filesystem::path& filepath) noexcept(false);
	};

	class OCTOON_EXPORT VMDImporter final : public AssetImporter
	{
		OctoonDeclareSubClass(VMDImporter, AssetImporter)
	public:
		VMDImporter() noexcept;
		virtual ~VMDImporter() noexcept;

		virtual void onImportAsset(AssetImporterContext& context) noexcept(false) override;

		static void save(std::ostream& stream, const Animation& animation) noexcept(false);
		static void save(const std::filesystem::path& filepath, const Animation& animation) noexcept(false);

	private:
		VMDImporter(const VMDImporter&) = delete;
		VMDImporter& operator=(const VMDImporter&) = delete;
	};
}

#endif