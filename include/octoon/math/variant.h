#ifndef OCTOON_VARIANT_H_
#define OCTOON_VARIANT_H_

#include <octoon/math/mat4.h>
#include <octoon/math/vector2.h>
#include <octoon/math/vector4.h>
#include <octoon/runtime/platform.h>
#include <stdexcept>

namespace octoon::math
{
	class OCTOON_EXPORT Variant final
	{
	public:
		enum class Type
		{
			Void,
			Bool,
			Int,
			Float,
			Float2,
			Float3,
			Float4,
			Quaternion,
			String,
			Float4x4,
			Blob,
			Guid,
			Object,
			IntArray,
			FloatArray,
			BoolArray,
			Float4Array,
			StringArray,
			Float4x4Array,
		};

		Variant();
		Variant(int rhs);
		Variant(bool rhs);
		Variant(const void* ptr);
		Variant(const float1& v);
		Variant(const float2& v);
		Variant(const float3& v);
		Variant(const float4& v);
		Variant(const float4x4& m);
		Variant(const Quaternion& val);
		Variant(const std::string& rhs);
		Variant(const std::vector<int>& rhs);
		Variant(const std::vector<float>& rhs);
		Variant(const std::vector<bool>& rhs);
		Variant(const std::vector<float4>& rhs);
		Variant(const std::vector<float4x4>& rhs);
		Variant(const std::vector<std::string>& rhs);
		Variant(const Variant& rhs);
		Variant(Variant&& rhs);

		~Variant();

		void setType(Type t);
		Type getType() const;
		
		void assign(int val) { this->setInt(val); }
		void assign(bool val) { this->setBool(val); }
		void assign(const void* ptr) { this->setObject(ptr); }
		void assign(const float1& val) { this->setFloat(val); }
		void assign(const float2& val) { this->setFloat2(val); }
		void assign(const float3& val) { this->setFloat3(val); }
		void assign(const float4& val) { this->setFloat4(val); }
		void assign(const float4x4& val) { this->setFloat4x4(val); }
		void assign(const Quaternion& val) { this->setQuaternion(val); }
		void assign(const std::string& val) { this->setString(val); }
		void assign(const std::vector<int>& val) { this->setIntArray(val); }
		void assign(const std::vector<float>& val) { this->setFloatArray(val); }
		void assign(const std::vector<bool>& val) { this->setBoolArray(val); }
		void assign(const std::vector<float4>& val) { this->setFloat4Array(val); }
		void assign(const std::vector<float4x4>& val) { this->setFloat4x4Array(val); }
		void assign(const std::vector<std::string>& val) { this->setStringArray(val); }
		void assign(const Variant& variant) { this->setVariant(variant); }
		void assign(Variant&& variant) { this->setVariant(std::move(variant)); }

		void setInt(int val);
		void setBool(bool val);
		void setFloat(const float1& val);
		void setFloat2(const float2& val);
		void setFloat3(const float3& val);
		void setFloat4(const float4& val);
		void setFloat4x4(const float4x4& val);
		void setQuaternion(const Quaternion& val);
		void setObject(const void* ptr);
		void setString(const std::string& val);
		void setIntArray(const std::vector<int>& val);
		void setFloatArray(const std::vector<float>& val);
		void setBoolArray(const std::vector<bool>& val);
		void setFloat4Array(const std::vector<float4>& val);
		void setFloat4x4Array(const std::vector<float4x4>& val);
		void setStringArray(const std::vector<std::string>& val);
		void setVariant(const Variant& rhs);
		void setVariant(Variant&& rhs);

		int getInt() const;
		bool getBool() const;
		const std::string& getString() const;
		const float1& getFloat() const;
		const float2& getFloat2() const;
		const float3& getFloat3() const;
		const float4& getFloat4() const;
		const Quaternion& getQuaternion() const;
		const float4x4& getFloat4x4() const;
		const void* getObject() const;
		const std::vector<int>& getIntArray() const;
		const std::vector<float>& getFloatArray() const;
		const std::vector<bool>& getBoolArray() const;
		const std::vector<float4>& getFloat4Array() const;
		const std::vector<float4x4>& getFloat4x4Array() const;
		const std::vector<std::string>& getStringArray() const;

		static std::string to_string(Type t);
		static Type to_type(const std::string& str);

		void operator=(bool val) { this->assign(val); }
		void operator=(int val) { this->assign(val); }
		void operator=(const float1& val) { this->assign(val); }
		void operator=(const float2& val) { this->assign(val); }
		void operator=(const float3& val) { this->assign(val); }
		void operator=(const float4& val) { this->assign(val); }
		void operator=(const math::Quaternion& val) { this->assign(val); }
		void operator=(const std::string& val) { this->assign(val); }
		void operator=(const float4x4& val) { this->assign(val); }
		void operator=(const void* val) { this->assign(val); }
		void operator=(const std::vector<int>& val) { this->assign(val); }
		void operator=(const std::vector<float>& val) { this->assign(val); }
		void operator=(const std::vector<bool>& val) { this->assign(val); }
		void operator=(const std::vector<float4>& val) { this->assign(val); }
		void operator=(const std::vector<float4x4>& val) { this->assign(val); }
		void operator=(const std::vector<std::string>& val) { this->assign(val); }
		void operator=(const Variant& val) { this->assign(val); }
		void operator=(Variant&& val) { this->assign(std::move(val)); }

	public:
		friend bool operator==(const Variant& lhs, int rhs)
		{
			return (lhs.type_ == Type::Int && lhs.value_.i.x == rhs);
		}

		friend bool operator==(const Variant& lhs, float rhs)
		{
			return (lhs.type_ == Type::Float && lhs.value_.f.x == rhs);
		}

		friend bool operator==(const Variant& lhs, bool rhs)
		{
			return (lhs.type_ == Type::Bool && lhs.value_.b == rhs);
		}

		friend bool operator==(const Variant& lhs, const void* ptr)
		{
			return (lhs.type_ == Type::Object && lhs.value_.object == ptr);
		}

		friend bool operator==(const Variant& lhs, const float4& rhs)
		{
			return (lhs.type_ == Type::Float4 || math::equal(lhs.value_.f.x, rhs.x) || math::equal(lhs.value_.f.y, rhs.y) || math::equal(lhs.value_.f.z, rhs.z) || math::equal(lhs.value_.f.w, rhs.w));
		}

		friend bool operator==(const Variant& lhs, const math::Quaternion& rhs)
		{
			return (lhs.type_ == Type::Quaternion && lhs.value_.quaternion == rhs);
		}

		friend bool operator==(const Variant& lhs, const std::string& rhs)
		{
			return (lhs.type_ == Type::String && *lhs.value_.string == rhs);
		}

		friend bool operator==(const Variant& lhs, const Variant& rhs)
		{
			if (lhs.type_ == rhs.type_)
			{
				switch (lhs.type_)
				{
				case Type::Void:
					return true;
				case Type::Int:
					return (lhs.value_.i == rhs.value_.i);
				case Type::Bool:
					return (lhs.value_.b == rhs.value_.b);
				case Type::Float:
					return (lhs.value_.f.x == rhs.value_.f.x);
				case Type::Quaternion:
					return (lhs.value_.quaternion == rhs.value_.quaternion);
				case Type::String:
					return ((*lhs.value_.string) == (*rhs.value_.string));
				case Type::Float4:
					return ((lhs.value_.f.x == rhs.value_.f.x) && (lhs.value_.f.y == rhs.value_.f.y) && (lhs.value_.f.z == rhs.value_.f.z) && (lhs.value_.f.w == rhs.value_.f.w));
				case Type::Object:
					return (lhs.value_.object == rhs.value_.object);
				case Type::Float4x4:
					return lhs.value_.matrix == rhs.value_.matrix;
				default:
					throw std::runtime_error("Variant::operator==(): invalid variant type!");
				}
			}

			return false;
		}

		friend bool operator!=(const Variant& lhs, int rhs)
		{
			return (lhs.type_ != Type::Int || lhs.value_.i.x != rhs);
		}

		friend bool operator!=(const Variant& lhs, float rhs)
		{
			return (lhs.type_ != Type::Float || lhs.value_.f.x != rhs);
		}

		friend bool operator!=(const Variant& lhs, bool rhs)
		{
			return (lhs.type_ != Type::Bool || lhs.value_.b != rhs);
		}

		friend bool operator!=(const Variant& lhs, const void* ptr)
		{
			return (lhs.type_ != Type::Object || lhs.value_.object != ptr);
		}

		friend bool operator!=(const Variant& lhs, const std::string& rhs)
		{
			return (lhs.type_ != Type::String || *lhs.value_.string != rhs);
		}

		friend bool operator!=(const Variant& lhs, const float4& rhs)
		{
			return (lhs.type_ != Type::Float4 || !math::equal(lhs.value_.f.x, rhs.x) || !math::equal(lhs.value_.f.y, rhs.y) || !math::equal(lhs.value_.f.z, rhs.z) || !math::equal(lhs.value_.f.w, rhs.w));
		}

		friend bool operator!=(const Variant& lhs, const math::Quaternion& rhs)
		{
			return (lhs.type_ != Type::Quaternion || lhs.value_.quaternion != rhs);
		}

		friend bool operator!=(const Variant& lhs, const Variant& rhs)
		{
			return !(lhs == rhs);
		}

		friend bool operator>(const Variant& lhs, const Variant& rhs)
		{
			assert(lhs.type_ == rhs.type_);

			switch (rhs.type_)
			{
			case Type::Void:
				return true;
			case Type::Int:
				return (lhs.value_.i.x > rhs.value_.i.x);
			case Type::Bool:
				return (lhs.value_.b > rhs.value_.b);
			case Type::Float:
				return (lhs.value_.f.x > rhs.value_.f.x);
			case Type::String:
				return ((*lhs.value_.string) > (*rhs.value_.string));
			case Type::Float4:
				return ((lhs.value_.f.x > rhs.value_.f.x) && (lhs.value_.f.y > rhs.value_.f.y) && (lhs.value_.f.z > rhs.value_.f.z) && (lhs.value_.f.w > rhs.value_.f.w));
			case Type::Object:
				return (lhs.value_.object > rhs.value_.object);
			default:
				throw std::runtime_error("Variant::operator>(): invalid variant type!");
			}

			return false;
		}

		friend bool operator<(const Variant& lhs, const Variant& rhs)
		{
			assert(lhs.type_ == rhs.type_);
			
			switch (rhs.type_)
			{
			case Type::Void:
				return true;
			case Type::Int:
				return (lhs.value_.i.x < rhs.value_.i.x);
			case Type::Bool:
				return (lhs.value_.b < rhs.value_.b);
			case Type::Float:
				return (lhs.value_.f.x < rhs.value_.f.x);
			case Type::String:
				return ((*lhs.value_.string) < (*rhs.value_.string));
			case Type::Float4:
				return ((lhs.value_.f.x < rhs.value_.f.x) && (lhs.value_.f.y < rhs.value_.f.y) && (lhs.value_.f.z < rhs.value_.f.z) && (lhs.value_.f.w < rhs.value_.f.w));
			case Type::Object:
				return (lhs.value_.object < rhs.value_.object);
			default:
				throw std::runtime_error("Variant::operator<(): invalid variant type!");
			}
		}

		friend bool operator>=(const Variant& lhs, const Variant& rhs)
		{
			assert(lhs.type_ == rhs.type_);

			switch (rhs.type_)
			{
			case Type::Void:
				return true;
			case Type::Int:
				return (lhs.value_.i.x >= rhs.value_.i.x);
			case Type::Bool:
				return (lhs.value_.b >= rhs.value_.b);
			case Type::Float:
				return (lhs.value_.f.x >= rhs.value_.f.x);
			case Type::String:
				return ((*lhs.value_.string) >= (*rhs.value_.string));
			case Type::Float4:
				return ((lhs.value_.f.x >= rhs.value_.f.x) && (lhs.value_.f.y >= rhs.value_.f.y) && (lhs.value_.f.z >= rhs.value_.f.z) && (lhs.value_.f.w >= rhs.value_.f.w));
			case Type::Object:
				return (lhs.value_.object >= rhs.value_.object);
			default:
				throw std::runtime_error("Variant::operator>(): invalid variant type!");
			}
		}

		friend bool operator<=(const Variant& lhs, const Variant& rhs)
		{
			assert(lhs.type_ == rhs.type_);

			switch (rhs.type_)
			{
			case Type::Void:
				return true;
			case Type::Int:
				return (lhs.value_.i.x <= rhs.value_.i.x);
			case Type::Bool:
				return (lhs.value_.b <= rhs.value_.b);
			case Type::Float:
				return (lhs.value_.f.x <= rhs.value_.f.x);
			case Type::String:
				return ((*lhs.value_.string) <= (*rhs.value_.string));
			case Type::Float4:
				return ((lhs.value_.f.x <= rhs.value_.f.x) && (lhs.value_.f.y <= rhs.value_.f.y) && (lhs.value_.f.z <= rhs.value_.f.z) && (lhs.value_.f.w <= rhs.value_.f.w));
			case Type::Object:
				return (lhs.value_.object <= rhs.value_.object);
			default:
				throw std::runtime_error("Variant::operator<(): invalid variant type!");
			}
		}

		friend Variant operator+(const Variant& lhs, const Variant& rhs) noexcept(false)
		{
			assert(lhs.type_ == rhs.type_);

			switch (lhs.type_)
			{
			case Type::Int:
				return Variant(lhs.value_.i.x + rhs.value_.i.x);
			case Type::Float:
				return Variant(lhs.value_.f.x + rhs.value_.f.x);
			case Type::Float2:
				return Variant(lhs.value_.f.xy() + rhs.value_.f.xy());
			case Type::Float3:
				return Variant(lhs.value_.f.xyz() + rhs.value_.f.xyz());
			case Type::Float4:
				return Variant(lhs.value_.f + rhs.value_.f);
			case Type::Quaternion:
				return Variant(lhs.value_.quaternion + rhs.value_.quaternion);
			default:
				throw std::runtime_error("Variant::operator+(): invalid variant type!");
			}
		}

		friend Variant operator*(const Variant& v, float value) noexcept(false)
		{
			switch (v.type_)
			{
			case Type::Int:
				return Variant(v.value_.i.x * value);
			case Type::Float:
				return Variant(v.value_.f.x * value);
			case Type::Float2:
				return Variant(v.value_.f.xy() * value);
			case Type::Float3:
				return Variant(v.value_.f.xyz() * value);
			case Type::Float4:
				return Variant(v.value_.f * value);
			case Type::Quaternion:
				return Variant(v.value_.quaternion * value);
			default:
				throw std::runtime_error("Variant::operator*(): invalid variant type!");
			}
		}

	private:
		Type type_;

		union
		{
			bool b;
			int4 i;
			float4 f;
			float4x4* matrix;
			Quaternion quaternion;
			std::string* string;
			std::vector<int>* intArray;
			std::vector<float>* floatArray;
			std::vector<bool>* boolArray;
			std::vector<float4>* float4Array;
			std::vector<float4x4>* float4x4Array;
			std::vector<std::string>* stringArray;
			const void* object;
		} value_;
	};

	inline Variant interpolator(const Variant& lhs, const Variant& rhs, float t) noexcept(false)
	{
		assert(lhs.getType() == rhs.getType());

		switch (lhs.getType())
		{
		case Variant::Type::Int:
			return Variant(lerp(lhs.getInt(), rhs.getInt(), t));
		case Variant::Type::Float:
			return Variant(lerp(lhs.getFloat(), rhs.getFloat(), t));
		case Variant::Type::Float2:
			return Variant(lerp(lhs.getFloat2(), rhs.getFloat2(), t));
		case Variant::Type::Float3:
			return Variant(lerp(lhs.getFloat3(), rhs.getFloat3(), t));
		case Variant::Type::Float4:
			return Variant(lerp(lhs.getFloat4(), rhs.getFloat4(), t));
		case Variant::Type::Quaternion:
			return Variant(slerp(lhs.getQuaternion(), rhs.getQuaternion(), t));
		default:
			throw std::runtime_error("Variant::interpolator(): invalid variant type!");
		}
	}
}

#endif