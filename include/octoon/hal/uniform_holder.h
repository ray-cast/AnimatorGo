#ifndef OCTOON_UNIFORM_HOLDER_H_
#define OCTOON_UNIFORM_HOLDER_H_

#include <octoon/hal/graphics_texture.h>

namespace octoon
{
	class OCTOON_EXPORT UniformHolder final
	{
	public:
		UniformHolder() noexcept;
		virtual ~UniformHolder() noexcept;

		void setType(UniformAttributeFormat type) noexcept(false);
		UniformAttributeFormat getType() const noexcept;

		void uniform1b(bool value) noexcept;
		void uniform1f(float i1) noexcept;
		void uniform2f(float i1, float i2) noexcept;
		void uniform3f(float i1, float i2, float i3) noexcept;
		void uniform4f(float i1, float i2, float i3, float i4) noexcept;
		void uniform1i(std::int32_t i1) noexcept;
		void uniform2i(std::int32_t i1, std::int32_t i2) noexcept;
		void uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept;
		void uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept;
		void uniform1ui(std::uint32_t i1) noexcept;
		void uniform2ui(std::uint32_t i1, std::uint32_t i2) noexcept;
		void uniform3ui(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3) noexcept;
		void uniform4ui(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3, std::uint32_t i4) noexcept;
		void uniform2fmat(const float value[]) noexcept;
		void uniform3fmat(const float value[]) noexcept;
		void uniform4fmat(const float value[]) noexcept;
		void uniform1iv(std::size_t num, const std::int32_t* value) noexcept;
		void uniform2iv(std::size_t num, const std::int32_t* value) noexcept;
		void uniform3iv(std::size_t num, const std::int32_t* value) noexcept;
		void uniform4iv(std::size_t num, const std::int32_t* value) noexcept;
		void uniform1uiv(std::size_t num, const std::uint32_t* value) noexcept;
		void uniform2uiv(std::size_t num, const std::uint32_t* value) noexcept;
		void uniform3uiv(std::size_t num, const std::uint32_t* value) noexcept;
		void uniform4uiv(std::size_t num, const std::uint32_t* value) noexcept;
		void uniform1fv(std::size_t num, const float* value) noexcept;
		void uniform2fv(std::size_t num, const float* value) noexcept;
		void uniform3fv(std::size_t num, const float* value) noexcept;
		void uniform4fv(std::size_t num, const float* value) noexcept;
		void uniform2fmatv(std::size_t num, const float* value) noexcept;
		void uniform3fmatv(std::size_t num, const float* value) noexcept;
		void uniform4fmatv(std::size_t num, const float* value) noexcept;

		void uniform2i(const math::int2& value) noexcept;
		void uniform3i(const math::int3& value) noexcept;
		void uniform4i(const math::int4& value) noexcept;
		void uniform2ui(const math::uint2& value) noexcept;
		void uniform3ui(const math::uint3& value) noexcept;
		void uniform4ui(const math::uint4& value) noexcept;
		void uniform2f(const math::float2& value) noexcept;
		void uniform3f(const math::float3& value) noexcept;
		void uniform4f(const math::float4& value) noexcept;
		void uniform2fmat(const math::float2x2& value) noexcept;
		void uniform3fmat(const math::float3x3& value) noexcept;
		void uniform4fmat(const math::float4x4& value) noexcept;
		void uniform1iv(const std::vector<math::int1>& value) noexcept;
		void uniform2iv(const std::vector<math::int2>& value) noexcept;
		void uniform3iv(const std::vector<math::int3>& value) noexcept;
		void uniform4iv(const std::vector<math::int4>& value) noexcept;
		void uniform1uiv(const std::vector<math::uint1>& value) noexcept;
		void uniform2uiv(const std::vector<math::uint2>& value) noexcept;
		void uniform3uiv(const std::vector<math::uint3>& value) noexcept;
		void uniform4uiv(const std::vector<math::uint4>& value) noexcept;
		void uniform1fv(const std::vector<math::float1>& value) noexcept;
		void uniform2fv(const std::vector<math::float2>& value) noexcept;
		void uniform3fv(const std::vector<math::float3>& value) noexcept;
		void uniform4fv(const std::vector<math::float4>& value) noexcept;
		void uniform2fmatv(const std::vector<math::float2x2>& value) noexcept;
		void uniform3fmatv(const std::vector<math::float3x3>& value) noexcept;
		void uniform4fmatv(const std::vector<math::float4x4>& value) noexcept;
		void uniformTexture(std::shared_ptr<GraphicsTexture> texture, GraphicsSamplerPtr sampler = nullptr) noexcept;
		void uniformBuffer(GraphicsDataPtr ubo) noexcept;

		bool getBool() const noexcept;
		int getInt() const noexcept;
		const math::int2& getInt2() const noexcept;
		const math::int3& getInt3() const noexcept;
		const math::int4& getInt4() const noexcept;
		math::uint1 getUInt() const noexcept;
		const math::uint2& getUInt2() const noexcept;
		const math::uint3& getUInt3() const noexcept;
		const math::uint4& getUInt4() const noexcept;
		float getFloat() const noexcept;
		const math::float2& getFloat2() const noexcept;
		const math::float3& getFloat3() const noexcept;
		const math::float4& getFloat4() const noexcept;
		const math::float2x2& getFloat2x2() const noexcept;
		const math::float3x3& getFloat3x3() const noexcept;
		const math::float4x4& getFloat4x4() const noexcept;
		const std::vector<math::int1>& getIntArray() const noexcept;
		const std::vector<math::int2>& getInt2Array() const noexcept;
		const std::vector<math::int3>& getInt3Array() const noexcept;
		const std::vector<math::int4>& getInt4Array() const noexcept;
		const std::vector<math::uint1>& getUIntArray() const noexcept;
		const std::vector<math::uint2>& getUInt2Array() const noexcept;
		const std::vector<math::uint3>& getUInt3Array() const noexcept;
		const std::vector<math::uint4>& getUInt4Array() const noexcept;
		const std::vector<math::float1>& getFloatArray() const noexcept;
		const std::vector<math::float2>& getFloat2Array() const noexcept;
		const std::vector<math::float3>& getFloat3Array() const noexcept;
		const std::vector<math::float4>& getFloat4Array() const noexcept;
		const std::vector<math::float2x2>& getFloat2x2Array() const noexcept;
		const std::vector<math::float3x3>& getFloat3x3Array() const noexcept;
		const std::vector<math::float4x4>& getFloat4x4Array() const noexcept;
		const std::shared_ptr<GraphicsTexture>& getTexture() const noexcept;
		const GraphicsSamplerPtr& getTextureSampler() const noexcept;
		const GraphicsDataPtr& getBuffer() const noexcept;

	private:
		UniformHolder(const UniformHolder&) noexcept = delete;
		UniformHolder& operator=(const UniformHolder&) noexcept = delete;

	private:
		struct TexturePack
		{
			std::shared_ptr<GraphicsTexture> image;
			GraphicsSamplerPtr sampler;
		};

		union
		{
			bool boolValue_;
			int intValue_[4];
			math::uint1 uintValue_[4];
			float floatValue_[4];
			math::float2x2* m2Value_;
			math::float3x3* m3Value_;
			math::float4x4* m4Value_;
			std::vector<math::int1>* iarray_;
			std::vector<math::int2>* iarray2_;
			std::vector<math::int3>* iarray3_;
			std::vector<math::int4>* iarray4_;
			std::vector<math::uint1>* uiarray_;
			std::vector<math::uint2>* uiarray2_;
			std::vector<math::uint3>* uiarray3_;
			std::vector<math::uint4>* uiarray4_;
			std::vector<math::float1>* farray_;
			std::vector<math::float2>* farray2_;
			std::vector<math::float3>* farray3_;
			std::vector<math::float4>* farray4_;
			std::vector<math::float2x2>* m2array_;
			std::vector<math::float3x3>* m3array_;
			std::vector<math::float4x4>* m4array_;
			TexturePack* texture_;
			GraphicsDataPtr* ubo_;
		};

		UniformAttributeFormat type_;
	};
}

#endif