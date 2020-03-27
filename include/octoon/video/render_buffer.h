#ifndef OCTOON_RENDER_BUFFER_H_
#define OCTOON_RENDER_BUFFER_H_

#include <octoon/mesh/mesh.h>
#include <octoon/hal/graphics_data.h>

namespace octoon::video
{
	class OCTOON_EXPORT RenderBuffer final
	{
	public:
		RenderBuffer() noexcept;
		RenderBuffer(const mesh::MeshPtr& mesh) noexcept(false);
		virtual ~RenderBuffer() noexcept;

		void setMesh(const mesh::MeshPtr& mesh) noexcept(false);
		const mesh::MeshPtr& getMesh() const noexcept;

		std::size_t getNumVertices() const noexcept;
		std::size_t getNumIndices(std::size_t n) const noexcept;

		const hal::GraphicsDataPtr& getVertexBuffer() const noexcept;
		const hal::GraphicsDataPtr& getIndexBuffer(std::size_t n) const noexcept;

	private:
		void updateData(const mesh::MeshPtr& mesh) noexcept(false);

	private:
		RenderBuffer(const RenderBuffer&) = delete;
		RenderBuffer& operator=(const RenderBuffer&) = delete;

	private:
		std::uint32_t numVertices_;
		std::uint32_t numIndices_;

		hal::GraphicsDataPtr vertices_;
		std::vector<hal::GraphicsDataPtr> indices_;

		std::shared_ptr<mesh::Mesh> mesh_;
	};
}

#endif