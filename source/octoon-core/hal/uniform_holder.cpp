#include <octoon/hal/uniform_holder.h>
#include <octoon/runtime/except.h>
#include <cstring>

namespace octoon
{
	UniformHolder::UniformHolder() noexcept
		: type_(UniformAttributeFormat::Null)
		, texture_(nullptr)
	{
	}

	UniformHolder::~UniformHolder() noexcept
	{
		this->setType(UniformAttributeFormat::Null);
	}

	void
	UniformHolder::setType(UniformAttributeFormat type) noexcept(false)
	{
		if (type_ != type)
		{
			switch (type_)
			{
				case UniformAttributeFormat::IntArray: { delete iarray_; iarray_ = nullptr; } break;
				case UniformAttributeFormat::Int2Array: { delete iarray2_; iarray2_ = nullptr; } break;
				case UniformAttributeFormat::Int3Array: { delete iarray3_; iarray3_ = nullptr; } break;
				case UniformAttributeFormat::Int4Array: { delete iarray4_; iarray4_ = nullptr; } break;
				case UniformAttributeFormat::UIntArray: { delete uiarray_; uiarray_ = nullptr; } break;
				case UniformAttributeFormat::UInt2Array: { delete uiarray2_; uiarray2_ = nullptr; } break;
				case UniformAttributeFormat::UInt3Array: { delete uiarray3_; uiarray3_ = nullptr; } break;
				case UniformAttributeFormat::UInt4Array: { delete uiarray4_; uiarray4_ = nullptr; } break;
				case UniformAttributeFormat::FloatArray: { delete farray_; farray_ = nullptr; } break;
				case UniformAttributeFormat::Float2Array: { delete farray2_; farray2_ = nullptr; } break;
				case UniformAttributeFormat::Float3Array: { delete farray3_; farray3_ = nullptr; } break;
				case UniformAttributeFormat::Float4Array: { delete farray4_; farray4_ = nullptr; } break;
				case UniformAttributeFormat::Float2x2: { delete m2Value_; m2Value_ = nullptr; } break;
				case UniformAttributeFormat::Float3x3: { delete m3Value_; m3Value_ = nullptr; } break;
				case UniformAttributeFormat::Float4x4: { delete m4Value_; m4Value_ = nullptr; } break;
				case UniformAttributeFormat::Float2x2Array: { delete m2array_; m2array_ = nullptr; } break;
				case UniformAttributeFormat::Float3x3Array: { delete m3array_; m3array_ = nullptr; } break;
				case UniformAttributeFormat::Float4x4Array: { delete m4array_; m4array_ = nullptr; } break;
				case UniformAttributeFormat::UniformBuffer: { delete ubo_; ubo_ = nullptr; } break;
				case UniformAttributeFormat::SamplerImage:
				case UniformAttributeFormat::StorageImage:
				case UniformAttributeFormat::CombinedImageSampler:
				{
					delete texture_;
					texture_ = nullptr;
				}
				break;
			}

			switch (type)
			{
				case UniformAttributeFormat::Null: { boolValue_ = false; } break;
				case UniformAttributeFormat::Boolean: { boolValue_ = false; } break;
				case UniformAttributeFormat::Float: { floatValue_[0] = 0; } break;
				case UniformAttributeFormat::Float2: { floatValue_[0] = 0; floatValue_[1] = 0; } break;
				case UniformAttributeFormat::Float3: { floatValue_[0] = 0; floatValue_[1] = 0; floatValue_[2] = 0; } break;
				case UniformAttributeFormat::Float4: { floatValue_[0] = 0; floatValue_[1] = 0; floatValue_[2] = 0; floatValue_[4] = 0; } break;
				case UniformAttributeFormat::Int: { intValue_[0] = 0; } break;
				case UniformAttributeFormat::Int2: { intValue_[0] = 0; intValue_[1] = 0; } break;
				case UniformAttributeFormat::Int3: { intValue_[0] = 0; intValue_[1] = 0; intValue_[2] = 0; } break;
				case UniformAttributeFormat::Int4: { intValue_[0] = 0; intValue_[1] = 0; intValue_[2] = 0; intValue_[4] = 0; } break;
				case UniformAttributeFormat::UInt: { uintValue_[0] = 0; } break;
				case UniformAttributeFormat::UInt2: { uintValue_[0] = 0; uintValue_[1] = 0; } break;
				case UniformAttributeFormat::UInt3: { uintValue_[0] = 0; uintValue_[1] = 0; uintValue_[2] = 0; } break;
				case UniformAttributeFormat::UInt4: { uintValue_[0] = 0; uintValue_[1] = 0; uintValue_[2] = 0; uintValue_[4] = 0; } break;
				case UniformAttributeFormat::IntArray: iarray_ = new std::vector<math::int1>(); break;
				case UniformAttributeFormat::Int2Array: iarray2_ = new std::vector<math::int2>(); break;
				case UniformAttributeFormat::Int3Array: iarray3_ = new std::vector<math::int3>(); break;
				case UniformAttributeFormat::Int4Array: iarray4_ = new std::vector<math::int4>(); break;
				case UniformAttributeFormat::UIntArray: uiarray_ = new std::vector<math::uint1>(); break;
				case UniformAttributeFormat::UInt2Array: uiarray2_ = new std::vector<math::uint2>(); break;
				case UniformAttributeFormat::UInt3Array: uiarray3_ = new std::vector<math::uint3>(); break;
				case UniformAttributeFormat::UInt4Array: uiarray4_ = new std::vector<math::uint4>(); break;
				case UniformAttributeFormat::FloatArray: farray_ = new std::vector<math::float1>(); break;
				case UniformAttributeFormat::Float2Array: farray2_ = new std::vector<math::float2>(); break;
				case UniformAttributeFormat::Float3Array: farray3_ = new std::vector<math::float3>(); break;
				case UniformAttributeFormat::Float4Array: farray4_ = new std::vector<math::float4>(); break;
				case UniformAttributeFormat::Float2x2: m2Value_ = new math::float2x2; break;
				case UniformAttributeFormat::Float3x3: m3Value_ = new math::float3x3; break;
				case UniformAttributeFormat::Float4x4: m4Value_ = new math::float4x4; break;
				case UniformAttributeFormat::Float2x2Array: m2array_ = new std::vector<math::float2x2>; break;
				case UniformAttributeFormat::Float3x3Array: m3array_ = new std::vector<math::float3x3>; break;
				case UniformAttributeFormat::Float4x4Array: m4array_ = new std::vector<math::float4x4>; break;
				case UniformAttributeFormat::UniformBuffer: ubo_ = new GraphicsDataPtr; break;
				case UniformAttributeFormat::SamplerImage:
				case UniformAttributeFormat::StorageImage:
				case UniformAttributeFormat::CombinedImageSampler:
				{
					texture_ = new TexturePack;
					texture_->image = nullptr;
					texture_->sampler = nullptr;
				}
				break;
				default:
				{
					throw runtime_error::create("Unkonwn Type");
				}
			}

			type_ = type;
		}
	}

	UniformAttributeFormat
	UniformHolder::getType() const noexcept
	{
		return type_;
	}

	void
	UniformHolder::uniform1b(bool b1) noexcept
	{
		assert(type_ == UniformAttributeFormat::Boolean);
		boolValue_ = b1;
	}

	void
	UniformHolder::uniform1i(std::int32_t i1) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int);
		intValue_[0] = i1;
	}

	void
	UniformHolder::uniform2i(const math::int2& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int2);
		intValue_[0] = value.x;
		intValue_[1] = value.y;
	}

	void
	UniformHolder::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int2);
		intValue_[0] = i1;
		intValue_[1] = i2;
	}

	void
	UniformHolder::uniform3i(const math::int3& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int3);
		intValue_[0] = value.x;
		intValue_[1] = value.y;
		intValue_[2] = value.z;
	}

	void
	UniformHolder::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int3);
		intValue_[0] = i1;
		intValue_[1] = i2;
		intValue_[2] = i3;
	}

	void
	UniformHolder::uniform4i(const math::int4& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int4);
		intValue_[0] = value.x;
		intValue_[1] = value.y;
		intValue_[2] = value.z;
		intValue_[3] = value.w;
	}

	void
	UniformHolder::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int4);
		intValue_[0] = i1;
		intValue_[1] = i2;
		intValue_[2] = i3;
		intValue_[3] = i4;
	}

	void
	UniformHolder::uniform1ui(std::uint32_t ui1) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float);
		uintValue_[0] = ui1;
	}

	void
	UniformHolder::uniform2ui(const math::uint2& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt2);
		uintValue_[0] = value.x;
		uintValue_[1] = value.y;
	}

	void
	UniformHolder::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2);
		uintValue_[0] = ui1;
		uintValue_[1] = ui2;
	}

	void
	UniformHolder::uniform3ui(const math::uint3& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt3);
		uintValue_[0] = value.x;
		uintValue_[1] = value.y;
		uintValue_[2] = value.z;
	}

	void
	UniformHolder::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3);
		uintValue_[0] = ui1;
		uintValue_[1] = ui2;
		uintValue_[2] = ui3;
	}

	void
	UniformHolder::uniform4ui(const math::uint4& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt4);
		uintValue_[0] = value.x;
		uintValue_[1] = value.y;
		uintValue_[2] = value.z;
		uintValue_[3] = value.w;
	}

	void
	UniformHolder::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt4);
		uintValue_[0] = ui1;
		uintValue_[1] = ui2;
		uintValue_[2] = ui3;
		uintValue_[3] = ui4;
	}

	void
	UniformHolder::uniform1f(float f1) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float);
		floatValue_[0] = f1;
	}

	void
	UniformHolder::uniform2f(const math::float2& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2);
		floatValue_[0] = value.x;
		floatValue_[1] = value.y;
	}

	void
	UniformHolder::uniform2f(float f1, float f2) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2);
		floatValue_[0] = f1;
		floatValue_[1] = f2;
	}

	void
	UniformHolder::uniform3f(const math::float3& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3);
		floatValue_[0] = value.x;
		floatValue_[1] = value.y;
		floatValue_[2] = value.z;
	}

	void
	UniformHolder::uniform3f(float f1, float f2, float f3) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3);
		floatValue_[0] = f1;
		floatValue_[1] = f2;
		floatValue_[2] = f3;
	}

	void
	UniformHolder::uniform4f(const math::float4& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4);
		floatValue_[0] = value.x;
		floatValue_[1] = value.y;
		floatValue_[2] = value.z;
		floatValue_[3] = value.w;
	}

	void
	UniformHolder::uniform4f(float f1, float f2, float f3, float f4) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4);
		floatValue_[0] = f1;
		floatValue_[1] = f2;
		floatValue_[2] = f3;
		floatValue_[3] = f4;
	}

	void
	UniformHolder::uniform2fmat(const math::float2x2& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2x2);
		*m2Value_ = value;
	}

	void
	UniformHolder::uniform2fmat(const float mat2[]) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2x2);
		std::memcpy(m2Value_, mat2, sizeof(math::float2x2));
	}

	void
	UniformHolder::uniform3fmat(const math::float3x3& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3x3);
		*m3Value_ = value;
	}

	void
	UniformHolder::uniform3fmat(const float mat3[]) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3x3);
		std::memcpy(m3Value_, mat3, sizeof(math::float3x3));
	}

	void
	UniformHolder::uniform4fmat(const math::float4x4& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4x4);
		*m4Value_ = value;
	}
	void
	UniformHolder::uniform4fmat(const float mat4[]) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4x4);
		std::memcpy(m4Value_, mat4, sizeof(math::float4x4));
	}

	void
	UniformHolder::uniform1iv(const std::vector<math::int1>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::IntArray);
		*iarray_ = value;
	}

	void
	UniformHolder::uniform1iv(std::size_t num, const std::int32_t* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::IntArray);
		iarray_->resize(num);
		std::memcpy(iarray_->data(), str, sizeof(math::int1) * num);
	}

	void
	UniformHolder::uniform2iv(const std::vector<math::int2>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int2Array);
		*iarray2_ = value;
	}

	void
	UniformHolder::uniform2iv(std::size_t num, const std::int32_t* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int2Array);
		iarray2_->resize(num);
		std::memcpy(iarray2_->data(), str, sizeof(math::int2) * num);
	}

	void
	UniformHolder::uniform3iv(const std::vector<math::int3>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int3Array);
		*iarray3_ = value;
	}

	void
	UniformHolder::uniform3iv(std::size_t num, const std::int32_t* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int2Array);
		iarray3_->resize(num);
		std::memcpy(iarray3_->data(), str, sizeof(math::int3) * num);
	}

	void
	UniformHolder::uniform4iv(const std::vector<math::int4>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int4Array);
		*iarray4_ = value;
	}

	void
	UniformHolder::uniform4iv(std::size_t num, const std::int32_t* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::Int2Array);
		iarray4_->resize(num);
		std::memcpy(iarray4_->data(), str, sizeof(math::int4) * num);
	}

	void
	UniformHolder::uniform1uiv(const std::vector<math::uint1>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::UIntArray);
		*uiarray_ = value;
	}

	void
	UniformHolder::uniform1uiv(std::size_t num, const std::uint32_t* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::UIntArray);
		uiarray_->resize(num);
		std::memcpy(uiarray_->data(), str, sizeof(math::uint1) * num);
	}

	void
	UniformHolder::uniform2uiv(const std::vector<math::uint2>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt2Array);
		*uiarray2_ = value;
	}

	void
	UniformHolder::uniform2uiv(std::size_t num, const std::uint32_t* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt2Array);
		uiarray2_->resize(num);
		std::memcpy(uiarray2_->data(), str, sizeof(math::uint2) * num);
	}

	void
	UniformHolder::uniform3uiv(const std::vector<math::uint3>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt3Array);
		*uiarray3_ = value;
	}

	void
	UniformHolder::uniform3uiv(std::size_t num, const std::uint32_t* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt3Array);
		uiarray3_->resize(num);
		std::memcpy(uiarray3_->data(), str, sizeof(math::uint3) * num);
	}

	void
	UniformHolder::uniform4uiv(const std::vector<math::uint4>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt4Array);
		*uiarray4_ = value;
	}

	void
	UniformHolder::uniform4uiv(std::size_t num, const std::uint32_t* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt4Array);
		uiarray4_->resize(num);
		std::memcpy(uiarray4_->data(), str, sizeof(math::uint4) * num);
	}

	void
	UniformHolder::uniform1fv(const std::vector<math::float1>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::FloatArray);
		*farray_ = value;
	}

	void
	UniformHolder::uniform1fv(std::size_t num, const float* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::FloatArray);
		farray_->resize(num);
		std::memcpy(farray_->data(), str, sizeof(math::float1) * num);
	}

	void
	UniformHolder::uniform2fv(const std::vector<math::float2>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2Array);
		*farray2_ = value;
	}

	void
	UniformHolder::uniform2fv(std::size_t num, const float* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2Array);
		farray2_->resize(num);
		std::memcpy(farray2_->data(), str, sizeof(math::float2) * num);
	}

	void
	UniformHolder::uniform3fv(const std::vector<math::float3>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3Array);
		*farray3_ = value;
	}

	void
	UniformHolder::uniform3fv(std::size_t num, const float* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3Array);
		farray3_->resize(num);
		std::memcpy(farray3_->data(), str, sizeof(math::float3) * num);
	}

	void
	UniformHolder::uniform4fv(const std::vector<math::float4>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4Array);
		*farray4_ = value;
	}

	void
	UniformHolder::uniform4fv(std::size_t num, const float* str) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4Array);
		farray4_->resize(num);
		std::memcpy(farray4_->data(), str, sizeof(math::float4) * num);
	}

	void
	UniformHolder::uniform2fmatv(const std::vector<math::float2x2>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2x2Array);
		*m2array_ = value;
	}

	void
	UniformHolder::uniform2fmatv(std::size_t num, const float* mat2) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4Array);
		m2array_->resize(num);
		std::memcpy(m2array_->data(), mat2, sizeof(math::float2x2) * num);
	}

	void
	UniformHolder::uniform3fmatv(const std::vector<math::float3x3>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3x3Array);
		*m3array_ = value;
	}

	void
	UniformHolder::uniform3fmatv(std::size_t num, const float* mat3) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4Array);
		m3array_->resize(num);
		std::memcpy(m3array_->data(), mat3, sizeof(math::float3x3) * num);
	}

	void
	UniformHolder::uniform4fmatv(const std::vector<math::float4x4>& value) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4x4Array);
		*m4array_ = value;
	}

	void
	UniformHolder::uniform4fmatv(std::size_t num, const float* mat4) noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4Array);
		m4array_->resize(num);
		std::memcpy(m4array_->data(), mat4, sizeof(math::float4x4) * num);
	}

	void
	UniformHolder::uniformTexture(std::shared_ptr<GraphicsTexture> texture, GraphicsSamplerPtr sampler) noexcept
	{
		assert(type_ == UniformAttributeFormat::StorageImage || type_ == UniformAttributeFormat::CombinedImageSampler || type_ == UniformAttributeFormat::SamplerImage);
		texture_->image = texture;
		texture_->sampler = sampler;
	}

	void
	UniformHolder::uniformBuffer(GraphicsDataPtr ubo) noexcept
	{
		assert(type_ == UniformAttributeFormat::UniformBuffer);
		*ubo_ = ubo;
	}

	bool
	UniformHolder::getBool() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Boolean);
		return boolValue_;
	}

	int
	UniformHolder::getInt() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Int);
		return intValue_[0];
	}

	const math::int2&
	UniformHolder::getInt2() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Int2);
		return (math::int2&)intValue_;
	}

	const math::int3&
	UniformHolder::getInt3() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Int3);
		return (math::int3&)intValue_;
	}

	const math::int4&
	UniformHolder::getInt4() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Int4);
		return (math::int4&)intValue_;
	}

	math::uint1
	UniformHolder::getUInt() const noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt);
		return uintValue_[0];
	}

	const math::uint2&
	UniformHolder::getUInt2() const noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt2);
		return (math::uint2&)uintValue_;
	}

	const math::uint3&
	UniformHolder::getUInt3() const noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt3);
		return (math::uint3&)uintValue_;
	}

	const math::uint4&
	UniformHolder::getUInt4() const noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt4);
		return (math::uint4&)uintValue_;
	}

	float
	UniformHolder::getFloat() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float);
		return floatValue_[0];
	}

	const math::float2&
	UniformHolder::getFloat2() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2);
		return (math::float2&)floatValue_;
	}

	const math::float3&
	UniformHolder::getFloat3() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3);
		return (math::float3&)floatValue_;
	}

	const math::float4&
	UniformHolder::getFloat4() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4);
		return (math::float4&)floatValue_;
	}

	const math::float2x2&
	UniformHolder::getFloat2x2() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2x2);
		return (math::float2x2&)*m2Value_;
	}

	const math::float3x3&
	UniformHolder::getFloat3x3() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3x3);
		return (math::float3x3&)*m3Value_;
	}

	const math::float4x4&
	UniformHolder::getFloat4x4() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4x4);
		return (math::float4x4&)*m4Value_;
	}

	const std::vector<math::int1>&
	UniformHolder::getIntArray() const noexcept
	{
		assert(type_ == UniformAttributeFormat::IntArray);
		return *iarray_;
	}

	const std::vector<math::int2>&
	UniformHolder::getInt2Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Int2Array);
		return *iarray2_;
	}

	const std::vector<math::int3>&
	UniformHolder::getInt3Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Int3Array);
		return *iarray3_;
	}

	const std::vector<math::int4>&
	UniformHolder::getInt4Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Int4Array);
		return *iarray4_;
	}

	const std::vector<math::uint1>&
	UniformHolder::getUIntArray() const noexcept
	{
		assert(type_ == UniformAttributeFormat::UIntArray);
		return *uiarray_;
	}

	const std::vector<math::uint2>&
	UniformHolder::getUInt2Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt2Array);
		return *uiarray2_;
	}

	const std::vector<math::uint3>&
	UniformHolder::getUInt3Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt3Array);
		return *uiarray3_;
	}

	const std::vector<math::uint4>&
	UniformHolder::getUInt4Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::UInt4Array);
		return *uiarray4_;
	}

	const std::vector<float>&
	UniformHolder::getFloatArray() const noexcept
	{
		assert(type_ == UniformAttributeFormat::FloatArray);
		return *farray_;
	}

	const std::vector<math::float2>&
	UniformHolder::getFloat2Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2Array);
		return *farray2_;
	}

	const std::vector<math::float3>&
	UniformHolder::getFloat3Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3Array);
		return *farray3_;
	}

	const std::vector<math::float4>&
	UniformHolder::getFloat4Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4Array);
		return *farray4_;
	}

	const std::vector<math::float2x2>&
	UniformHolder::getFloat2x2Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float2x2Array);
		return *m2array_;
	}

	const std::vector<math::float3x3>&
	UniformHolder::getFloat3x3Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float3x3Array);
		return *m3array_;
	}

	const std::vector<math::float4x4>&
	UniformHolder::getFloat4x4Array() const noexcept
	{
		assert(type_ == UniformAttributeFormat::Float4x4Array);
		return *m4array_;
	}

	const std::shared_ptr<GraphicsTexture>&
	UniformHolder::getTexture() const noexcept
	{
		assert(type_ == UniformAttributeFormat::SamplerImage || type_ == UniformAttributeFormat::StorageImage);
		return texture_->image;
	}

	const GraphicsSamplerPtr&
	UniformHolder::getTextureSampler() const noexcept
	{
		assert(type_ == UniformAttributeFormat::SamplerImage || type_ == UniformAttributeFormat::StorageImage);
		return texture_->sampler;
	}

	const GraphicsDataPtr&
	UniformHolder::getBuffer() const noexcept
	{
		assert(type_ == UniformAttributeFormat::UniformBuffer);
		return *ubo_;
	}
}