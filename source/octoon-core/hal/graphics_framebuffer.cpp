#include <octoon/hal/graphics_framebuffer.h>

namespace octoon
{
	namespace hal
	{
		OctoonImplementSubInterface(GraphicsFramebuffer, GraphicsChild, "GraphicsFramebuffer")
		OctoonImplementSubInterface(GraphicsFramebufferLayout, GraphicsChild, "GraphicsFramebufferLayout")

		GraphicsAttachmentLayout::GraphicsAttachmentLayout() noexcept
			: _type(GraphicsImageLayout::Undefined)
			, _slot(0)
			, _format(GraphicsFormat::Undefined)
		{
		}

		GraphicsAttachmentLayout::GraphicsAttachmentLayout(std::uint32_t slot, GraphicsImageLayout type, GraphicsFormat format) noexcept
			: _type(type)
			, _slot(slot)
			, _format(format)
		{
		}

		GraphicsAttachmentLayout::~GraphicsAttachmentLayout() noexcept
		{
		}

		void
		GraphicsAttachmentLayout::setAttachSlot(std::uint32_t slot) noexcept
		{
			_slot = slot;
		}

		std::uint32_t
		GraphicsAttachmentLayout::getAttachSlot() const noexcept
		{
			return _slot;
		}

		void
		GraphicsAttachmentLayout::setAttachType(GraphicsImageLayout type) noexcept
		{
			_type = type;
		}

		GraphicsImageLayout
		GraphicsAttachmentLayout::getAttachType() const noexcept
		{
			return _type;
		}

		void
		GraphicsAttachmentLayout::setAttachFormat(GraphicsFormat format) noexcept
		{
			_format = format;
		}

		GraphicsFormat
		GraphicsAttachmentLayout::getAttachFormat() const noexcept
		{
			return _format;
		}

		GraphicsAttachmentBinding::GraphicsAttachmentBinding() noexcept
			: _mipLevel(0)
			, _layer(0)
		{
		}

		GraphicsAttachmentBinding::GraphicsAttachmentBinding(GraphicsTexturePtr&& texture, std::uint32_t mipLevel, std::uint32_t layer) noexcept
			: _mipLevel(mipLevel)
			, _layer(layer)
			, _texture(std::move(texture))
		{
		}

		GraphicsAttachmentBinding::GraphicsAttachmentBinding(const GraphicsTexturePtr& texture, std::uint32_t mipLevel, std::uint32_t layer) noexcept
			: _mipLevel(mipLevel)
			, _layer(layer)
			, _texture(texture)
		{
		}

		GraphicsAttachmentBinding::~GraphicsAttachmentBinding() noexcept
		{
		}

		void
		GraphicsAttachmentBinding::setBindingLevel(std::uint32_t mipLevel) noexcept
		{
			_mipLevel = mipLevel;
		}

		std::uint32_t
		GraphicsAttachmentBinding::getBindingLevel() const noexcept
		{
			return _mipLevel;
		}

		void
		GraphicsAttachmentBinding::setBindingLayer(std::uint32_t layer) noexcept
		{
			_layer = layer;
		}

		std::uint32_t
		GraphicsAttachmentBinding::getBindingLayer() const noexcept
		{
			return _layer;
		}

		void
		GraphicsAttachmentBinding::setBindingTexture(GraphicsTexturePtr texture) noexcept
		{
			_texture = texture;
		}

		GraphicsTexturePtr
		GraphicsAttachmentBinding::getBindingTexture() const noexcept
		{
			return _texture;
		}

		GraphicsFramebufferLayoutDesc::GraphicsFramebufferLayoutDesc() noexcept
		{
		}

		GraphicsFramebufferLayoutDesc::~GraphicsFramebufferLayoutDesc() noexcept
		{
		}

		void
		GraphicsFramebufferLayoutDesc::addComponent(const GraphicsAttachmentLayout& component) noexcept
		{
			assert(component.getAttachFormat() != GraphicsFormat::Undefined);
			assert(std::find_if(_components.begin(), _components.end(), [&](const GraphicsAttachmentLayout& it) { return  it.getAttachSlot() == component.getAttachSlot(); }) == _components.end());
			_components.push_back(component);
		}

		void
		GraphicsFramebufferLayoutDesc::setComponents(const GraphicsAttachmentLayouts& components) noexcept
		{
			_components = components;
		}

		const GraphicsAttachmentLayouts&
		GraphicsFramebufferLayoutDesc::getComponents() const noexcept
		{
			return _components;
		}

		GraphicsFramebufferDesc::GraphicsFramebufferDesc() noexcept
			: _width(0)
			, _height(0)
			, _layer(1)
		{
		}

		GraphicsFramebufferDesc::~GraphicsFramebufferDesc() noexcept
		{
		}

		void
		GraphicsFramebufferDesc::setWidth(std::uint32_t w) noexcept
		{
			_width = w;
		}

		void
		GraphicsFramebufferDesc::setHeight(std::uint32_t h) noexcept
		{
			_height = h;
		}

		void
		GraphicsFramebufferDesc::setLayer(std::uint32_t layer) noexcept
		{
			_layer = layer;
		}

		std::uint32_t
		GraphicsFramebufferDesc::getWidth() const noexcept
		{
			return _width;
		}

		std::uint32_t
		GraphicsFramebufferDesc::getHeight() const noexcept
		{
			return _height;
		}

		std::uint32_t
		GraphicsFramebufferDesc::getLayer() const noexcept
		{
			return _layer;
		}

		void
		GraphicsFramebufferDesc::addColorAttachment(const GraphicsAttachmentBinding& texture) noexcept
		{
			assert(texture.getBindingTexture());
			_colorAttachments.push_back(texture);
		}

		void
		GraphicsFramebufferDesc::setColorAttachments(const GraphicsAttachmentBindings& bindings) noexcept
		{
			_colorAttachments = bindings;
		}

		const GraphicsAttachmentBinding&
		GraphicsFramebufferDesc::getColorAttachment(std::uint32_t n) const noexcept
		{
			assert(n < _colorAttachments.size());
			return _colorAttachments[n];
		}

		const GraphicsAttachmentBindings&
		GraphicsFramebufferDesc::getColorAttachments() const noexcept
		{
			return _colorAttachments;
		}

		void
		GraphicsFramebufferDesc::setDepthStencilAttachment(const GraphicsAttachmentBinding& attachment) noexcept
		{
			_depthDepthAttachment = attachment;
		}

		const GraphicsAttachmentBinding&
		GraphicsFramebufferDesc::getDepthStencilAttachment() const noexcept
		{
			return _depthDepthAttachment;
		}

		void
		GraphicsFramebufferDesc::setFramebufferLayout(GraphicsFramebufferLayoutPtr&& layout) noexcept
		{
			_framebufferLayout = std::move(layout);
		}

		void
		GraphicsFramebufferDesc::setFramebufferLayout(const GraphicsFramebufferLayoutPtr& layout) noexcept
		{
			_framebufferLayout = layout;
		}

		GraphicsFramebufferLayoutPtr
		GraphicsFramebufferDesc::getFramebufferLayout() const noexcept
		{
			return _framebufferLayout;
		}
	}
}