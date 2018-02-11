#ifndef OCTOON_OGL_DESCRIPTOR_H_
#define OCTOON_OGL_DESCRIPTOR_H_

#include "ogl_types.h"

namespace octoon
{
	namespace graphics
	{
		class OGLCoreDescriptorSet final : public GraphicsDescriptorSet
		{
			OctoonDeclareSubClass(OGLCoreDescriptorSet, OGLCoreGraphicsData)
		public:
			OGLCoreDescriptorSet() noexcept;
			~OGLCoreDescriptorSet() noexcept;

			bool setup(const GraphicsDescriptorSetDesc& desc) noexcept;
			void close() noexcept;

			void apply(const OGLProgram& shaderObject) noexcept;

			void copy(std::uint32_t descriptorCopyCount, const GraphicsDescriptorSetPtr descriptorCopies[]) noexcept;

			const GraphicsUniformSets& getGraphicsUniformSets() const noexcept;
			const GraphicsDescriptorSetDesc& getGraphicsDescriptorSetDesc() const noexcept;

		private:
			friend class OGLDevice;
			void setDevice(const GraphicsDevicePtr& device) noexcept;
			GraphicsDevicePtr getDevice() noexcept;

		private:
			OGLCoreDescriptorSet(const OGLCoreDescriptorSet&) noexcept = delete;
			OGLCoreDescriptorSet& operator=(const OGLCoreDescriptorSet&) noexcept = delete;

		private:
			GraphicsUniformSets _activeUniformSets;
			GraphicsDeviceWeakPtr _device;
			GraphicsDescriptorSetDesc _descriptorSetDesc;
		};
	}
}

#endif