#include <octoon/io/variant.h>

namespace octoon
{
	namespace io
	{
		Variant::Variant()
			: _type(Type::Void)
		{
			_value.i = 0;
		}

		Variant::Variant(const Variant& rhs)
			: _type(Type::Void)
		{
			this->copy(rhs);
		}

		Variant::Variant(int rhs)
			: _type(Type::Int)
		{
			_value.i = rhs;
		}

		Variant::Variant(float rhs)
			: _type(Type::Float)
		{
			_value.f[0] = rhs;
		}

		Variant::Variant(bool rhs)
			: _type(Type::Bool)
		{
			_value.b = rhs;
		}

		Variant::Variant(const float4& rhs)
			: _type(Type::Float4)
		{
			_value.f[0] = rhs.x;
			_value.f[1] = rhs.y;
			_value.f[2] = rhs.z;
			_value.f[3] = rhs.w;
		}

		Variant::Variant(const std::string& rhs)
			:_type(Type::String)
		{
			_value.string = new std::string(rhs);
		}

		Variant::Variant(const char* chrPtr)
			: _type(Type::String)
		{
			_value.string = new std::string(chrPtr);
		}

		Variant::Variant(const void* ptr)
			: _type(Type::Object)
		{
			_value.object = ptr;
		}

		Variant::Variant(const float4x4& rhs)
			: _type(Type::Float4x4)
		{
			_value.matrix = new float4x4(rhs);
		}

		Variant::Variant(const std::vector<int>& rhs)
			: _type(Type::IntArray)
		{
			_value.intArray = new std::vector<int>(rhs);
		}

		Variant::Variant(const std::vector<float>& rhs)
			: _type(Type::FloatArray)
		{
			_value.floatArray = new std::vector<float>(rhs);
		}

		Variant::Variant(const std::vector<bool>& rhs)
			: _type(Type::BoolArray)
		{
			_value.boolArray = new std::vector<bool>(rhs);
		}

		Variant::Variant(const std::vector<float4>& rhs)
			: _type(Type::Float4Array)
		{
			_value.float4Array = new std::vector<float4>(rhs);
		}

		Variant::Variant(const std::vector<float4x4>& rhs)
			: _type(Type::Float4x4Array)
		{
			_value.float4x4Array = new std::vector<float4x4>(rhs);
		}

		Variant::Variant(const std::vector<std::string>& rhs)
			: _type(Type::StringArray)
		{
			_value.stringArray = new std::vector<std::string>(rhs);
		}

		Variant::~Variant()
		{
			this->destroy();
		}

		void
		Variant::setType(Type t)
		{
			this->destroy();
			_type = Type::t;
			switch (t)
			{
			case Type::String:
				_value.string = new std::string;
				break;
			case Type::Float4x4:
				_value.matrix = new float4x4;
				break;
			case Type::Object:
				_value.object = nullptr;
				break;
			case Type::IntArray:
				_value.intArray = new std::vector<int>;
				break;
			case Type::FloatArray:
				_value.floatArray = new std::vector<float>;
				break;
			case Type::BoolArray:
				_value.boolArray = new std::vector<bool>;
				break;
			case Type::Float4Array:
				_value.float4Array = new std::vector<float4>;
				break;
			case Type::Float4x4Array:
				_value.float4x4Array = new std::vector<float4x4>;
				break;
			case Type::StringArray:
				_value.stringArray = new std::vector<std::string>;
				break;
			default:
				break;
			}
		}

		Variant::Type
		Variant::getType() const
		{
			return _type;
		}

		void
		Variant::operator=(const Variant& rhs)
		{
			this->destroy();
			this->copy(rhs);
		}

		void
		Variant::operator=(int val)
		{
			this->destroy();
			_type = Type::Int;
			_value.i = val;
		}

		void
		Variant::operator=(float val)
		{
			this->destroy();
			_type = Type::Float;
			_value.f[0] = val;
		}

		void
		Variant::operator=(bool val)
		{
			this->destroy();
			_type = Type::Bool;
			_value.b = val;
		}

		void
		Variant::operator=(const float4& val)
		{
			this->destroy();
			_type = Type::Float4;
			_value.f[0] = val.x;
			_value.f[1] = val.y;
			_value.f[2] = val.z;
			_value.f[3] = val.w;
		}

		void
		Variant::operator=(const std::string& s)
		{
			if (Type::String == _type)
			{
				*_value.string = s;
			}
			else
			{
				this->destroy();
				_value.string = new std::string(s);
			}

			_type = Type::String;
		}

		void
		Variant::operator=(const char* chrPtr)
		{
			*this = std::string(chrPtr);
		}

		void
		Variant::operator=(const float4x4& val)
		{
			if (Type::Float4x4 == _type)
			{
				*_value.matrix = val;
			}
			else
			{
				this->destroy();
				_value.matrix = new float4x4(val);
			}
			_type = Type::Float4x4;
		}

		void
		Variant::operator=(const void* ptr)
		{
			this->destroy();
			_type = Type::Object;
			_value.object = ptr;
		}

		void
		Variant::operator=(const std::vector<int>& val)
		{
			if (Type::IntArray == _type)
			{
				*_value.intArray = val;
			}
			else
			{
				this->destroy();
				_value.intArray = new std::vector<int>(val);
			}
			_type = Type::IntArray;
		}

		void
		Variant::operator=(const std::vector<float>& val)
		{
			if (Type::FloatArray == _type)
			{
				*_value.floatArray = val;
			}
			else
			{
				this->destroy();
				_value.floatArray = new std::vector<float>(val);
			}
			_type = Type::FloatArray;
		}

		void
		Variant::operator=(const std::vector<bool>& val)
		{
			if (Type::BoolArray == _type)
			{
				*_value.boolArray = val;
			}
			else
			{
				this->destroy();
				_value.boolArray = new std::vector<bool>(val);
			}
			_type = Type::BoolArray;
		}

		void
		Variant::operator=(const std::vector<float4>& val)
		{
			if (Type::Float4Array == _type)
			{
				*_value.float4Array = val;
			}
			else
			{
				this->destroy();
				_value.float4Array = new std::vector<float4>(val);
			}

			_type = Type::Float4Array;
		}

		void
		Variant::operator=(const std::vector<float4x4>& val)
		{
			if (Type::Float4x4Array == _type)
			{
				*_value.float4x4Array = val;
			}
			else
			{
				this->destroy();
				_value.float4x4Array = new std::vector<float4x4>(val);
			}
			_type = Type::Float4x4Array;
		}

		void
		Variant::operator=(const std::vector<std::string>& val)
		{
			if (Type::StringArray == _type)
			{
				*_value.stringArray = val;
			}
			else
			{
				this->destroy();
				_value.stringArray = new std::vector<std::string>(val);
			}
			_type = Type::StringArray;
		}

		bool
		Variant::operator==(const Variant& rhs) const
		{
			if (rhs._type == _type)
			{
				switch (rhs._type)
				{
				case Type::Void:
					return true;
				case Type::Int:
					return (_value.i == rhs._value.i);
				case Type::Bool:
					return (_value.b == rhs._value.b);
				case Type::Float:
					return (_value.f[0] == rhs._value.f[0]);
				case Type::String:
					return ((*_value.string) == (*rhs._value.string));
				case Type::Float4:
					return ((_value.f[0] == rhs._value.f[0]) &&
						(_value.f[1] == rhs._value.f[1]) &&
						(_value.f[2] == rhs._value.f[2]) &&
						(_value.f[3] == rhs._value.f[3]));
				case Type::Object:
					return (_value.object == rhs._value.object);
				case Type::Float4x4:
					return _value.matrix == rhs._value.matrix;
				default:
					throw failure("Variant::operator==(): invalid variant type!");
				}
			}
			return false;
		}

		bool
		Variant::operator>(const Variant& rhs) const
		{
			if (rhs._type == _type)
			{
				switch (rhs._type)
				{
				case Type::Void:
					return true;
				case Type::Int:
					return (_value.i > rhs._value.i);
				case Type::Bool:
					return (_value.b > rhs._value.b);
				case Type::Float:
					return (_value.f[0] > rhs._value.f[0]);
				case Type::String:
					return ((*_value.string) > (*rhs._value.string));
				case Type::Float4:
					return ((_value.f[0] > rhs._value.f[0]) &&
						(_value.f[1] > rhs._value.f[1]) &&
						(_value.f[2] > rhs._value.f[2]) &&
						(_value.f[3] > rhs._value.f[3]));
				case Type::Object:
					return (_value.object > rhs._value.object);
				default:
					throw failure("Variant::operator>(): invalid variant type!");
				}
			}
			return false;
		}

		bool
		Variant::operator<(const Variant& rhs) const
		{
			if (rhs._type == _type)
			{
				switch (rhs._type)
				{
				case Type::Void:
					return true;
				case Type::Int:
					return (_value.i < rhs._value.i);
				case Type::Bool:
					return (_value.b < rhs._value.b);
				case Type::Float:
					return (_value.f[0] < rhs._value.f[0]);
				case Type::String:
					return ((*_value.string) < (*rhs._value.string));
				case Type::Float4:
					return ((_value.f[0] < rhs._value.f[0]) &&
						(_value.f[1] < rhs._value.f[1]) &&
						(_value.f[2] < rhs._value.f[2]) &&
						(_value.f[3] < rhs._value.f[3]));
				case Type::Object:
					return (_value.object < rhs._value.object);
				default:
					throw failure("Variant::operator<(): invalid variant type!");
				}
			}
			return false;
		}

		bool
		Variant::operator>=(const Variant& rhs) const
		{
			if (rhs._type == _type)
			{
				switch (rhs._type)
				{
				case Type::Void:
					return true;
				case Type::Int:
					return (_value.i >= rhs._value.i);
				case Type::Bool:
					return (_value.b >= rhs._value.b);
				case Type::Float:
					return (_value.f[0] >= rhs._value.f[0]);
				case Type::String:
					return ((*_value.string) >= (*rhs._value.string));
				case Type::Float4:
					return ((_value.f[0] >= rhs._value.f[0]) &&
						(_value.f[1] >= rhs._value.f[1]) &&
						(_value.f[2] >= rhs._value.f[2]) &&
						(_value.f[3] >= rhs._value.f[3]));
				case Type::Object:
					return (_value.object >= rhs._value.object);
				default:
					throw failure("Variant::operator>(): invalid variant type!");
				}
			}

			return false;
		}

		bool
		Variant::operator<=(const Variant& rhs) const
		{
			if (rhs._type == _type)
			{
				switch (rhs._type)
				{
				case Type::Void:
					return true;
				case Type::Int:
					return (_value.i <= rhs._value.i);
				case Type::Bool:
					return (_value.b <= rhs._value.b);
				case Type::Float:
					return (_value.f[0] <= rhs._value.f[0]);
				case Type::String:
					return ((*_value.string) <= (*rhs._value.string));
				case Type::Float4:
					return ((_value.f[0] <= rhs._value.f[0]) &&
						(_value.f[1] <= rhs._value.f[1]) &&
						(_value.f[2] <= rhs._value.f[2]) &&
						(_value.f[3] <= rhs._value.f[3]));
				case Type::Object:
					return (_value.object <= rhs._value.object);
				default:
					throw failure("Variant::operator<(): invalid variant type!");
				}
			}
			return false;
		}

		bool
		Variant::operator!=(const Variant& rhs) const
		{
			return !(*this == rhs);
		}

		bool
		Variant::operator==(int rhs) const
		{
			assert(Type::Int == _type);
			return (_value.i == rhs);
		}

		bool
		Variant::operator==(float rhs) const
		{
			assert(Type::Float == _type);
			return (_value.f[0] == rhs);
		}

		bool
		Variant::operator==(bool rhs) const
		{
			assert(Type::Bool == _type);
			return (_value.b == rhs);
		}

		bool
		Variant::operator==(const std::string& rhs) const
		{
			assert(Type::String == _type);
			return ((*_value.string) == rhs);
		}

		bool
		Variant::operator==(const char* chrPtr) const
		{
			return *this == std::string(chrPtr);
		}

		bool
		Variant::operator==(const float4& rhs) const
		{
			assert(Type::Float4 == _type);
			return ((_value.f[0] == rhs.x) && (_value.f[1] == rhs.y) && (_value.f[2] == rhs.z) && (_value.f[3] == rhs.w));
		}

		bool
		Variant::operator==(const void* ptr) const
		{
			assert(Type::Object == _type);
			return _value.object == ptr;
		}

		bool
		Variant::operator!=(int rhs) const
		{
			assert(Type::Int == _type);
			return (_value.i != rhs);
		}

		bool
		Variant::operator!=(float rhs) const
		{
			assert(Type::Float == _type);
			return (_value.f[0] != rhs);
		}

		bool
		Variant::operator!=(bool rhs) const
		{
			assert(Type::Bool == _type);
			return (_value.b != rhs);
		}

		bool
		Variant::operator!=(const std::string& rhs) const
		{
			assert(Type::String == _type);
			return (*_value.string) != rhs;
		}

		bool
		Variant::operator!=(const char* chrPtr) const
		{
			return *this != std::string(chrPtr);
		}

		bool
		Variant::operator!=(const float4& rhs) const
		{
			assert(Type::Float4 == _type);
			return ((_value.f[0] != rhs.x) || (_value.f[1] != rhs.y) || (_value.f[2] != rhs.z) || (_value.f[3] != rhs.w));
		}

		bool
		Variant::operator!=(const void* ptr) const
		{
			assert(Type::Object == _type);
			return (_value.object == ptr);
		}

		void
		Variant::assign(int val)
		{
			this->setInt(val);
		}

		void
		Variant::assign(float val)
		{
			this->setFloat(val);
		}

		void
		Variant::assign(bool val)
		{
			this->setBool(val);
		}

		void
		Variant::assign(const void* ptr)
		{
			this->setObject(ptr);
		}

		void
		Variant::assign(const float4& val)
		{
			this->setFloat4(val);
		}

		void
		Variant::assign(const float4x4& val)
		{
			this->setFloat4x4(val);
		}

		void
		Variant::assign(const std::string& val)
		{
			this->setString(val);
		}

		void
		Variant::assign(const std::vector<int>& val)
		{
			this->setIntArray(val);
		}

		void
		Variant::assign(const std::vector<float>& val)
		{
			this->setFloatArray(val);
		}

		void
		Variant::assign(const std::vector<bool>& val)
		{
			this->setBoolArray(val);
		}

		void
		Variant::assign(const std::vector<float4>& val)
		{
			this->setFloat4Array(val);
		}

		void
		Variant::assign(const std::vector<float4x4>& val)
		{
			this->setFloat4x4Array(val);
		}

		void
		Variant::assign(const std::vector<std::string>& val)
		{
			this->setStringArray(val);
		}

		void
		Variant::setInt(int val)
		{
			*this = val;
		}

		int
		Variant::getInt() const
		{
			assert(Type::Int == _type);
			return _value.i;
		}

		void
		Variant::setFloat(float val)
		{
			*this = val;
		}

		float
		Variant::getFloat() const
		{
			assert(Type::Float == _type);
			return _value.f[0];
		}

		void
		Variant::setBool(bool val)
		{
			*this = val;
		}

		bool
		Variant::getBool() const
		{
			assert(Type::Bool == _type);
			return _value.b;
		}

		void
		Variant::setString(const std::string& val)
		{
			*this = val;
		}

		const std::string&
		Variant::getString() const
		{
			assert(Type::String == _type);
			return *(_value.string);
		}

		void
		Variant::setFloat4(const float4& val)
		{
			*this = val;
		}

		const float4&
		Variant::getFloat4() const
		{
			assert(Type::Float4 == _type);
			return (float4&)_value.f;
		}

		void
		Variant::setFloat4x4(const float4x4& val)
		{
			*this = val;
		}

		const float4x4&
		Variant::getFloat4x4() const
		{
			assert(Type::Float4x4 == _type);
			return *(_value.matrix);
		}

		void
		Variant::setObject(const void* ptr)
		{
			*this = ptr;
		}

		const void*
		Variant::getObject() const
		{
			assert(Type::Object == _type);
			return _value.object;
		}

		void
		Variant::setIntArray(const std::vector<int>& val)
		{
			*this = val;
		}

		const std::vector<int>&
		Variant::getIntArray() const
		{
			assert(Type::IntArray == _type);
			return *(_value.intArray);
		}

		void
		Variant::setFloatArray(const std::vector<float>& val)
		{
			*this = val;
		}

		const std::vector<float>&
		Variant::getFloatArray() const
		{
			assert(Type::FloatArray == _type);
			return *(_value.floatArray);
		}

		void
		Variant::setBoolArray(const std::vector<bool>& val)
		{
			*this = val;
		}

		const std::vector<bool>&
		Variant::getBoolArray() const
		{
			assert(Type::BoolArray == _type);
			return *(_value.boolArray);
		}

		void
		Variant::setFloat4Array(const std::vector<float4>& val)
		{
			*this = val;
		}

		const std::vector<float4>&
		Variant::getFloat4Array() const
		{
			assert(Type::Float4Array == _type);
			return *(_value.float4Array);
		}

		void
		Variant::setFloat4x4Array(const std::vector<float4x4>& val)
		{
			*this = val;
		}

		const std::vector<float4x4>&
		Variant::getFloat4x4Array() const
		{
			assert(Type::Float4x4Array == _type);
			return *(_value.float4x4Array);
		}

		void
		Variant::setStringArray(const std::vector<std::string>& val)
		{
			*this = val;
		}

		const std::vector<std::string>&
		Variant::getStringArray() const
		{
			assert(Type::StringArray == _type);
			return *(_value.stringArray);
		}

		std::string
		Variant::typeToString(Type t)
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
				throw failure(tformat("Variant::typeToString(): invalid type enum '%d'!")) % t;
			}
		}

		Variant::Type
		Variant::stringToType(const std::string& str)
		{
			if ("void" == str)             return Type::Void;
			else if ("int" == str)              return Type::Int;
			else if ("float" == str)            return Type::Float;
			else if ("bool" == str)             return Type::Bool;
			else if ("float4" == str)           return Type::Float4;
			else if ("color" == str)            return Type::Float4; // NOT A BUG!
			else if ("string" == str)           return Type::String;
			else if ("Float4x4" == str)        return Type::Float4x4;
			else if ("blob" == str)             return Type::Blob;
			else if ("guid" == str)             return Type::Guid;
			else if ("object" == str)           return Type::Object;
			else if ("intarray" == str)         return Type::IntArray;
			else if ("floatarray" == str)       return Type::FloatArray;
			else if ("boolarray" == str)        return Type::BoolArray;
			else if ("float4array" == str)      return Type::Float4Array;
			else if ("Float4x4array" == str)   return Type::Float4x4Array;
			else if ("stringarray" == str)      return Type::StringArray;
			else
			{
				throw failure(tformat("Variant::stringToType(): invalid type string!"));
			}
		}

		void
		Variant::destroy()
		{
			if (Type::String == _type)
			{
				assert(_value.string);
				delete _value.string;
				_value.string = 0;
			}
			else if (Type::Float4x4 == _type)
			{
				assert(_value.matrix);
				delete _value.matrix;
				_value.matrix = 0;
			}
			else if (Type::Object == _type)
			{
				if (_value.object)
				{
					//_value.object->Release();
					_value.object = 0;
				}
			}
			else if (Type::IntArray == _type)
			{
				assert(_value.intArray);
				delete _value.intArray;
				_value.intArray = 0;
			}
			else if (Type::FloatArray == _type)
			{
				assert(_value.floatArray);
				delete _value.floatArray;
				_value.floatArray = 0;
			}
			else if (Type::BoolArray == _type)
			{
				assert(_value.boolArray);
				delete _value.boolArray;
				_value.boolArray = 0;
			}
			else if (Type::Float4Array == _type)
			{
				assert(_value.float4Array);
				delete _value.float4Array;
				_value.float4Array = 0;
			}
			else if (Type::Float4x4Array == _type)
			{
				assert(_value.float4x4Array);
				delete _value.float4x4Array;
				_value.float4x4Array = 0;
			}
			else if (Type::StringArray == _type)
			{
				assert(_value.stringArray);
				delete _value.stringArray;
				_value.stringArray = 0;
			}

			_type = Type::Void;
		}

		void
		Variant::clear()
		{
			this->destroy();
		}

		void
		Variant::copy(const Variant& rhs)
		{
			assert(Type::Void == _type);
			_type = Type::rhs._type;
			switch (rhs._type)
			{
			case Type::Void:
				break;
			case Type::Int:
				_value.i = rhs._value.i;
				break;
			case Type::Float:
				_value.f[0] = rhs._value.f[0];
				break;
			case Type::Bool:
				_value.b = rhs._value.b;
				break;
			case Type::Float4:
				_value.f[0] = rhs._value.f[0];
				_value.f[1] = rhs._value.f[1];
				_value.f[2] = rhs._value.f[2];
				_value.f[3] = rhs._value.f[3];
				break;
			case Type::String:
				_value.string = new std::string(*rhs._value.string);
				break;
			case Type::Float4x4:
				_value.matrix = new float4x4(*rhs._value.matrix);
				break;
			case Type::Object:
				_value.object = rhs._value.object;
				break;
			case Type::IntArray:
				_value.intArray = new std::vector<int>(*rhs._value.intArray);
				break;
			case Type::FloatArray:
				_value.floatArray = new std::vector<float>(*rhs._value.floatArray);
				break;
			case Type::BoolArray:
				_value.boolArray = new std::vector<bool>(*rhs._value.boolArray);
				break;
			case Type::Float4Array:
				_value.float4Array = new std::vector<float4>(*rhs._value.float4Array);
				break;
			case Type::Float4x4Array:
				_value.float4x4Array = new std::vector<float4x4>(*rhs._value.float4x4Array);
				break;
			case Type::StringArray:
				_value.stringArray = new std::vector<std::string>(*rhs._value.stringArray);
				break;
			default:
				throw failure("Variant::copy(): invalid type!");
				break;
			}
		}
	}
}