#ifndef OCTOON_VARIANT_H_
#define OCTOON_VARIANT_H_

#include <octoon/math/math.h>

namespace octoon
{
	namespace io
	{
		class OCTOON_EXPORT Variant
		{
		public:
			enum class Type
			{
				Void,
				Int,
				Float,
				Bool,
				Float4,
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
			Variant(float rhs);
			Variant(bool rhs);
			Variant(const math::float4& v);
			Variant(const math::float4x4& m);
			Variant(const std::string& rhs);
			Variant(const char* chrPtr);
			Variant(const void* ptr);
			Variant(const std::vector<int>& rhs);
			Variant(const std::vector<float>& rhs);
			Variant(const std::vector<bool>& rhs);
			Variant(const std::vector<math::float4>& rhs);
			Variant(const std::vector<math::float4x4>& rhs);
			Variant(const std::vector<std::string>& rhs);
			Variant(const Variant& rhs);

			~Variant();

			void setType(Type t);
			Type getType() const;
			void clear();

			void operator=(const Variant& rhs);
			void operator=(int val);
			void operator=(float val);
			void operator=(bool val);
			void operator=(const math::float4& val);
			void operator=(const math::float4x4& val);
			void operator=(const std::string& s);
			void operator=(const char* chrPtr);
			void operator=(const void* ptr);
			void operator=(const std::vector<int>& rhs);
			void operator=(const std::vector<float>& rhs);
			void operator=(const std::vector<bool>& rhs);
			void operator=(const std::vector<math::float4>& rhs);
			void operator=(const std::vector<math::float4x4>& rhs);
			void operator=(const std::vector<std::string>& rhs);

			bool operator==(const Variant& rhs) const;
			bool operator==(int rhs) const;
			bool operator==(float rhs) const;
			bool operator==(bool rhs) const;
			bool operator==(const math::float4& rhs) const;
			bool operator==(const std::string& rhs) const;
			bool operator==(const char* chrPtr) const;
			bool operator==(const void* ptr) const;

			bool operator!=(const Variant& rhs) const;
			bool operator!=(int rhs) const;
			bool operator!=(float rhs) const;
			bool operator!=(bool rhs) const;
			bool operator!=(const math::float4& rhs) const;
			bool operator!=(const std::string& rhs) const;
			bool operator!=(const char* chrPtr) const;
			bool operator!=(const void* ptr) const;

			bool operator>(const Variant& rhs) const;
			bool operator<(const Variant& rhs) const;
			bool operator>=(const Variant& rhs) const;
			bool operator<=(const Variant& rhs) const;

			void assign(int val);
			void assign(float val);
			void assign(bool val);
			void assign(const void* ptr);
			void assign(const math::float4& val);
			void assign(const math::float4x4& val);
			void assign(const std::string& val);
			void assign(const std::vector<int>& val);
			void assign(const std::vector<float>& val);
			void assign(const std::vector<bool>& val);
			void assign(const std::vector<math::float4>& val);
			void assign(const std::vector<math::float4x4>& val);
			void assign(const std::vector<std::string>& val);

			void setInt(int val);
			void setFloat(float val);
			void setBool(bool val);
			void setFloat4(const math::float4& val);
			void setFloat4x4(const math::float4x4& val);
			void setObject(const void* ptr);
			void setString(const std::string& val);
			void setIntArray(const std::vector<int>& val);
			void setFloatArray(const std::vector<float>& val);
			void setBoolArray(const std::vector<bool>& val);
			void setFloat4Array(const std::vector<math::float4>& val);
			void setFloat4x4Array(const std::vector<math::float4x4>& val);
			void setStringArray(const std::vector<std::string>& val);

			int getInt() const;
			float getFloat() const;
			bool getBool() const;
			const std::string& getString() const;
			const math::float4& getFloat4() const;
			const math::float4x4& getFloat4x4() const;
			const void* getObject() const;
			const std::vector<int>& getIntArray() const;
			const std::vector<float>& getFloatArray() const;
			const std::vector<bool>& getBoolArray() const;
			const std::vector<math::float4>& getFloat4Array() const;
			const std::vector<math::float4x4>& getFloat4x4Array() const;
			const std::vector<std::string>& getStringArray() const;

			static std::string typeToString(Type t);
			static Type stringToType(const std::string& str);

		private:
			void destroy();
			void copy(const Variant& rhs);

			Type _type;

			union
			{
				int i;
				bool b;
				float f[4];
				math::float4x4* matrix;
				const void* object;
				std::string* string;
				std::vector<int>* intArray;
				std::vector<float>* floatArray;
				std::vector<bool>* boolArray;
				std::vector<math::float4>* float4Array;
				std::vector<math::float4x4>* float4x4Array;
				std::vector<std::string>* stringArray;
			} _value;
		};
	}
}

#endif