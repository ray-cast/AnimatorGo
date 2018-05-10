#ifndef OCTOON_PATH_MESHING_COMPONENT_H_
#define OCTOON_PATH_MESHING_COMPONENT_H_

#include <octoon/mesh_filter_component.h>
#include <octoon/video/render_types.h>
#include <octoon/image/image.h>
#include <octoon/model/mesh.h>
#include <octoon/model/contour_group.h>

struct PathMeshing
{
	struct Transform
	{
		octoon::math::float3 translate = octoon::math::float3::Zero;
		octoon::math::float3 scale = octoon::math::float3::One;
		octoon::math::float3 rotation = octoon::math::float3::Zero;
	};

	struct BoundingBox
	{
		octoon::math::AABB aabb = octoon::math::AABB::Empty;
		octoon::math::float3 center = octoon::math::float3::Zero;
	};

	struct Material
	{
		enum Type
		{
			Line = 0,
			Basic = 1,
			PhongShading = 2
		};

		Type type;

		bool hollow;
		octoon::math::float3 color;
		octoon::math::float1 thickness;
		std::uint16_t bezierSteps;

		struct LineMaterial
		{
			float lineWidth;
		};

		struct BasicMaterial
		{
		};

		struct PhongMaterial
		{
			float intensity;
			float ambient;
			float highlight;
			float highlightSize;

			octoon::math::float1 thickness;
			octoon::math::float3 darkcolor;
			octoon::math::float3 direction;
		};

		union
		{
			LineMaterial line;
			BasicMaterial basic;
			PhongMaterial phong;
		};
	};

	Transform transform;
	BoundingBox bound;
	Material material;
	octoon::model::Contours contours;

	PathMeshing()
	{
		material.type = Material::PhongShading;
		material.hollow = false;
		material.color = octoon::math::float3::Zero;
		material.thickness = 1.0f;
		material.bezierSteps = 6;

		material.phong.intensity = 1.0;
		material.phong.ambient = 0.6;
		material.phong.highlight = 0.0f;
		material.phong.highlightSize = 0.0f;
		material.phong.thickness = 1.0;
		material.phong.darkcolor = octoon::math::float3::Zero;
		material.phong.direction = octoon::math::float3::UnitY;
	}
};

class PathMeshingComponent : public octoon::GameComponent
{
	OctoonDeclareSubClass(PathMeshingComponent, octoon::GameComponent)
public:
	PathMeshingComponent() noexcept;
	PathMeshingComponent(std::string&& json) noexcept;
	PathMeshingComponent(const std::string& json) noexcept;
	virtual ~PathMeshingComponent() noexcept;

	void setBezierPath(std::string&& json) except;
	void setBezierPath(const std::string& json) except;
	const std::string& getBezierPath() const noexcept;

	virtual octoon::GameComponentPtr clone() const noexcept override;

protected:
	virtual void updateContour(const std::string& json) noexcept(false);
	virtual void updateMesh() noexcept;
	virtual void onSaveImage(octoon::image::Image& image, float x, float y) except;

private:
	virtual void onActivate() noexcept(false);
	virtual void onDeactivate() noexcept;

	virtual void onFrameEnd() except;

private:
	PathMeshingComponent(const PathMeshingComponent&) = delete;
	PathMeshingComponent& operator=(const PathMeshingComponent&) = delete;

private:
	std::string json_;

	PathMeshing params_;

	octoon::GameObjectPtr object_;
	octoon::GameObjectPtr camera_;
};

#endif