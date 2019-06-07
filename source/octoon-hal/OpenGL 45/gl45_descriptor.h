#ifndef OCTOON_GL33_DESCRIPTOR_H_
#define OCTOON_GL33_DESCRIPTOR_H_

#include "gl33_types.h"

namespace octoon
{
	namespace hal
	{
		class GL45DescriptorSet final : public GraphicsDescriptorSet
		{
			OctoonDeclareSubClass(GL45DescriptorSet, GL45GraphicsData)
		public:
			GL45DescriptorSet() noexcept;
			~GL45DescriptorSet() noexcept;

			bool setup(const GraphicsDescriptorSetDesc& desc) noexcept;
			void close() noexcept;

			void apply(const GL33Program& shaderObject) noexcept;

			void copy(std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept;

			const GraphicsUniformSets& getUniformSets() const noexcept;
			const GraphicsDescriptorSetDesc& getDescriptorSetDesc() const noexcept override;

		private:
			friend class GL33Device;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept override;

		private:
			GL45DescriptorSet(const GL45DescriptorSet&) noexcept = delete;
			GL45DescriptorSet& operator=(const GL45DescriptorSet&) noexcept = delete;

		private:
			GraphicsUniformSets _activeUniformSets;
			GraphicsDeviceWeakPtr _device;
			GraphicsDescriptorSetDesc _descriptorSetDesc;
		};
	}
}

#endif