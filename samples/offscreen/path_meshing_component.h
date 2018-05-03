#ifndef OCTOON_PATH_MESHING_COMPONENT_H_
#define OCTOON_PATH_MESHING_COMPONENT_H_

#include <octoon/mesh_filter_component.h>
#include <octoon/video/render_types.h>

class PathMeshingComponent final : public octoon::GameComponent
{
	OctoonDeclareSubClass(PathMeshingComponent, octoon::GameComponent)
public:
	PathMeshingComponent() noexcept;
	PathMeshingComponent(std::string&& json, std::uint16_t bezierSteps = 8, bool clockwise = true) noexcept;
	PathMeshingComponent(const std::string& json, std::uint16_t bezierSteps = 8, bool clockwise = true) noexcept;
	virtual ~PathMeshingComponent() noexcept;

	void setBezierPath(std::string&& json) noexcept;
	void setBezierPath(const std::string& json) noexcept;
	const std::string& getBezierPath() const noexcept;

	void setBezierSteps(std::uint16_t bezierSteps) noexcept;
	std::uint16_t getBezierSteps() const noexcept;

	virtual octoon::GameComponentPtr clone() const noexcept override;

private:
	virtual void onActivate() noexcept(false);
	virtual void onDeactivate() noexcept;

private:
	void updateContour(const std::string& json) noexcept(false);

private:
	PathMeshingComponent(const PathMeshingComponent&) = delete;
	PathMeshingComponent& operator=(const PathMeshingComponent&) = delete;

private:
	std::string json_;
	std::uint16_t bezierSteps_;

	octoon::GameObjectPtr object_;
};

#endif