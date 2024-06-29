#ifndef OCTOON_SCRIPTABLE_RENDER_BUFFER_H_
#define OCTOON_SCRIPTABLE_RENDER_BUFFER_H_

#include <octoon/mesh/mesh.h>
#include <octoon/hal/graphics_data.h>
#include <octoon/video/scriptable_render_context.h>

namespace octoon
{
	class OCTOON_EXPORT ScriptableRenderBuffer final
	{
	public:
		ScriptableRenderBuffer() noexcept;
		ScriptableRenderBuffer(const GraphicsDevicePtr& context, const std::shared_ptr<Mesh>& mesh) noexcept(false);
		virtual ~ScriptableRenderBuffer() noexcept;

		std::size_t getNumVertices() const noexcept;
		std::size_t getNumSubsets() const noexcept;
		std::size_t getNumIndicesAll() const noexcept;
		std::size_t getNumIndices(std::size_t n) const noexcept;
		std::size_t getStartIndices(std::size_t n) const noexcept;

		const GraphicsDataPtr& getVertexBuffer() const noexcept;
		const GraphicsDataPtr& getIndexBuffer() const noexcept;

		void updateData(const GraphicsDevicePtr& context, const std::shared_ptr<Mesh>& mesh) noexcept(false);

	private:
		ScriptableRenderBuffer(const ScriptableRenderBuffer&) = delete;
		ScriptableRenderBuffer& operator=(const ScriptableRenderBuffer&) = delete;

	private:
		std::uint32_t numVertices_;
		std::uint32_t numIndices_;
		std::uint32_t numSubsets_;

		std::vector<std::size_t> startIndice_;

		GraphicsDataPtr vertices_;
		GraphicsDataPtr indices_;

		std::shared_ptr<Mesh> mesh_;
	};
}

#endif