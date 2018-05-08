#ifndef OCTOON_PATH_MESHING_COMPONENT_H_
#define OCTOON_PATH_MESHING_COMPONENT_H_

#include <octoon/mesh_filter_component.h>
#include <octoon/video/render_types.h>
#include <octoon/image/image.h>

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
			float highLight;
			float highLightSize;

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
	BoundingBox aabb;
	Material material;

	PathMeshing()
	{
		material.type = Material::PhongShading;
		material.hollow = false;
		material.color = octoon::math::float3::Zero;
		material.thickness = 1.0f;

		material.phong.intensity = 1.0;
		material.phong.ambient = 0.6;
		material.phong.highLight = 0.0f;
		material.phong.highLightSize = 0.0f;
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
	PathMeshingComponent(std::string&& json, std::uint16_t bezierSteps = 6) noexcept;
	PathMeshingComponent(const std::string& json, std::uint16_t bezierSteps = 6) noexcept;
	virtual ~PathMeshingComponent() noexcept;

	void setBezierPath(std::string&& json) noexcept;
	void setBezierPath(const std::string& json) noexcept;
	const std::string& getBezierPath() const noexcept;

	void setBezierSteps(std::uint16_t bezierSteps) noexcept;
	std::uint16_t getBezierSteps() const noexcept;

	virtual octoon::GameComponentPtr clone() const noexcept override;

protected:
	virtual void onSaveImage(octoon::image::Image& image, std::uint32_t x, std::uint32_t y) except;

private:
	virtual void onActivate() noexcept(false);
	virtual void onDeactivate() noexcept;

	virtual void onFrameEnd() except;

private:
	void updateContour(const std::string& json) noexcept(false);

private:
	PathMeshingComponent(const PathMeshingComponent&) = delete;
	PathMeshingComponent& operator=(const PathMeshingComponent&) = delete;

private:
	std::string json_;
	std::uint16_t bezierSteps_;
	PathMeshing params_;
	octoon::GameObjectPtr object_;
	octoon::GameObjectPtr camera_;
};

#endif