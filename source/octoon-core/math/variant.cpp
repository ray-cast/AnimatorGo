#include <octoon/math/variant.h>

namespace octoon::math
{
	Variant::Variant()
		: type_(Type::Void)
	{
		value_.i = int4::Zero;
	}

	Variant::Variant(bool rhs)
		: type_(Type::Bool)
	{
		value_.b = rhs;
	}

	Variant::Variant(int rhs)
		: type_(Type::Int)
	{
		value_.i.x = rhs;
	}

	Variant::Variant(const void* ptr)
		: type_(Type::Object)
	{
		value_.object = ptr;
	}

	Variant::Variant(const float1& rhs)
		: type_(Type::Float)
	{
		value_.f.x = rhs;
	}

	Variant::Variant(const float2& rhs)
		: type_(Type::Float2)
	{
		value_.f.x = rhs.x;
		value_.f.y = rhs.y;
	}

	Variant::Variant(const float3& rhs)
		: type_(Type::Float3)
	{
		value_.f.x = rhs.x;
		value_.f.y = rhs.y;
		value_.f.z = rhs.z;
	}

	Variant::Variant(const float4& rhs)
		: type_(Type::Float4)
	{
		value_.f.x = rhs.x;
		value_.f.y = rhs.y;
		value_.f.z = rhs.z;
		value_.f.w = rhs.w;
	}

	Variant::Variant(const Quaternion& rhs)
		: type_(Type::Quaternion)
	{
		value_.quaternion = rhs;
	}

	Variant::Variant(const float4x4& rhs)
		: type_(Type::Float4x4)
	{
		value_.matrix = new float4x4(rhs);
	}

	Variant::Variant(const std::string& rhs)
		: type_(Type::String)
	{
		value_.string = new std::string(rhs);
	}

	Variant::Variant(const std::vector<int>& rhs)
		: type_(Type::IntArray)
	{
		value_.intArray = new std::vector<int>(rhs);
	}

	Variant::Variant(const std::vector<float>& rhs)
		: type_(Type::FloatArray)
	{
		value_.floatArray = new std::vector<float>(rhs);
	}

	Variant::Variant(const std::vector<bool>& rhs)
		: type_(Type::BoolArray)
	{
		value_.boolArray = new std::vector<bool>(rhs);
	}

	Variant::Variant(const std::vector<float4>& rhs)
		: type_(Type::Float4Array)
	{
		value_.float4Array = new std::vector<float4>(rhs);
	}

	Variant::Variant(const std::vector<float4x4>& rhs)
		: type_(Type::Float4x4Array)
	{
		value_.float4x4Array = new std::vector<float4x4>(rhs);
	}

	Variant::Variant(const std::vector<std::string>& rhs)
		: type_(Type::StringArray)
	{
		value_.stringArray = new std::vector<std::string>(rhs);
	}

	Variant::Variant(const Variant& rhs)
		: type_(Type::Void)
	{
		this->setType(rhs.type_);
		this->setVariant(rhs);
	}

	Variant::Variant(Variant&& rhs)
		: type_(Type::Void)
	{
		this->setVariant(std::move(rhs));
	}

	Variant::~Variant()
	{
		this->setType(Type::Void);
	}

	void
	Variant::setType(Type type)
	{
		switch (type_)
		{
		case Type::String: delete value_.string; break;
		case Type::Float4x4: delete value_.matrix; break;
		case Type::IntArray: delete value_.intArray; break;
		case Type::FloatArray: delete value_.floatArray; break;
		case Type::BoolArray: delete value_.boolArray; break;
		case Type::Float4Array: delete value_.float4Array; break;
		case Type::Float4x4Array: delete value_.float4x4Array; break;
		case Type::StringArray: delete value_.stringArray; break;
		}

		type_ = type;

		switch (type_)
		{
		case Type::String: value_.string = new std::string; break;
		case Type::Float4x4: value_.matrix = new float4x4; break;
		case Type::IntArray: value_.intArray = new std::vector<int>; break;
		case Type::FloatArray: value_.floatArray = new std::vector<float>; break;
		case Type::BoolArray: value_.boolArray = new std::vector<bool>; break;
		case Type::Float4Array: value_.float4Array = new std::vector<float4>; break;
		case Type::Float4x4Array: value_.float4x4Array = new std::vector<float4x4>; break;
		case Type::StringArray: value_.stringArray = new std::vector<std::string>; break;
		default:
			break;
		}
	}

	Variant::Type
	Variant::getType() const
	{
		return type_;
	}

	void
	Variant::setInt(int val)
	{
		assert(type_ == Type::Int);
		value_.i.x = val;
	}

	void
	Variant::setBool(bool val)
	{
		assert(type_ == Type::Bool);
		value_.b = val;
	}

	void
	Variant::setString(const std::string& val)
	{
		assert(type_ == Type::String);
		*value_.string = val;
	}

	void
	Variant::setFloat(const float1& val)
	{
		assert(type_ == Type::Float);
		value_.f.x = val;
	}

	void
	Variant::setFloat2(const float2& val)
	{
		assert(type_ == Type::Float2);
		value_.f.x = val.x;
		value_.f.y = val.y;
	}

	void
	Variant::setFloat3(const float3& val)
	{
		assert(type_ == Type::Float3);
		value_.f.x = val.x;
		value_.f.y = val.y;
		value_.f.z = val.z;
	}

	void
	Variant::setFloat4(const float4& val)
	{
		assert(type_ == Type::Float4);
		value_.f.x = val.x;
		value_.f.y = val.y;
		value_.f.z = val.z;
		value_.f.w = val.w;
	}

	void
	Variant::setFloat4x4(const float4x4& val)
	{
		assert(type_ == Type::Float4x4);
		*value_.matrix = val;
	}

	void
	Variant::setQuaternion(const Quaternion& val)
	{
		assert(type_ == Type::Quaternion);
		value_.quaternion = val;
	}

	void
	Variant::setObject(const void* ptr)
	{
		assert(type_ == Type::Object);
		value_.object = ptr;
	}

	void
	Variant::setBoolArray(const std::vector<bool>& val)
	{
		assert(type_ == Type::BoolArray);
		*value_.boolArray = val;
	}

	void
	Variant::setIntArray(const std::vector<int>& val)
	{
		assert(type_ == Type::IntArray);
		*value_.intArray = val;
	}

	void
	Variant::setFloatArray(const std::vector<float>& val)
	{
		assert(type_ == Type::FloatArray);
		*value_.floatArray = val;
	}

	void
	Variant::setFloat4Array(const std::vector<float4>& val)
	{
		assert(type_ == Type::Float4Array);
		*value_.float4Array = val;
	}

	void
	Variant::setFloat4x4Array(const std::vector<float4x4>& val)
	{
		assert(type_ == Type::Float4x4Array);
		*value_.float4x4Array = val;
	}

	void
	Variant::setStringArray(const std::vector<std::string>& val)
	{
		assert(type_ == Type::StringArray);
		*value_.stringArray = val;
	}

	void
	Variant::setVariant(const Variant& rhs)
	{
		assert(type_ == rhs.type_);

		switch (rhs.type_)
		{
		case Type::Void:
			break;
		case Type::Bool:
			value_.b = rhs.value_.b;
			break;
		case Type::Int:
			value_.i = rhs.value_.i;
			break;
		case Type::Float:
			value_.f.x = rhs.value_.f.x;
			break;
		case Type::Float2:
			value_.f.x = rhs.value_.f.x;
			value_.f.y = rhs.value_.f.y;
			break;
		case Type::Float3:
			value_.f.x = rhs.value_.f.x;
			value_.f.y = rhs.value_.f.y;
			value_.f.z = rhs.value_.f.z;
			break;
		case Type::Float4:
			value_.f.x = rhs.value_.f.x;
			value_.f.y = rhs.value_.f.y;
			value_.f.z = rhs.value_.f.z;
			value_.f.w = rhs.value_.f.w;
			break;
		case Type::Quaternion:
			value_.quaternion = rhs.value_.quaternion;
			break;
		case Type::Object:
			value_.object = rhs.value_.object;
			break;
		case Type::String:
			*value_.string = *rhs.value_.string;
			break;
		case Type::Float4x4:
			*value_.matrix = *value_.matrix;
			break;
		case Type::IntArray:
			*value_.intArray = *value_.intArray;
			break;
		case Type::FloatArray:
			*value_.floatArray = *value_.floatArray;
			break;
		case Type::BoolArray:
			*value_.boolArray = *value_.boolArray;
			break;
		case Type::Float4Array:
			*value_.float4Array = *value_.float4Array;
			break;
		case Type::Float4x4Array:
			*value_.float4x4Array = *value_.float4x4Array;
			break;
		case Type::StringArray:
			*value_.stringArray = *value_.stringArray;
			break;
		default:
			throw std::runtime_error("Variant::copy(): invalid type!");
			break;
		}
	}

	void
	Variant::setVariant(Variant&& rhs)
	{
		this->setType(Type::Void);

		type_ = rhs.type_;

		switch (type_)
		{
		case Type::Void:
			break;
		case Type::Bool:
			value_.b = rhs.value_.b;
			break;
		case Type::Int:
			value_.i = rhs.value_.i;
			break;
		case Type::Float:
			value_.f.x = rhs.value_.f.x;
			break;
		case Type::Float2:
			value_.f.x = rhs.value_.f.x;
			value_.f.y = rhs.value_.f.y;
			break;
		case Type::Float3:
			value_.f.x = rhs.value_.f.x;
			value_.f.y = rhs.value_.f.y;
			value_.f.z = rhs.value_.f.z;
			break;
		case Type::Float4:
			value_.f.x = rhs.value_.f.x;
			value_.f.y = rhs.value_.f.y;
			value_.f.z = rhs.value_.f.z;
			value_.f.w = rhs.value_.f.w;
			break;
		case Type::Quaternion:
			value_.quaternion = rhs.value_.quaternion;
			break;
		case Type::Object:
			value_.object = rhs.value_.object;
			rhs.value_.object = nullptr;
			break;
		case Type::String:
			value_.string = rhs.value_.string;
			rhs.value_.string = nullptr;
			break;
		case Type::Float4x4:
			value_.matrix = value_.matrix;
			rhs.value_.matrix = nullptr;
			break;
		case Type::IntArray:
			value_.intArray = value_.intArray;
			rhs.value_.intArray = nullptr;
			break;
		case Type::FloatArray:
			value_.floatArray = value_.floatArray;
			rhs.value_.floatArray = nullptr;
			break;
		case Type::BoolArray:
			value_.boolArray = value_.boolArray;
			rhs.value_.boolArray = nullptr;
			break;
		case Type::Float4Array:
			value_.float4Array = value_.float4Array;
			rhs.value_.float4Array = nullptr;
			break;
		case Type::Float4x4Array:
			value_.float4x4Array = value_.float4x4Array;
			rhs.value_.float4x4Array = nullptr;
			break;
		case Type::StringArray:
			value_.stringArray = value_.stringArray;
			rhs.value_.stringArray = nullptr;
			break;
		default:
			throw std::runtime_error("Variant::copy(): invalid type!");
			break;
		}
	}

	int
	Variant::getInt() const
	{
		assert(Type::Int == type_);
		return value_.i.x;
	}

	bool
	Variant::getBool() const
	{
		assert(Type::Bool == type_);
		return value_.b;
	}

	const std::string&
	Variant::getString() const
	{
		assert(Type::String == type_);
		return *(value_.string);
	}

	const float1&
	Variant::getFloat() const
	{
		assert(Type::Float == type_);
		return (float1&)value_.f;
	}

	const float2&
	Variant::getFloat2() const
	{
		assert(Type::Float2 == type_);
		return (float2&)value_.f;
	}

	const float3&
	Variant::getFloat3() const
	{
		assert(Type::Float3 == type_);
		return (float3&)value_.f;
	}

	const float4&
	Variant::getFloat4() const
	{
		assert(Type::Float4 == type_);
		return (float4&)value_.f;
	}

	const math::Quaternion&
	Variant::getQuaternion() const
	{
		assert(Type::Quaternion == type_);
		return value_.quaternion;
	}

	const float4x4&
	Variant::getFloat4x4() const
	{
		assert(Type::Float4x4 == type_);
		return *(value_.matrix);
	}

	const void*
	Variant::getObject() const
	{
		assert(Type::Object == type_);
		return value_.object;
	}

	const std::vector<int>&
	Variant::getIntArray() const
	{
		assert(Type::IntArray == type_);
		return *(value_.intArray);
	}

	const std::vector<float>&
	Variant::getFloatArray() const
	{
		assert(Type::FloatArray == type_);
		return *(value_.floatArray);
	}

	const std::vector<bool>&
	Variant::getBoolArray() const
	{
		assert(Type::BoolArray == type_);
		return *(value_.boolArray);
	}

	const std::vector<float4>&
	Variant::getFloat4Array() const
	{
		assert(Type::Float4Array == type_);
		return *(value_.float4Array);
	}

	const std::vector<float4x4>&
	Variant::getFloat4x4Array() const
	{
		assert(Type::Float4x4Array == type_);
		return *(value_.float4x4Array);
	}

	const std::vector<std::string>&
	Variant::getStringArray() const
	{
		assert(Type::StringArray == type_);
		return *(value_.stringArray);
	}

	std::string
	Variant::to_string(Type t)
	{
		switch (t)
		{
		case Type::Void:          return "void";
		case Type::Int:           return "int";
		case Type::Float:         return "float";
		case Type::Bool:          return "bool";
		case Type::Float4:        return "float4";
		case Type::String:        return "string";
		case Type::Float4x4:      return "float4x4";
		case Type::Blob:          return "blob";
		case Type::Guid:          return "guid";
		case Type::Object:        return "object";
		case Type::IntArray:      return "intarray";
		case Type::FloatArray:    return "floatarray";
		case Type::BoolArray:     return "boolarray";
		case Type::Float4Array:   return "float4array";
		case Type::Float4x4Array: return "float4x4Array";
		case Type::StringArray:   return "stringarray";
		default:
			throw std::runtime_error("Variant::typeToString(): invalid type enum!");
		}
	}

	Variant::Type
	Variant::to_type(const std::string& str)
	{
		if ("void" == str)					return Type::Void;
		else if ("int" == str)              return Type::Int;
		else if ("float" == str)            return Type::Float;
		else if ("bool" == str)             return Type::Bool;
		else if ("float4" == str)           return Type::Float4;
		else if ("color" == str)            return Type::Float4; // NOT A BUG!
		else if ("string" == str)           return Type::String;
		else if ("Float4x4" == str)			return Type::Float4x4;
		else if ("blob" == str)             return Type::Blob;
		else if ("guid" == str)             return Type::Guid;
		else if ("object" == str)           return Type::Object;
		else if ("intarray" == str)         return Type::IntArray;
		else if ("floatarray" == str)       return Type::FloatArray;
		else if ("boolarray" == str)        return Type::BoolArray;
		else if ("float4array" == str)      return Type::Float4Array;
		else if ("Float4x4array" == str)	return Type::Float4x4Array;
		else if ("stringarray" == str)      return Type::StringArray;
		else
		{
			throw std::runtime_error("Variant::stringToType(): invalid type string!");
		}
	}
}