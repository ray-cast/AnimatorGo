#ifndef OCTOON_VIDEO_FORWARD_BUFFER_H_
#define OCTOON_VIDEO_FORWARD_BUFFER_H_

#include <octoon/mesh/mesh.h>
#include <octoon/hal/graphics_data.h>

namespace octoon::video
{
	class OCTOON_EXPORT ForwardBuffer final
	{
	public:
		ForwardBuffer() noexcept;
		ForwardBuffer(const std::shared_ptr<mesh::Mesh>& mesh) noexcept(false);
		virtual ~ForwardBuffer() noexcept;

		void setMesh(const std::shared_ptr<mesh::Mesh>& mesh) noexcept(false);
		const std::shared_ptr<mesh::Mesh>& getMesh() const noexcept;

		std::size_t getNumVertices() const noexcept;
		std::size_t getNumIndices(std::size_t n) const noexcept;

		const hal::GraphicsDataPtr& getVertexBuffer() const noexcept;
		const hal::GraphicsDataPtr& getIndexBuffer(std::size_t n) const noexcept;

	private:
		void updateData(const std::shared_ptr<mesh::Mesh>& mesh) noexcept(false);

	private:
		ForwardBuffer(const ForwardBuffer&) = delete;
		ForwardBuffer& operator=(const ForwardBuffer&) = delete;

	private:
		std::uint32_t numVertices_;
		std::uint32_t numIndices_;

		hal::GraphicsDataPtr vertices_;
		std::vector<hal::GraphicsDataPtr> indices_;

		std::shared_ptr<mesh::Mesh> mesh_;
	};
}

#endif