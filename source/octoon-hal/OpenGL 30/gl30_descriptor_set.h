#ifndef OCTOON_GL30_DESCRIPTOR_H_
#define OCTOON_GL30_DESCRIPTOR_H_

#include "gl30_types.h"

namespace octoon
{
	namespace graphics
	{
		class GL30GraphicsUniformSet final : public GraphicsUniformSet
		{
			OctoonDeclareSubClass(GL30GraphicsUniformSet, GraphicsUniformSet)
		public:
			GL30GraphicsUniformSet() noexcept;
			virtual ~GL30GraphicsUniformSet() noexcept;

			const std::string& getName() const noexcept override;

			void uniform1b(bool value) noexcept override;
			void uniform1i(std::int32_t i1) noexcept override;
			void uniform2i(const int2& value) noexcept override;
			void uniform2i(std::int32_t i1, std::int32_t i2) noexcept override;
			void uniform3i(const int3& value) noexcept override;
			void uniform3i(std::int32_t i1, std::int32_t i2, std::int32_t i3) noexcept override;
			void uniform4i(const int4& value) noexcept override;
			void uniform4i(std::int32_t i1, std::int32_t i2, std::int32_t i3, std::int32_t i4) noexcept override;
			void uniform1ui(std::uint32_t i1) noexcept override;
			void uniform2ui(const uint2& value) noexcept override;
			void uniform2ui(std::uint32_t i1, std::uint32_t i2) noexcept override;
			void uniform3ui(const uint3& value) noexcept override;
			void uniform3ui(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3) noexcept override;
			void uniform4ui(const uint4& value) noexcept override;
			void uniform4ui(std::uint32_t i1, std::uint32_t i2, std::uint32_t i3, std::uint32_t i4) noexcept override;
			void uniform1f(float i1) noexcept override;
			void uniform2f(const float2& value) noexcept override;
			void uniform2f(float i1, float i2) noexcept override;
			void uniform3f(const float3& value) noexcept override;
			void uniform3f(float i1, float i2, float i3) noexcept override;
			void uniform4f(const float4& value) noexcept override;
			void uniform4f(float i1, float i2, float i3, float i4) noexcept override;
			void uniform2fmat(const float* mat2) noexcept override;
			void uniform2fmat(const float2x2& value) noexcept override;
			void uniform3fmat(const float* mat3) noexcept override;
			void uniform3fmat(const float3x3& value) noexcept override;
			void uniform4fmat(const float* mat4) noexcept override;
			void uniform4fmat(const float4x4& value) noexcept override;
			void uniform1iv(const std::vector<int1>& value) noexcept override;
			void uniform1iv(std::size_t num, const std::int32_t* str) noexcept override;
			void uniform2iv(const std::vector<int2>& value) noexcept override;
			void uniform2iv(std::size_t num, const std::int32_t* str) noexcept override;
			void uniform3iv(const std::vector<int3>& value) noexcept override;
			void uniform3iv(std::size_t num, const std::int32_t* str) noexcept override;
			void uniform4iv(const std::vector<int4>& value) noexcept override;
			void uniform4iv(std::size_t num, const std::int32_t* str) noexcept override;
			void uniform1uiv(const std::vector<uint1>& value) noexcept override;
			void uniform1uiv(std::size_t num, const std::uint32_t* str) noexcept override;
			void uniform2uiv(const std::vector<uint2>& value) noexcept override;
			void uniform2uiv(std::size_t num, const std::uint32_t* str) noexcept override;
			void uniform3uiv(const std::vector<uint3>& value) noexcept override;
			void uniform3uiv(std::size_t num, const std::uint32_t* str) noexcept override;
			void uniform4uiv(const std::vector<uint4>& value) noexcept override;
			void uniform4uiv(std::size_t num, const std::uint32_t* str) noexcept override;
			void uniform1fv(const std::vector<float1>& value) noexcept override;
			void uniform1fv(std::size_t num, const float* str) noexcept override;
			void uniform2fv(const std::vector<float2>& value) noexcept override;
			void uniform2fv(std::size_t num, const float* str) noexcept override;
			void uniform3fv(const std::vector<float3>& value) noexcept override;
			void uniform3fv(std::size_t num, const float* str) noexcept override;
			void uniform4fv(const std::vector<float4>& value) noexcept override;
			void uniform4fv(std::size_t num, const float* str) noexcept override;
			void uniform2fmatv(const std::vector<float2x2>& value) noexcept override;
			void uniform2fmatv(std::size_t num, const float* mat2) noexcept override;
			void uniform3fmatv(const std::vector<float3x3>& value) noexcept override;
			void uniform3fmatv(std::size_t num, const float* mat3) noexcept override;
			void uniform4fmatv(const std::vector<float4x4>& value) noexcept override;
			void uniform4fmatv(std::size_t num, const float* mat4) noexcept override;
			void uniformTexture(GraphicsTexturePtr texture, GraphicsSamplerPtr sampler) noexcept override;
			void uniformBuffer(GraphicsDataPtr ubo) noexcept override;

			bool getBool() const noexcept override;
			int getInt() const noexcept override;
			const int2& getInt2() const noexcept override;
			const int3& getInt3() const noexcept override;
			const int4& getInt4() const noexcept override;
			uint1 getUInt() const noexcept override;
			const uint2& getUInt2() const noexcept override;
			const uint3& getUInt3() const noexcept override;
			const uint4& getUInt4() const noexcept override;
			float getFloat() const noexcept override;
			const float2& getFloat2() const noexcept override;
			const float3& getFloat3() const noexcept override;
			const float4& getFloat4() const noexcept override;
			const float2x2& getFloat2x2() const noexcept override;
			const float3x3& getFloat3x3() const noexcept override;
			const float4x4& getFloat4x4() const noexcept override;
			const std::vector<int1>& getIntArray() const noexcept override;
			const std::vector<int2>& getInt2Array() const noexcept override;
			const std::vector<int3>& getInt3Array() const noexcept override;
			const std::vector<int4>& getInt4Array() const noexcept override;
			const std::vector<uint1>& getUIntArray() const noexcept override;
			const std::vector<uint2>& getUInt2Array() const noexcept override;
			const std::vector<uint3>& getUInt3Array() const noexcept override;
			const std::vector<uint4>& getUInt4Array() const noexcept override;
			const std::vector<float1>& getFloatArray() const noexcept override;
			const std::vector<float2>& getFloat2Array() const noexcept override;
			const std::vector<float3>& getFloat3Array() const noexcept override;
			const std::vector<float4>& getFloat4Array() const noexcept override;
			const std::vector<float2x2>& getFloat2x2Array() const noexcept override;
			const std::vector<float3x3>& getFloat3x3Array() const noexcept override;
			const std::vector<float4x4>& getFloat4x4Array() const noexcept override;
			const GraphicsTexturePtr& getTexture() const noexcept override;
			const GraphicsSamplerPtr& getTextureSampler() const noexcept override;
			const GraphicsDataPtr& getBuffer() const noexcept override;

			void setGraphicsParam(GraphicsParamPtr param) noexcept;
			const GraphicsParamPtr& getGraphicsParam() const noexcept override;

		private:
			GL30GraphicsUniformSet(const GL30GraphicsUniformSet&) = delete;
			GL30GraphicsUniformSet& operator=(const GL30GraphicsUniformSet&) = delete;

		private:
			GraphicsParamPtr _param;
			GraphicsVariant _variant;
		};

		class GL30DescriptorPool final : public GraphicsDescriptorPool
		{
			OctoonDeclareSubClass(GL30DescriptorPool, GraphicsDescriptorPool)
		public:
			GL30DescriptorPool() noexcept;
			~GL30DescriptorPool() noexcept;

			bool setup(const GraphicsDescriptorPoolDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsDescriptorPoolDesc& getDescriptorPoolDesc() const noexcept override;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL30DescriptorPool(const GL30DescriptorPool&) noexcept = delete;
			GL30DescriptorPool& operator=(const GL30DescriptorPool&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsDescriptorPoolDesc _descriptorPoolDesc;
		};

		class GL30DescriptorSetLayout final : public GraphicsDescriptorSetLayout
		{
			OctoonDeclareSubClass(GL30DescriptorSetLayout, GraphicsDescriptorSetLayout)
		public:
			GL30DescriptorSetLayout() noexcept;
			~GL30DescriptorSetLayout() noexcept;

			bool setup(const GraphicsDescriptorSetLayoutDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsDescriptorSetLayoutDesc& getDescriptorSetLayoutDesc() const noexcept override;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL30DescriptorSetLayout(const GL30DescriptorSetLayout&) noexcept = delete;
			GL30DescriptorSetLayout& operator=(const GL30DescriptorSetLayout&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsDescriptorSetLayoutDesc _descripotrSetLayoutDesc;
		};

		class GL30DescriptorSet final : public GraphicsDescriptorSet
		{
			OctoonDeclareSubClass(GL30DescriptorSet, GraphicsDescriptorSet)
		public:
			GL30DescriptorSet() noexcept;
			~GL30DescriptorSet() noexcept;

			bool setup(const GraphicsDescriptorSetDesc& desc) noexcept;
			void close() noexcept;

			void apply(const GL30Program& program) noexcept;

			void copy(std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept;

			const GraphicsUniformSets& getUniformSets() const noexcept override;
			const GraphicsDescriptorSetDesc& getDescriptorSetDesc() const noexcept override;

		private:
			friend class GL30Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL30DescriptorSet(const GL30DescriptorSet&) noexcept = delete;
			GL30DescriptorSet& operator=(const GL30DescriptorSet&) noexcept = delete;

		private:
			GraphicsUniformSets _activeUniformSets;
			GraphicsDeviceWeakPtr _device;
			GraphicsDescriptorSetDesc _descriptorSetDesc;
		};
	}
}

#endif