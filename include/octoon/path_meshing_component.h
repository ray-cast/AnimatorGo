#ifndef TEXT3D_PATH_MESHING_H_
#define TEXT3D_PATH_MESHING_H_

#include <text3d/mesh_filter.h>
#include <text3d/renderer/render_types.h>

namespace text3d
{
	class TEXT3D_EXPORT PathMeshing final : public MeshFilter
	{
		Text3dDeclareSubClass(PathMeshing, MeshFilter)
	public:
		PathMeshing() noexcept;
		PathMeshing(std::string&& json, std::uint16_t bezierSteps = 8) noexcept;
		PathMeshing(const std::string& json, std::uint16_t bezierSteps = 8) noexcept;
		virtual ~PathMeshing() noexcept;

		void setBezierPath(std::string&& json) noexcept;
		void setBezierPath(const std::string& json) noexcept;
		const std::string& getBezierPath() const noexcept;

		void setBezierSteps(std::uint16_t bezierSteps) noexcept;
		std::uint16_t getBezierSteps() const noexcept;

		virtual EntityComponentPtr clone() const noexcept override;

	private:
		virtual void onActivate() noexcept(false);
		virtual void onDeactivate() noexcept;

	private:
		void updateContour(const std::string& json) noexcept(false);

	private:
		PathMeshing(const PathMeshing&) = delete;
		PathMeshing& operator=(const PathMeshing&) = delete;

	private:
		std::string json_;
		std::uint16_t bezierSteps_;
	};
}

#endif