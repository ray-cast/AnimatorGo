#ifndef OCTOON_GEOMETRY_H_
#define OCTOON_GEOMETRY_H_

#include <octoon/video/mesh.h>
#include <octoon/video/render_object.h>

namespace octoon
{
	namespace video
	{
		class OCTOON_EXPORT Geometry final : public RenderObject
		{
			OctoonDeclareSubClass(Geometry, RenderObject)
		public:
			Geometry() noexcept;
			~Geometry() noexcept;

			void setCastShadow(bool enable) noexcept;
			bool getCastShadow() const noexcept;

			void setReceiveShadow(bool enable) noexcept;
			bool getReceiveShadow() const noexcept;

			void setMesh(const MeshPtr& mesh) noexcept;
			const MeshPtr& getMesh() const noexcept;

			void setDrawType(DrawType type) noexcept;
			DrawType getDrawType() const noexcept;

		private:
			MeshPtr mesh_;

			bool isCastShadow_;
			bool isReceiveShadow_;

			std::intptr_t _vertexOffset;
			std::intptr_t _indexOffset;

			DrawType drawType_;
			GraphicsIndexType indexType_;
		};
	}
}

#endif