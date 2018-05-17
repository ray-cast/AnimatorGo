#ifndef OCTOON_GRAPHICS_DESCRIPTOR_H_
#define OCTOON_GRAPHICS_DESCRIPTOR_H_

#include <octoon/graphics/graphics_child.h>

namespace octoon
{
	namespace graphics
	{
		class OCTOON_EXPORT GraphicsDescriptorPoolComponent final
		{
		public:
			GraphicsDescriptorPoolComponent() noexcept;
			GraphicsDescriptorPoolComponent(GraphicsUniformType type, std::uint32_t nums) noexcept;
			~GraphicsDescriptorPoolComponent() noexcept;

			void setDescriptorType(GraphicsUniformType type) noexcept;
			GraphicsUniformType getDescriptorType() const noexcept;

			void setDescriptorNums(std::uint32_t nums) noexcept;
			std::uint32_t getDescriptorNums() const noexcept;

		private:
			std::uint32_t _descriptorNums;
			GraphicsUniformType _type;
		};

		class OCTOON_EXPORT GraphicsDescriptorSetLayoutDesc final
		{
		public:
			GraphicsDescriptorSetLayoutDesc() noexcept = default;
			~GraphicsDescriptorSetLayoutDesc() = default;

			void addUniformComponent(GraphicsParamPtr component) noexcept;
			void removeUniformComponent(GraphicsParamPtr component) noexcept;

			void setUniformComponents(const GraphicsParams& component) noexcept;
			const GraphicsParams& getUniformComponents() const noexcept;

		private:
			GraphicsParams _params;
		};

		class OCTOON_EXPORT GraphicsDescriptorPoolDesc final
		{
		public:
			GraphicsDescriptorPoolDesc() noexcept;
			GraphicsDescriptorPoolDesc(std::uint32_t maxSets) noexcept;
			~GraphicsDescriptorPoolDesc() noexcept;

			void setMaxSets(std::uint32_t maxsets) noexcept;
			std::uint32_t getMaxSets() const noexcept;

			void addGraphicsDescriptorPoolComponent(const GraphicsDescriptorPoolComponent& desc) noexcept;
			const GraphicsDescriptorPoolComponents& getGraphicsDescriptorPoolComponents() const noexcept;

		private:
			std::uint32_t _maxSets;
			GraphicsDescriptorPoolComponents _poolSizes;
		};

		class OCTOON_EXPORT GraphicsDescriptorSetDesc final
		{
		public:
			GraphicsDescriptorSetDesc() noexcept;
			~GraphicsDescriptorSetDesc() noexcept;

			void setGraphicsDescriptorSetLayout(GraphicsDescriptorSetLayoutPtr layout) noexcept;
			GraphicsDescriptorSetLayoutPtr getDescriptorSetLayout() const noexcept;

			void setGraphicsDescriptorPool(GraphicsDescriptorPoolPtr pool) noexcept;
			GraphicsDescriptorPoolPtr getGraphicsDescriptorPool() const noexcept;

		private:
			GraphicsDescriptorPoolPtr _pool;
			GraphicsDescriptorSetLayoutPtr _layout;
		};

		class OCTOON_EXPORT GraphicsUniformSet : public runtime::RttiInterface
		{
			OctoonDeclareSubInterface(GraphicsUniformSet, runtime::RttiInterface)
		public:
			GraphicsUniformSet() noexcept = default;
			virtual ~GraphicsUniformSet() = default;

			virtual const std::string& getName() const noexcept = 0;

			virtual void uniform1b(bool value) noexcept = 0;
			virtual void uniform1i(std::int32_t i1) noexcept = 0;
			virtual void uniform2i(const int2& value) noexcept = 0;
			virtual void uniform2i(std::int32_t i1, std::int32_t i2) noexcept = 0;
			virtual void uniform3i(const int3& value) noexcept = 0;
			virtual void uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept = 0;
			virtual void uniform4i(const int4& value) noexcept = 0;
			virtual void uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept = 0;
			virtual void uniform1ui(std::uint32_t i1) noexcept = 0;
			virtual void uniform2ui(const uint2& value) noexcept = 0;
			virtual void uniform2ui(std::uint32_t i1, std::uint32_t i2) noexcept = 0;
			virtual void uniform3ui(const uint3& value) noexcept = 0;
			virtual void uniform3ui(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3) noexcept = 0;
			virtual void uniform4ui(const uint4& value) noexcept = 0;
			virtual void uniform4ui(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3, std::uint32_t i4) noexcept = 0;
			virtual void uniform1f(float i1) noexcept = 0;
			virtual void uniform2f(const float2& value) noexcept = 0;
			virtual void uniform2f(float i1, float i2) noexcept = 0;
			virtual void uniform3f(const float3& value) noexcept = 0;
			virtual void uniform3f(float i1, float i2, float i3) noexcept = 0;
			virtual void uniform4f(const float4& value) noexcept = 0;
			virtual void uniform4f(float i1, float i2, float i3, float i4) noexcept = 0;
			virtual void uniform2fmat(const float* mat2) noexcept = 0;
			virtual void uniform2fmat(const float2x2& value) noexcept = 0;
			virtual void uniform3fmat(const float* mat3) noexcept = 0;
			virtual void uniform3fmat(const float3x3& value) noexcept = 0;
			virtual void uniform4fmat(const float* mat4) noexcept = 0;
			virtual void uniform4fmat(const float4x4& value) noexcept = 0;
			virtual void uniform1iv(const std::vector<int1>& value) noexcept = 0;
			virtual void uniform1iv(std::size_t num, const std::int32_t* str) noexcept = 0;
			virtual void uniform2iv(const std::vector<int2>& value) noexcept = 0;
			virtual void uniform2iv(std::size_t num, const std::int32_t* str) noexcept = 0;
			virtual void uniform3iv(const std::vector<int3>& value) noexcept = 0;
			virtual void uniform3iv(std::size_t num, const std::int32_t* str) noexcept = 0;
			virtual void uniform4iv(const std::vector<int4>& value) noexcept = 0;
			virtual void uniform4iv(std::size_t num, const std::int32_t* str) noexcept = 0;
			virtual void uniform1uiv(const std::vector<uint1>& value) noexcept = 0;
			virtual void uniform1uiv(std::size_t num, const std::uint32_t* str) noexcept = 0;
			virtual void uniform2uiv(const std::vector<uint2>& value) noexcept = 0;
			virtual void uniform2uiv(std::size_t num, const std::uint32_t* str) noexcept = 0;
			virtual void uniform3uiv(const std::vector<uint3>& value) noexcept = 0;
			virtual void uniform3uiv(std::size_t num, const std::uint32_t* str) noexcept = 0;
			virtual void uniform4uiv(const std::vector<uint4>& value) noexcept = 0;
			virtual void uniform4uiv(std::size_t num, const std::uint32_t* str) noexcept = 0;
			virtual void uniform1fv(const std::vector<float1>& value) noexcept = 0;
			virtual void uniform1fv(std::size_t num, const float* str) noexcept = 0;
			virtual void uniform2fv(const std::vector<float2>& value) noexcept = 0;
			virtual void uniform2fv(std::size_t num, const float* str) noexcept = 0;
			virtual void uniform3fv(const std::vector<float3>& value) noexcept = 0;
			virtual void uniform3fv(std::size_t num, const float* str) noexcept = 0;
			virtual void uniform4fv(const std::vector<float4>& value) noexcept = 0;
			virtual void uniform4fv(std::size_t num, const float* str) noexcept = 0;
			virtual void uniform2fmatv(const std::vector<float2x2>& value) noexcept = 0;
			virtual void uniform2fmatv(std::size_t num, const float* mat2) noexcept = 0;
			virtual void uniform3fmatv(const std::vector<float3x3>& value) noexcept = 0;
			virtual void uniform3fmatv(std::size_t num, const float* mat3) noexcept = 0;
			virtual void uniform4fmatv(const std::vector<float4x4>& value) noexcept = 0;
			virtual void uniform4fmatv(std::size_t num, const float* mat4) noexcept = 0;
			virtual void uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler = nullptr) noexcept = 0;
			virtual void uniformBuffer(GraphicsDataPtr ubo) noexcept = 0;

			virtual bool getBool() const noexcept = 0;
			virtual int getInt() const noexcept = 0;
			virtual const int2& getInt2() const noexcept = 0;
			virtual const int3& getInt3() const noexcept = 0;
			virtual const int4& getInt4() const noexcept = 0;
			virtual uint1 getUInt() const noexcept = 0;
			virtual const uint2& getUInt2() const noexcept = 0;
			virtual const uint3& getUInt3() const noexcept = 0;
			virtual const uint4& getUInt4() const noexcept = 0;
			virtual float getFloat() const noexcept = 0;
			virtual const float2& getFloat2() const noexcept = 0;
			virtual const float3& getFloat3() const noexcept = 0;
			virtual const float4& getFloat4() const noexcept = 0;
			virtual const float2x2& getFloat2x2() const noexcept = 0;
			virtual const float3x3& getFloat3x3() const noexcept = 0;
			virtual const float4x4& getFloat4x4() const noexcept = 0;
			virtual const std::vector<int1>& getIntArray() const noexcept = 0;
			virtual const std::vector<int2>& getInt2Array() const noexcept = 0;
			virtual const std::vector<int3>& getInt3Array() const noexcept = 0;
			virtual const std::vector<int4>& getInt4Array() const noexcept = 0;
			virtual const std::vector<uint1>& getUIntArray() const noexcept = 0;
			virtual const std::vector<uint2>& getUInt2Array() const noexcept = 0;
			virtual const std::vector<uint3>& getUInt3Array() const noexcept = 0;
			virtual const std::vector<uint4>& getUInt4Array() const noexcept = 0;
			virtual const std::vector<float1>& getFloatArray() const noexcept = 0;
			virtual const std::vector<float2>& getFloat2Array() const noexcept = 0;
			virtual const std::vector<float3>& getFloat3Array() const noexcept = 0;
			virtual const std::vector<float4>& getFloat4Array() const noexcept = 0;
			virtual const std::vector<float2x2>& getFloat2x2Array() const noexcept = 0;
			virtual const std::vector<float3x3>& getFloat3x3Array() const noexcept = 0;
			virtual const std::vector<float4x4>& getFloat4x4Array() const noexcept = 0;
			virtual const GraphicsTexturePtr& getTexture() const noexcept = 0;
			virtual const GraphicsSamplerPtr& getTextureSampler() const noexcept = 0;
			virtual const GraphicsDataPtr& getBuffer() const noexcept = 0;

			virtual const GraphicsParamPtr& getGraphicsParam() const noexcept = 0;

		private:
			GraphicsUniformSet(const GraphicsUniformSet&) noexcept = delete;
			GraphicsUniformSet& operator=(const GraphicsUniformSet&) noexcept = delete;
		};

		class OCTOON_EXPORT GraphicsDescriptorPool : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsDescriptorPool, GraphicsChild)
		public:
			GraphicsDescriptorPool() noexcept = default;
			virtual ~GraphicsDescriptorPool() = default;

			virtual const GraphicsDescriptorPoolDesc& getGraphicsDescriptorPoolDesc() const noexcept = 0;

		private:
			GraphicsDescriptorPool(const GraphicsDescriptorPool&) = delete;
			GraphicsDescriptorPool& operator=(const GraphicsDescriptorPool&) = delete;
		};

		class OCTOON_EXPORT GraphicsDescriptorSetLayout : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsDescriptorSetLayout, GraphicsChild)
		public:
			GraphicsDescriptorSetLayout() noexcept = default;
			virtual ~GraphicsDescriptorSetLayout() = default;

			virtual const GraphicsDescriptorSetLayoutDesc& getGraphicsDescriptorSetLayoutDesc() const noexcept = 0;

		private:
			GraphicsDescriptorSetLayout(const GraphicsDescriptorSetLayout&) = delete;
			GraphicsDescriptorSetLayout& operator=(const GraphicsDescriptorSetLayout&) = delete;
		};

		class OCTOON_EXPORT GraphicsDescriptorSet : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsDescriptorSet, GraphicsChild)
		public:
			GraphicsDescriptorSet() noexcept = default;
			virtual ~GraphicsDescriptorSet() = default;

			virtual const GraphicsUniformSets& getGraphicsUniformSets() const noexcept = 0;
			virtual const GraphicsDescriptorSetDesc& getGraphicsDescriptorSetDesc() const noexcept = 0;

		private:
			GraphicsDescriptorSet(const GraphicsDescriptorSet&) = delete;
			GraphicsDescriptorSet& operator=(const GraphicsDescriptorSet&) = delete;
		};
	}
}

#endif