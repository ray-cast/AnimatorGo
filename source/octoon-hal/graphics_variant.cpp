#include <octoon/hal/graphics_variant.h>
#include <cstring>

namespace octoon
{
	namespace graphics
	{
		GraphicsVariant::GraphicsVariant() noexcept
			: _type(GraphicsUniformType::Null)
		{
			std::memset(&_value, 0, sizeof(_value));
		}

		GraphicsVariant::~GraphicsVariant() noexcept
		{
			this->setType(GraphicsUniformType::Null);
		}

		void
		GraphicsVariant::setType(GraphicsUniformType type) noexcept
		{
			if (_type != type)
			{
				if (_type == GraphicsUniformType::IntArray)
				{
					delete _value.iarray;
					_value.iarray = nullptr;
				}
				else if (_type == GraphicsUniformType::Int2Array)
				{
					delete _value.iarray2;
					_value.iarray2 = nullptr;
				}
				else if (_type == GraphicsUniformType::Int3Array)
				{
					delete _value.iarray3;
					_value.iarray3 = nullptr;
				}
				else if (_type == GraphicsUniformType::Int4Array)
				{
					delete _value.iarray4;
					_value.iarray4 = nullptr;
				}
				else if (_type == GraphicsUniformType::UIntArray)
				{
					delete _value.uiarray;
					_value.uiarray = nullptr;
				}
				else if (_type == GraphicsUniformType::UInt2Array)
				{
					delete _value.uiarray2;
					_value.uiarray2 = nullptr;
				}
				else if (_type == GraphicsUniformType::UInt3Array)
				{
					delete _value.uiarray3;
					_value.uiarray3 = nullptr;
				}
				else if (_type == GraphicsUniformType::UInt4Array)
				{
					delete _value.uiarray4;
					_value.uiarray4 = nullptr;
				}
				else if (_type == GraphicsUniformType::FloatArray)
				{
					delete _value.farray;
					_value.farray = nullptr;
				}
				else if (_type == GraphicsUniformType::Float2Array)
				{
					delete _value.farray2;
					_value.farray2 = nullptr;
				}
				else if (_type == GraphicsUniformType::Float3Array)
				{
					delete _value.farray3;
					_value.farray3 = nullptr;
				}
				else if (_type == GraphicsUniformType::Float4Array)
				{
					delete _value.farray4;
					_value.farray4 = nullptr;
				}
				else if (_type == GraphicsUniformType::Float2x2)
				{
					delete _value.m2;
					_value.m2 = nullptr;
				}
				else if (_type == GraphicsUniformType::Float3x3)
				{
					delete _value.m3;
					_value.m3 = nullptr;
				}
				else if (_type == GraphicsUniformType::Float4x4)
				{
					delete _value.m4;
					_value.m4 = nullptr;
				}
				else if (_type == GraphicsUniformType::Float2x2Array)
				{
					delete _value.m2array;
					_value.m2array = nullptr;
				}
				else if (_type == GraphicsUniformType::Float3x3Array)
				{
					delete _value.m3array;
					_value.m3array = nullptr;
				}
				else if (_type == GraphicsUniformType::Float4x4Array)
				{
					delete _value.m4array;
					_value.m4array = nullptr;
				}
				else if (_type == GraphicsUniformType::UniformBuffer)
				{
					delete _value.ubo;
					_value.ubo = nullptr;
				}
				else if (_type == GraphicsUniformType::SamplerImage ||
					_type == GraphicsUniformType::StorageImage ||
					_type == GraphicsUniformType::CombinedImageSampler)
				{
					if (type != GraphicsUniformType::SamplerImage ||
						type != GraphicsUniformType::StorageImage ||
						type != GraphicsUniformType::CombinedImageSampler)
					{
						delete _value.texture;
						_value.texture = nullptr;
					}
				}

				if (type == GraphicsUniformType::IntArray)
					_value.iarray = new std::vector<int1>();
				else if (type == GraphicsUniformType::Int2Array)
					_value.iarray2 = new std::vector<int2>();
				else if (type == GraphicsUniformType::Int3Array)
					_value.iarray3 = new std::vector<int3>();
				else if (type == GraphicsUniformType::Int4Array)
					_value.iarray4 = new std::vector<int4>();
				else if (type == GraphicsUniformType::UIntArray)
					_value.uiarray = new std::vector<uint1>();
				else if (type == GraphicsUniformType::UInt2Array)
					_value.uiarray2 = new std::vector<uint2>();
				else if (type == GraphicsUniformType::UInt3Array)
					_value.uiarray3 = new std::vector<uint3>();
				else if (type == GraphicsUniformType::UInt4Array)
					_value.uiarray4 = new std::vector<uint4>();
				else if (type == GraphicsUniformType::FloatArray)
					_value.farray = new std::vector<float1>();
				else if (type == GraphicsUniformType::Float2Array)
					_value.farray2 = new std::vector<float2>();
				else if (type == GraphicsUniformType::Float3Array)
					_value.farray3 = new std::vector<float3>();
				else if (type == GraphicsUniformType::Float4Array)
					_value.farray4 = new std::vector<float4>();
				else if (type == GraphicsUniformType::Float2x2)
					_value.m2 = new float2x2;
				else if (type == GraphicsUniformType::Float3x3)
					_value.m3 = new float3x3;
				else if (type == GraphicsUniformType::Float4x4)
					_value.m4 = new float4x4;
				else if (type == GraphicsUniformType::Float3x3Array)
					_value.m3array = new std::vector<float3x3>;
				else if (type == GraphicsUniformType::Float4x4Array)
					_value.m4array = new std::vector<float4x4>;
				else if (type == GraphicsUniformType::UniformBuffer)
					_value.ubo = new GraphicsDataPtr;
				else if (type == GraphicsUniformType::SamplerImage ||
					type == GraphicsUniformType::StorageImage ||
					type == GraphicsUniformType::CombinedImageSampler)
				{
					if (_type != GraphicsUniformType::SamplerImage ||
						_type != GraphicsUniformType::StorageImage ||
						_type != GraphicsUniformType::CombinedImageSampler)
					{
						_value.texture = new TexturePack;
					}
					else
					{
						_value.texture->image = nullptr;
						_value.texture->sampler = nullptr;
					}
				}
				else
				{
					std::memset(&_value, 0, sizeof(_value));
				}

				_type = type;
			}
		}

		GraphicsUniformType
		GraphicsVariant::getType() const noexcept
		{
			return _type;
		}

		void
		GraphicsVariant::uniform1b(bool b1) noexcept
		{
			assert(_type == GraphicsUniformType::Boolean);
			_value.b = b1;
		}

		void
		GraphicsVariant::uniform1i(std::int32_t i1) noexcept
		{
			assert(_type == GraphicsUniformType::Int);
			_value.i[0] = i1;
		}

		void
		GraphicsVariant::uniform2i(const int2& value) noexcept
		{
			assert(_type == GraphicsUniformType::Int2);
			_value.i[0] = value.x;
			_value.i[1] = value.y;
		}

		void
		GraphicsVariant::uniform2i(std::int32_t i1, std::int32_t i2) noexcept
		{
			assert(_type == GraphicsUniformType::Int2);
			_value.i[0] = i1;
			_value.i[1] = i2;
		}

		void
		GraphicsVariant::uniform3i(const int3& value) noexcept
		{
			assert(_type == GraphicsUniformType::Int3);
			_value.i[0] = value.x;
			_value.i[1] = value.y;
			_value.i[2] = value.z;
		}

		void
		GraphicsVariant::uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept
		{
			assert(_type == GraphicsUniformType::Int3);
			_value.i[0] = i1;
			_value.i[1] = i2;
			_value.i[2] = i3;
		}

		void
		GraphicsVariant::uniform4i(const int4& value) noexcept
		{
			assert(_type == GraphicsUniformType::Int4);
			_value.i[0] = value.x;
			_value.i[1] = value.y;
			_value.i[2] = value.z;
			_value.i[3] = value.w;
		}

		void
		GraphicsVariant::uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept
		{
			assert(_type == GraphicsUniformType::Int4);
			_value.i[0] = i1;
			_value.i[1] = i2;
			_value.i[2] = i3;
			_value.i[3] = i4;
		}

		void
		GraphicsVariant::uniform1ui(std::uint32_t ui1) noexcept
		{
			assert(_type == GraphicsUniformType::Float);
			_value.ui[0] = ui1;
		}

		void
		GraphicsVariant::uniform2ui(const uint2& value) noexcept
		{
			assert(_type == GraphicsUniformType::UInt2);
			_value.ui[0] = value.x;
			_value.ui[1] = value.y;
		}

		void
		GraphicsVariant::uniform2ui(std::uint32_t ui1, std::uint32_t ui2) noexcept
		{
			assert(_type == GraphicsUniformType::Float2);
			_value.ui[0] = ui1;
			_value.ui[1] = ui2;
		}

		void
		GraphicsVariant::uniform3ui(const uint3& value) noexcept
		{
			assert(_type == GraphicsUniformType::UInt3);
			_value.ui[0] = value.x;
			_value.ui[1] = value.y;
			_value.ui[2] = value.z;
		}

		void
		GraphicsVariant::uniform3ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3) noexcept
		{
			assert(_type == GraphicsUniformType::Float3);
			_value.ui[0] = ui1;
			_value.ui[1] = ui2;
			_value.ui[2] = ui3;
		}

		void
		GraphicsVariant::uniform4ui(const uint4& value) noexcept
		{
			assert(_type == GraphicsUniformType::UInt4);
			_value.ui[0] = value.x;
			_value.ui[1] = value.y;
			_value.ui[2] = value.z;
			_value.ui[3] = value.w;
		}

		void
		GraphicsVariant::uniform4ui(std::uint32_t ui1, std::uint32_t ui2, std::uint32_t ui3, std::uint32_t ui4) noexcept
		{
			assert(_type == GraphicsUniformType::UInt4);
			_value.ui[0] = ui1;
			_value.ui[1] = ui2;
			_value.ui[2] = ui3;
			_value.ui[3] = ui4;
		}

		void
		GraphicsVariant::uniform1f(float f1) noexcept
		{
			assert(_type == GraphicsUniformType::Float);
			_value.f[0] = f1;
		}

		void
		GraphicsVariant::uniform2f(const float2& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float2);
			_value.f[0] = value.x;
			_value.f[1] = value.y;
		}

		void
		GraphicsVariant::uniform2f(float f1, float f2) noexcept
		{
			assert(_type == GraphicsUniformType::Float2);
			_value.f[0] = f1;
			_value.f[1] = f2;
		}

		void
		GraphicsVariant::uniform3f(const float3& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float3);
			_value.f[0] = value.x;
			_value.f[1] = value.y;
			_value.f[2] = value.z;
		}

		void
		GraphicsVariant::uniform3f(float f1, float f2, float f3) noexcept
		{
			assert(_type == GraphicsUniformType::Float3);
			_value.f[0] = f1;
			_value.f[1] = f2;
			_value.f[2] = f3;
		}

		void
		GraphicsVariant::uniform4f(const float4& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float4);
			_value.f[0] = value.x;
			_value.f[1] = value.y;
			_value.f[2] = value.z;
			_value.f[3] = value.w;
		}

		void
		GraphicsVariant::uniform4f(float f1, float f2, float f3, float f4) noexcept
		{
			assert(_type == GraphicsUniformType::Float4);
			_value.f[0] = f1;
			_value.f[1] = f2;
			_value.f[2] = f3;
			_value.f[3] = f4;
		}

		void
		GraphicsVariant::uniform2fmat(const float2x2& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float2x2);
			*_value.m2 = value;
		}

		void
		GraphicsVariant::uniform2fmat(const float* mat2) noexcept
		{
			assert(_type == GraphicsUniformType::Float2x2);
			std::memcpy(_value.m2, mat2, sizeof(float2x2));
		}

		void
		GraphicsVariant::uniform3fmat(const float3x3& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float3x3);
			*_value.m3 = value;
		}

		void
		GraphicsVariant::uniform3fmat(const float* mat3) noexcept
		{
			assert(_type == GraphicsUniformType::Float3x3);
			std::memcpy(_value.m3, mat3, sizeof(float3x3));
		}

		void
		GraphicsVariant::uniform4fmat(const float4x4& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float4x4);
			*_value.m4 = value;
		}
		void
		GraphicsVariant::uniform4fmat(const float* mat4) noexcept
		{
			assert(_type == GraphicsUniformType::Float4x4);
			std::memcpy(_value.m4, mat4, sizeof(float4x4));
		}

		void
		GraphicsVariant::uniform1iv(const std::vector<int1>& value) noexcept
		{
			assert(_type == GraphicsUniformType::IntArray);
			*_value.iarray = value;
		}

		void
		GraphicsVariant::uniform1iv(std::size_t num, const std::int32_t* str) noexcept
		{
			assert(_type == GraphicsUniformType::IntArray);
			_value.iarray->resize(num);
			std::memcpy(_value.iarray->data(), str, sizeof(int1) * num);
		}

		void
		GraphicsVariant::uniform2iv(const std::vector<int2>& value) noexcept
		{
			assert(_type == GraphicsUniformType::Int2Array);
			*_value.iarray2 = value;
		}

		void
		GraphicsVariant::uniform2iv(std::size_t num, const std::int32_t* str) noexcept
		{
			assert(_type == GraphicsUniformType::Int2Array);
			_value.iarray2->resize(num);
			std::memcpy(_value.iarray2->data(), str, sizeof(int2) * num);
		}

		void
		GraphicsVariant::uniform3iv(const std::vector<int3>& value) noexcept
		{
			assert(_type == GraphicsUniformType::Int3Array);
			*_value.iarray3 = value;
		}

		void
		GraphicsVariant::uniform3iv(std::size_t num, const std::int32_t* str) noexcept
		{
			assert(_type == GraphicsUniformType::Int2Array);
			_value.iarray3->resize(num);
			std::memcpy(_value.iarray3->data(), str, sizeof(int3) * num);
		}

		void
		GraphicsVariant::uniform4iv(const std::vector<int4>& value) noexcept
		{
			assert(_type == GraphicsUniformType::Int4Array);
			*_value.iarray4 = value;
		}

		void
		GraphicsVariant::uniform4iv(std::size_t num, const std::int32_t* str) noexcept
		{
			assert(_type == GraphicsUniformType::Int2Array);
			_value.iarray4->resize(num);
			std::memcpy(_value.iarray4->data(), str, sizeof(int4) * num);
		}

		void
		GraphicsVariant::uniform1uiv(const std::vector<uint1>& value) noexcept
		{
			assert(_type == GraphicsUniformType::UIntArray);
			*_value.uiarray = value;
		}

		void
		GraphicsVariant::uniform1uiv(std::size_t num, const std::uint32_t* str) noexcept
		{
			assert(_type == GraphicsUniformType::UIntArray);
			_value.uiarray->resize(num);
			std::memcpy(_value.uiarray->data(), str, sizeof(uint1) * num);
		}

		void
		GraphicsVariant::uniform2uiv(const std::vector<uint2>& value) noexcept
		{
			assert(_type == GraphicsUniformType::UInt2Array);
			*_value.uiarray2 = value;
		}

		void
		GraphicsVariant::uniform2uiv(std::size_t num, const std::uint32_t* str) noexcept
		{
			assert(_type == GraphicsUniformType::UInt2Array);
			_value.uiarray2->resize(num);
			std::memcpy(_value.uiarray2->data(), str, sizeof(uint2) * num);
		}

		void
		GraphicsVariant::uniform3uiv(const std::vector<uint3>& value) noexcept
		{
			assert(_type == GraphicsUniformType::UInt3Array);
			*_value.uiarray3 = value;
		}

		void
		GraphicsVariant::uniform3uiv(std::size_t num, const std::uint32_t* str) noexcept
		{
			assert(_type == GraphicsUniformType::UInt3Array);
			_value.uiarray3->resize(num);
			std::memcpy(_value.uiarray3->data(), str, sizeof(uint3) * num);
		}

		void
		GraphicsVariant::uniform4uiv(const std::vector<uint4>& value) noexcept
		{
			assert(_type == GraphicsUniformType::UInt4Array);
			*_value.uiarray4 = value;
		}

		void
		GraphicsVariant::uniform4uiv(std::size_t num, const std::uint32_t* str) noexcept
		{
			assert(_type == GraphicsUniformType::UInt4Array);
			_value.uiarray4->resize(num);
			std::memcpy(_value.uiarray4->data(), str, sizeof(uint4) * num);
		}

		void
		GraphicsVariant::uniform1fv(const std::vector<float1>& value) noexcept
		{
			assert(_type == GraphicsUniformType::FloatArray);
			*_value.farray = value;
		}

		void
		GraphicsVariant::uniform1fv(std::size_t num, const float* str) noexcept
		{
			assert(_type == GraphicsUniformType::FloatArray);
			_value.farray->resize(num);
			std::memcpy(_value.farray->data(), str, sizeof(float1) * num);
		}

		void
		GraphicsVariant::uniform2fv(const std::vector<float2>& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float2Array);
			*_value.farray2 = value;
		}

		void
		GraphicsVariant::uniform2fv(std::size_t num, const float* str) noexcept
		{
			assert(_type == GraphicsUniformType::Float2Array);
			_value.farray2->resize(num);
			std::memcpy(_value.farray2->data(), str, sizeof(float2) * num);
		}

		void
		GraphicsVariant::uniform3fv(const std::vector<float3>& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float3Array);
			*_value.farray3 = value;
		}

		void
		GraphicsVariant::uniform3fv(std::size_t num, const float* str) noexcept
		{
			assert(_type == GraphicsUniformType::Float3Array);
			_value.farray3->resize(num);
			std::memcpy(_value.farray3->data(), str, sizeof(float3) * num);
		}

		void
		GraphicsVariant::uniform4fv(const std::vector<float4>& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float4Array);
			*_value.farray4 = value;
		}

		void
		GraphicsVariant::uniform4fv(std::size_t num, const float* str) noexcept
		{
			assert(_type == GraphicsUniformType::Float4Array);
			_value.farray4->resize(num);
			std::memcpy(_value.farray4->data(), str, sizeof(float4) * num);
		}

		void
		GraphicsVariant::uniform2fmatv(const std::vector<float2x2>& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float2x2Array);
			*_value.m2array = value;
		}

		void
		GraphicsVariant::uniform2fmatv(std::size_t num, const float* mat2) noexcept
		{
			assert(_type == GraphicsUniformType::Float4Array);
			_value.m2array->resize(num);
			std::memcpy(_value.m2array->data(), mat2, sizeof(float2x2) * num);
		}

		void
		GraphicsVariant::uniform3fmatv(const std::vector<float3x3>& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float3x3Array);
			*_value.m3array = value;
		}

		void
		GraphicsVariant::uniform3fmatv(std::size_t num, const float* mat3) noexcept
		{
			assert(_type == GraphicsUniformType::Float4Array);
			_value.m3array->resize(num);
			std::memcpy(_value.m3array->data(), mat3, sizeof(float3x3) * num);
		}

		void
		GraphicsVariant::uniform4fmatv(const std::vector<float4x4>& value) noexcept
		{
			assert(_type == GraphicsUniformType::Float4x4Array);
			*_value.m4array = value;
		}

		void
		GraphicsVariant::uniform4fmatv(std::size_t num, const float* mat4) noexcept
		{
			assert(_type == GraphicsUniformType::Float4Array);
			_value.m4array->resize(num);
			std::memcpy(_value.m4array->data(), mat4, sizeof(float4x4) * num);
		}

		void
		GraphicsVariant::uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept
		{
			assert(_type == GraphicsUniformType::StorageImage || _type == GraphicsUniformType::CombinedImageSampler || _type == GraphicsUniformType::SamplerImage);
			_value.texture->image = texture;
			_value.texture->sampler = sampler;
		}

		void
		GraphicsVariant::uniformBuffer(GraphicsDataPtr ubo) noexcept
		{
			assert(_type == GraphicsUniformType::UniformBuffer);
			*_value.ubo = ubo;
		}

		bool
		GraphicsVariant::getBool() const noexcept
		{
			assert(_type == GraphicsUniformType::Boolean);
			return _value.b;
		}

		int
		GraphicsVariant::getInt() const noexcept
		{
			assert(_type == GraphicsUniformType::Int);
			return _value.i[0];
		}

		const int2&
		GraphicsVariant::getInt2() const noexcept
		{
			assert(_type == GraphicsUniformType::Int2);
			return (int2&)_value.i;
		}

		const int3&
		GraphicsVariant::getInt3() const noexcept
		{
			assert(_type == GraphicsUniformType::Int3);
			return (int3&)_value.i;
		}

		const int4&
		GraphicsVariant::getInt4() const noexcept
		{
			assert(_type == GraphicsUniformType::Int4);
			return (int4&)_value.i;
		}

		uint1
		GraphicsVariant::getUInt() const noexcept
		{
			assert(_type == GraphicsUniformType::UInt);
			return _value.ui[0];
		}

		const uint2&
		GraphicsVariant::getUInt2() const noexcept
		{
			assert(_type == GraphicsUniformType::UInt2);
			return (uint2&)_value.ui;
		}

		const uint3&
		GraphicsVariant::getUInt3() const noexcept
		{
			assert(_type == GraphicsUniformType::UInt3);
			return (uint3&)_value.ui;
		}

		const uint4&
		GraphicsVariant::getUInt4() const noexcept
		{
			assert(_type == GraphicsUniformType::UInt4);
			return (uint4&)_value.ui;
		}

		float
		GraphicsVariant::getFloat() const noexcept
		{
			assert(_type == GraphicsUniformType::Float);
			return _value.f[0];
		}

		const float2&
		GraphicsVariant::getFloat2() const noexcept
		{
			assert(_type == GraphicsUniformType::Float2);
			return (float2&)_value.f;
		}

		const float3&
		GraphicsVariant::getFloat3() const noexcept
		{
			assert(_type == GraphicsUniformType::Float3);
			return (float3&)_value.f;
		}

		const float4&
		GraphicsVariant::getFloat4() const noexcept
		{
			assert(_type == GraphicsUniformType::Float4);
			return (float4&)_value.f;
		}

		const float2x2&
		GraphicsVariant::getFloat2x2() const noexcept
		{
			assert(_type == GraphicsUniformType::Float2x2);
			return (float2x2&)*_value.m2;
		}

		const float3x3&
		GraphicsVariant::getFloat3x3() const noexcept
		{
			assert(_type == GraphicsUniformType::Float3x3);
			return (float3x3&)*_value.m3;
		}

		const float4x4&
		GraphicsVariant::getFloat4x4() const noexcept
		{
			assert(_type == GraphicsUniformType::Float4x4);
			return (float4x4&)*_value.m4;
		}

		const std::vector<int1>&
		GraphicsVariant::getIntArray() const noexcept
		{
			assert(_type == GraphicsUniformType::IntArray);
			return *_value.iarray;
		}

		const std::vector<int2>&
		GraphicsVariant::getInt2Array() const noexcept
		{
			assert(_type == GraphicsUniformType::Int2Array);
			return *_value.iarray2;
		}

		const std::vector<int3>&
		GraphicsVariant::getInt3Array() const noexcept
		{
			assert(_type == GraphicsUniformType::Int3Array);
			return *_value.iarray3;
		}

		const std::vector<int4>&
		GraphicsVariant::getInt4Array() const noexcept
		{
			assert(_type == GraphicsUniformType::Int4Array);
			return *_value.iarray4;
		}

		const std::vector<uint1>&
		GraphicsVariant::getUIntArray() const noexcept
		{
			assert(_type == GraphicsUniformType::UIntArray);
			return *_value.uiarray;
		}

		const std::vector<uint2>&
		GraphicsVariant::getUInt2Array() const noexcept
		{
			assert(_type == GraphicsUniformType::UInt2Array);
			return *_value.uiarray2;
		}

		const std::vector<uint3>&
		GraphicsVariant::getUInt3Array() const noexcept
		{
			assert(_type == GraphicsUniformType::UInt3Array);
			return *_value.uiarray3;
		}

		const std::vector<uint4>&
		GraphicsVariant::getUInt4Array() const noexcept
		{
			assert(_type == GraphicsUniformType::UInt4Array);
			return *_value.uiarray4;
		}

		const std::vector<float>&
		GraphicsVariant::getFloatArray() const noexcept
		{
			assert(_type == GraphicsUniformType::FloatArray);
			return *_value.farray;
		}

		const std::vector<float2>&
		GraphicsVariant::getFloat2Array() const noexcept
		{
			assert(_type == GraphicsUniformType::Float2Array);
			return *_value.farray2;
		}

		const std::vector<float3>&
		GraphicsVariant::getFloat3Array() const noexcept
		{
			assert(_type == GraphicsUniformType::Float3Array);
			return *_value.farray3;
		}

		const std::vector<float4>&
		GraphicsVariant::getFloat4Array() const noexcept
		{
			assert(_type == GraphicsUniformType::Float4Array);
			return *_value.farray4;
		}

		const std::vector<float2x2>&
		GraphicsVariant::getFloat2x2Array() const noexcept
		{
			assert(_type == GraphicsUniformType::Float2x2Array);
			return *_value.m2array;
		}

		const std::vector<float3x3>&
		GraphicsVariant::getFloat3x3Array() const noexcept
		{
			assert(_type == GraphicsUniformType::Float3x3Array);
			return *_value.m3array;
		}

		const std::vector<float4x4>&
		GraphicsVariant::getFloat4x4Array() const noexcept
		{
			assert(_type == GraphicsUniformType::Float4x4Array);
			return *_value.m4array;
		}

		const GraphicsTexturePtr&
		GraphicsVariant::getTexture() const noexcept
		{
			assert(_type == GraphicsUniformType::SamplerImage || _type == GraphicsUniformType::StorageImage);
			return _value.texture->image;
		}

		const GraphicsSamplerPtr&
		GraphicsVariant::getTextureSampler() const noexcept
		{
			assert(_type == GraphicsUniformType::SamplerImage || _type == GraphicsUniformType::StorageImage);
			return _value.texture->sampler;
		}

		const GraphicsDataPtr&
		GraphicsVariant::getBuffer() const noexcept
		{
			assert(_type == GraphicsUniformType::UniformBuffer);
			return *_value.ubo;
		}
	}
}