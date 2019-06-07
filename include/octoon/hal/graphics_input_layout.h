#ifndef OCTOON_GRAPHICS_INPUT_LAYOUT_H_
#define OCTOON_GRAPHICS_INPUT_LAYOUT_H_

#include <octoon/hal/graphics_child.h>

namespace octoon
{
	namespace hal
	{
		class OCTOON_EXPORT GraphicsVertexBinding final
		{
		public:
			GraphicsVertexBinding() noexcept;
			GraphicsVertexBinding(std::uint8_t slot, std::uint16_t size) noexcept;
			GraphicsVertexBinding(std::uint8_t slot, std::uint16_t size, GraphicsVertexDivisor divisor) noexcept;
			~GraphicsVertexBinding() noexcept;

			void setVertexSlot(std::uint8_t slot) noexcept;
			std::uint8_t getVertexSlot() const noexcept;

			void setVertexSize(std::uint16_t size) noexcept;
			std::uint16_t getVertexSize() const noexcept;

			void setVertexDivisor(GraphicsVertexDivisor divisor) noexcept;
			GraphicsVertexDivisor getVertexDivisor() const noexcept;

		public:
			std::uint8_t _slot;
			std::uint16_t _size;
			GraphicsVertexDivisor _divisor;
		};

		class OCTOON_EXPORT GraphicsVertexLayout final
		{
		public:
			GraphicsVertexLayout() noexcept;
			GraphicsVertexLayout(std::uint8_t slot, std::string&& semantic, std::uint8_t semanticIndex, GraphicsFormat format, std::uint16_t offset = 0) noexcept;
			GraphicsVertexLayout(std::uint8_t slot, const std::string& semantic, std::uint8_t semanticIndex, GraphicsFormat format, std::uint16_t offset = 0) noexcept;
			~GraphicsVertexLayout() noexcept;

			void setSemantic(const std::string& semantic) noexcept;
			const std::string& getSemantic() const noexcept;

			void setSemanticIndex(std::uint8_t index) noexcept;
			std::uint8_t getSemanticIndex() const noexcept;

			void setVertexSlot(std::uint8_t slot) noexcept;
			std::uint8_t getVertexSlot() const noexcept;

			void setVertexOffset(std::uint16_t offset) noexcept;
			std::uint16_t getVertexOffset() const noexcept;

			void setVertexFormat(GraphicsFormat format) noexcept;
			GraphicsFormat getVertexFormat() const noexcept;

			std::uint8_t getVertexCount() const noexcept;
			std::uint8_t getVertexSize() const noexcept;

			static std::uint8_t getVertexCount(GraphicsFormat format) noexcept;
			static std::uint8_t getVertexSize(GraphicsFormat format) noexcept;

		private:
			std::string _name;
			std::uint8_t _index;
			std::uint8_t _count;
			std::uint8_t _size;
			std::uint8_t _slot;
			std::uint16_t _offset;
			GraphicsFormat _format;
		};

		class OCTOON_EXPORT GraphicsInputLayoutDesc final
		{
		public:
			GraphicsInputLayoutDesc() noexcept = default;
			~GraphicsInputLayoutDesc() = default;

			void addVertexLayout(const GraphicsVertexLayout& layout) noexcept;
			void addVertexBinding(const GraphicsVertexBinding& binding) noexcept;

			void setVertexLayouts(const GraphicsVertexLayouts& layouts) noexcept;
			const GraphicsVertexLayouts& getVertexLayouts() const noexcept;

			void setVertexBindings(const GraphicsVertexBindings& binding) noexcept;
			const GraphicsVertexBindings& getVertexBindings() const noexcept;

			std::uint16_t getVertexSize(std::uint8_t slot = 0) const noexcept;

		private:
			GraphicsVertexLayouts _layouts;
			GraphicsVertexBindings _bindings;
		};

		class OCTOON_EXPORT GraphicsInputLayout : public GraphicsChild
		{
			OctoonDeclareSubInterface(GraphicsInputLayout, GraphicsChild)
		public:
			GraphicsInputLayout() noexcept = default;
			virtual ~GraphicsInputLayout() = default;

			virtual const GraphicsInputLayoutDesc& getInputLayoutDesc() const noexcept = 0;

		private:
			GraphicsInputLayout(const GraphicsInputLayout&) = delete;
			GraphicsInputLayout& operator=(const GraphicsInputLayout&) = delete;
		};
	}
}

#endif