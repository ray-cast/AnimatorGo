#ifndef OCTOON_GL32_DESCRIPTOR_H_
#define OCTOON_GL32_DESCRIPTOR_H_

#include "gl32_types.h"

namespace octoon
{
	namespace hal
	{
		class GL32GraphicsUniformSet final : public GraphicsUniformSet
		{
			OctoonDeclareSubClass(GL32GraphicsUniformSet, GraphicsUniformSet)
		public:
			GL32GraphicsUniformSet() noexcept;
			virtual ~GL32GraphicsUniformSet() noexcept;

			const std::string& getName() const noexcept;

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

			void setGraphicsParam(GraphicsParamPtr param) noexcept;
			const GraphicsParamPtr& getGraphicsParam() const noexcept override;

		private:
			GL32GraphicsUniformSet(const GL32GraphicsUniformSet&) = delete;
			GL32GraphicsUniformSet& operator=(const GL32GraphicsUniformSet&) = delete;

		private:
			GraphicsParamPtr _param;
			UniformHolder _variant;
		};

		class GL32DescriptorPool final : public GraphicsDescriptorPool
		{
			OctoonDeclareSubClass(GL32DescriptorPool, GraphicsDescriptorPool)
		public:
			GL32DescriptorPool() noexcept;
			~GL32DescriptorPool() noexcept;

			bool setup(const GraphicsDescriptorPoolDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsDescriptorPoolDesc& getDescriptorPoolDesc() const noexcept override;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL32DescriptorPool(const GL32DescriptorPool&) noexcept = delete;
			GL32DescriptorPool& operator=(const GL32DescriptorPool&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsDescriptorPoolDesc _descriptorPoolDesc;
		};

		class GL32DescriptorSetLayout final : public GraphicsDescriptorSetLayout
		{
			OctoonDeclareSubClass(GL32DescriptorSetLayout, GraphicsDescriptorSetLayout)
		public:
			GL32DescriptorSetLayout() noexcept;
			~GL32DescriptorSetLayout() noexcept;

			bool setup(const GraphicsDescriptorSetLayoutDesc& desc) noexcept;
			void close() noexcept;

			const GraphicsDescriptorSetLayoutDesc& getDescriptorSetLayoutDesc() const noexcept override;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL32DescriptorSetLayout(const GL32DescriptorSetLayout&) noexcept = delete;
			GL32DescriptorSetLayout& operator=(const GL32DescriptorSetLayout&) noexcept = delete;

		private:
			GraphicsDeviceWeakPtr _device;
			GraphicsDescriptorSetLayoutDesc _descripotrSetLayoutDesc;
		};

		class GL32DescriptorSet final : public GraphicsDescriptorSet
		{
			OctoonDeclareSubClass(GL32DescriptorSet, GraphicsDescriptorSet)
		public:
			GL32DescriptorSet() noexcept;
			~GL32DescriptorSet() noexcept;

			bool setup(const GraphicsDescriptorSetDesc& desc) noexcept;
			void close() noexcept;

			void apply(const GL32Program& program) noexcept;

			void copy(std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept;

			const GraphicsUniformSets& getUniformSets() const noexcept;
			const GraphicsDescriptorSetDesc& getDescriptorSetDesc() const noexcept override;

		private:
			friend class GL32Device;
			void setDevice(GraphicsDevicePtr device) noexcept;
			GraphicsDevicePtr getDevice() const noexcept override;

		private:
			GL32DescriptorSet(const GL32DescriptorSet&) noexcept = delete;
			GL32DescriptorSet& operator=(const GL32DescriptorSet&) noexcept = delete;

		private:
			GraphicsUniformSets _activeUniformSets;
			GraphicsDeviceWeakPtr _device;
			GraphicsDescriptorSetDesc _descriptorSetDesc;
		};
	}
}

#endif