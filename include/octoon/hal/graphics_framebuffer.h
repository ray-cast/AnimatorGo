#ifndef OCTOON_GRAPHICS_FRAMEBUFFER_H_
#define OCTOON_GRAPHICS_FRAMEBUFFER_H_

#include <octoon/hal/graphics_texture.h>

namespace octoon
{
	class OCTOON_EXPORT GraphicsAttachmentLayout final
	{
	public:
		GraphicsAttachmentLayout() noexcept;
		GraphicsAttachmentLayout(std::uint32_t slot, GraphicsImageLayout type, GraphicsFormat format) noexcept;
		~GraphicsAttachmentLayout() noexcept;

		void setAttachSlot(std::uint32_t slot) noexcept;
		std::uint32_t getAttachSlot() const noexcept;

		void setAttachType(GraphicsImageLayout layout) noexcept;
		GraphicsImageLayout getAttachType() const noexcept;

		void setAttachFormat(GraphicsFormat format) noexcept;
		GraphicsFormat getAttachFormat() const noexcept;

	private:
		std::uint32_t _slot;
		GraphicsFormat _format;
		GraphicsImageLayout _type;
	};

	class OCTOON_EXPORT GraphicsAttachmentBinding final
	{
	public:
		GraphicsAttachmentBinding() noexcept;
		GraphicsAttachmentBinding(std::shared_ptr<GraphicsTexture>&& texture, std::uint32_t mipLevel = 0, std::uint32_t layer = 0) noexcept;
		GraphicsAttachmentBinding(const std::shared_ptr<GraphicsTexture>& texture, std::uint32_t mipLevel = 0, std::uint32_t layer = 0) noexcept;
		~GraphicsAttachmentBinding() noexcept;

		void setBindingLevel(std::uint32_t mipLevel) noexcept;
		std::uint32_t getBindingLevel() const noexcept;

		void setBindingLayer(std::uint32_t layer) noexcept;
		std::uint32_t getBindingLayer() const noexcept;

		void setBindingTexture(const std::shared_ptr<GraphicsTexture>& texture) noexcept;
		const std::shared_ptr<GraphicsTexture>& getBindingTexture() const noexcept;

	private:
		std::uint32_t _mipLevel;
		std::uint32_t _layer;
		std::shared_ptr<GraphicsTexture> _texture;
	};

	class OCTOON_EXPORT GraphicsFramebufferLayoutDesc final
	{
	public:
		GraphicsFramebufferLayoutDesc() noexcept;
		~GraphicsFramebufferLayoutDesc() noexcept;

		void addComponent(const GraphicsAttachmentLayout& component) noexcept;
		void setComponents(const GraphicsAttachmentLayouts& components) noexcept;
		const GraphicsAttachmentLayouts& getComponents() const noexcept;

	private:
		GraphicsAttachmentLayouts _components;
	};

	class OCTOON_EXPORT GraphicsFramebufferDesc final
	{
	public:
		GraphicsFramebufferDesc() noexcept;
		virtual ~GraphicsFramebufferDesc() noexcept;

		void setWidth(std::uint32_t w) noexcept;
		void setHeight(std::uint32_t h) noexcept;
		void setLayer(std::uint32_t layer) noexcept;

		std::uint32_t getWidth() const noexcept;
		std::uint32_t getHeight() const noexcept;
		std::uint32_t getLayer() const noexcept;

		void addColorAttachment(const GraphicsAttachmentBinding& texture) noexcept;
		void setColorAttachments(const GraphicsAttachmentBindings& bindings) noexcept;
		const GraphicsAttachmentBinding& getColorAttachment(std::uint32_t n = 0) const noexcept;
		const GraphicsAttachmentBindings& getColorAttachments() const noexcept;

		void setDepthStencilAttachment(const GraphicsAttachmentBinding& target) noexcept;
		const GraphicsAttachmentBinding& getDepthStencilAttachment() const noexcept;

		void setFramebufferLayout(GraphicsFramebufferLayoutPtr&& layout) noexcept;
		void setFramebufferLayout(const GraphicsFramebufferLayoutPtr& layout) noexcept;
		GraphicsFramebufferLayoutPtr getFramebufferLayout() const noexcept;

	private:
		std::uint32_t _width;
		std::uint32_t _height;
		std::uint32_t _layer;

		GraphicsAttachmentBindings _colorAttachments;
		GraphicsAttachmentBinding _depthDepthAttachment;
		GraphicsFramebufferLayoutPtr _framebufferLayout;
	};

	class OCTOON_EXPORT GraphicsFramebufferLayout : public GraphicsResource
	{
		OctoonDeclareSubInterface(GraphicsFramebufferLayout, GraphicsResource)
	public:
		GraphicsFramebufferLayout() noexcept = default;
		virtual ~GraphicsFramebufferLayout() = default;

		virtual const GraphicsFramebufferLayoutDesc& getFramebufferLayoutDesc() const noexcept = 0;

	private:
		GraphicsFramebufferLayout(GraphicsFramebufferLayout&) noexcept = delete;
		GraphicsFramebufferLayout& operator=(const GraphicsFramebufferLayout&) noexcept = delete;
	};

	class OCTOON_EXPORT GraphicsFramebuffer : public GraphicsResource
	{
		OctoonDeclareSubInterface(GraphicsFramebuffer, GraphicsResource)
	public:
		GraphicsFramebuffer() noexcept = default;
		virtual ~GraphicsFramebuffer() = default;

		virtual const std::uint64_t handle() const noexcept = 0;
		virtual const GraphicsFramebufferDesc& getFramebufferDesc() const noexcept = 0;

	private:
		GraphicsFramebuffer(GraphicsFramebuffer&) noexcept = delete;
		GraphicsFramebuffer& operator=(const GraphicsFramebuffer&) noexcept = delete;
	};
}

#endif