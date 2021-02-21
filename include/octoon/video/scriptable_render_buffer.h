#ifndef OCTOON_VIDEO_FORWARD_BUFFER_H_
#define OCTOON_VIDEO_FORWARD_BUFFER_H_

#include <octoon/mesh/mesh.h>
#include <octoon/hal/graphics_data.h>
#include <octoon/video/scriptable_render_context.h>

namespace octoon
{
	class OCTOON_EXPORT ScriptableRenderBuffer final
	{
	public:
		ScriptableRenderBuffer() noexcept;
		ScriptableRenderBuffer(ScriptableRenderContext& context, const std::shared_ptr<Mesh>& mesh) noexcept(false);
		virtual ~ScriptableRenderBuffer() noexcept;

		std::size_t getNumVertices() const noexcept;
		std::size_t getNumIndices(std::size_t n) const noexcept;
		std::size_t getStartIndices(std::size_t n) const noexcept;

		const hal::GraphicsDataPtr& getVertexBuffer() const noexcept;
		const hal::GraphicsDataPtr& getIndexBuffer() const noexcept;

	private:
		void updateData(ScriptableRenderContext& context, const std::shared_ptr<Mesh>& mesh) noexcept(false);

	private:
		ScriptableRenderBuffer(const ScriptableRenderBuffer&) = delete;
		ScriptableRenderBuffer& operator=(const ScriptableRenderBuffer&) = delete;

	private:
		std::uint32_t numVertices_;
		std::uint32_t numIndices_;
		std::vector<std::size_t> startIndice_;

		hal::GraphicsDataPtr vertices_;
		hal::GraphicsDataPtr indices_;

		std::shared_ptr<Mesh> mesh_;
	};
}

#endif