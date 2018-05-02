#include "path_meshing_component.h"

#include <octoon/model/mesh.h>
#include <octoon/model/contour_group.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/transform_component.h>
#include <octoon/runtime/json.h>
#include <octoon/runtime/except.h>
#include <octoon/video/text_material.h>
#include <octoon/video/line_material.h>
#include <octoon/video/phong_material.h>

#define POD_TT_PRIM_NONE 0
#define POD_TT_PRIM_LINE 1   	// line to, 一个点［x,y]
#define POD_TT_PRIM_QSPLINE 2	// qudratic bezier to, 三个点［[controlX,controlY]，[endX,endY]］
#define POD_TT_PRIM_CSPLINE 3	// cubic bezier to, 三个点［[control1X,control1Y]，[control2X,control2Y]，[endX,endY]］
#define POD_TT_PRIM_ARCLINE 4	// arc to, 三个点［[startX,startY]，[endX,endY]，[startDegree,sweepAngle]］
#define POD_TT_PRIM_MOVE 8   	// move to, 一个点［x,y]
#define POD_TT_PRIM_CLOSE 9  	// close path, equal to fisrt point of path

OctoonImplementSubClass(PathMeshingComponent, octoon::GameComponent, "PathMeshingComponent")

using namespace octoon;

math::float3& operator << (math::float3& v, json::reference& json)
{
	auto size = std::min<std::size_t>(3, json.size());
	for (std::uint8_t n = 0; n < size; n++)
		v[n] = json[n].get<json::number_float_t>();
	return v;
}

math::float4& operator << (math::float4& v, json::reference& json)
{
	auto size = std::min<std::size_t>(4, json.size());
	for (std::uint8_t n = 0; n < size; n++)
		v[n] = json[n].get<json::number_float_t>();
	return v;
}

PathMeshingComponent::PathMeshingComponent() noexcept
	: bezierSteps_(6)
{
}

PathMeshingComponent::PathMeshingComponent(std::string&& json, std::uint16_t bezierSteps, bool clockwise) noexcept
	: bezierSteps_(6)
{
	this->setBezierPath(json);
	this->setBezierSteps(bezierSteps);
}

PathMeshingComponent::PathMeshingComponent(const std::string& json, std::uint16_t bezierSteps, bool clockwise) noexcept
	: bezierSteps_(6)
{
	this->setBezierPath(json);
	this->setBezierSteps(bezierSteps);
}

PathMeshingComponent::~PathMeshingComponent() noexcept
{
}

void
PathMeshingComponent::setBezierPath(std::string&& json) noexcept
{
	json_ = std::move(json);

	if (this->getActive())
	{
		auto object = this->getGameObject();
		if (object && object->getActive())
			this->updateContour(json_);
	}
}

void
PathMeshingComponent::setBezierPath(const std::string& json) noexcept
{
	json_ = json;

	if (this->getActive())
	{
		auto object = this->getGameObject();
		if (object && object->getActive())
			this->updateContour(json_);
	}
}

const std::string&
PathMeshingComponent::getBezierPath() const noexcept
{
	return json_;
}

void
PathMeshingComponent::setBezierSteps(std::uint16_t bezierSteps) noexcept
{
	assert(bezierSteps > 0);

	if (bezierSteps_ != bezierSteps)
	{
		bezierSteps_ = bezierSteps;

		if (this->getActive())
		{
			auto object = this->getGameObject();
			if (object && object->getActive())
				this->updateContour(json_);
		}
	}
}

std::uint16_t
PathMeshingComponent::getBezierSteps() const noexcept
{
	return bezierSteps_;
}

void
PathMeshingComponent::onActivate() noexcept(false)
{
	if (!json_.empty())
		this->updateContour(json_);
}

void
PathMeshingComponent::onDeactivate() noexcept
{
}

GameComponentPtr
PathMeshingComponent::clone() const noexcept
{
	auto instance = std::make_shared<PathMeshingComponent>();
	instance->setName(this->getName());
	instance->setBezierPath(this->getBezierPath());
	instance->setBezierSteps(this->getBezierSteps());

	return instance;
}

void
PathMeshingComponent::updateContour(const std::string& data) noexcept(false)
{
	auto reader = json::parse(data);

	auto transform = reader["transform"];
	if (!transform.is_null())
	{
		math::float3 translate = math::float3::Zero;
		math::float3 scale = math::float3::One;
		math::float3 rotation = math::float3::Zero;

		translate << transform["translate"];
		scale << transform["scale"];
		rotation << transform["rotation"];

		auto transformComponent = this->getComponent<TransformComponent>();
		transformComponent->setTranslate(translate);
		transformComponent->setScale(scale);
		transformComponent->setQuaternion(math::Quaternion(math::radians(rotation)));
	}

	bool hollow = reader["material"]["hollow"].get<json::boolean_t>();
	bool wireframe = reader["material"]["wireframe"].get<json::boolean_t>();

	auto& text = reader["text"];
	if (!text.is_null())
	{
		math::float3 min = math::float3::Zero;
		math::float3 max = math::float3::Zero;
		math::float3 center = math::float3::Zero;

		center << text["center"];
		min << text["min"];
		max << text["max"];

		model::Contours contours;

		for (auto& group : text["chars"])
		{
			for (auto& path : group["paths"])
			{
				json::pointer prev = nullptr;
				json::pointer cur = nullptr;
				json::reference points = path["points"];

				auto contour = std::make_unique<model::Contour>();

				for (std::size_t index = 0; index < points.size(); index++)
				{
					cur = &points[index];

					switch (cur->at(2).get<json::number_unsigned_t>())
					{
					case POD_TT_PRIM_LINE:
					{
						contour->addPoints(math::float3(cur->at(0).get<json::number_float_t>(), cur->at(1).get<json::number_float_t>(), 0));
						prev = cur;
					}
					break;
					case POD_TT_PRIM_QSPLINE:
					{
						auto& p1 = *prev;
						auto& p2 = *cur;
						auto& p3 = points[index + 1];

						math::float3 A(p1[0].get<json::number_float_t>(), p1[1].get<json::number_float_t>(), 0.0);
						math::float3 B(p2[0].get<json::number_float_t>(), p2[1].get<json::number_float_t>(), 0.0);
						math::float3 C(p3[0].get<json::number_float_t>(), p3[1].get<json::number_float_t>(), 0.0);

						contour->addPoints(A, B, C, bezierSteps_);

						prev = &p3;

						index++;
					}
					break;
					case POD_TT_PRIM_CSPLINE:
					{
						auto& p1 = *prev;
						auto& p2 = *cur;
						auto& p3 = points[index + 1];
						auto& p4 = points[index + 2];

						math::float3 A(p1[0].get<json::number_float_t>(), p1[1].get<json::number_float_t>(), 0.0);
						math::float3 B(p2[0].get<json::number_float_t>(), p2[1].get<json::number_float_t>(), 0.0);
						math::float3 C(p3[0].get<json::number_float_t>(), p3[1].get<json::number_float_t>(), 0.0);
						math::float3 D(p4[0].get<json::number_float_t>(), p4[1].get<json::number_float_t>(), 0.0);

						contour->addPoints(A, B, C, D, bezierSteps_);

						prev = &p4;

						index += 2;
					}
					break;
					case POD_TT_PRIM_MOVE:
					{
						contour->addPoints(math::float3(cur->at(0).get<json::number_float_t>(), cur->at(1).get<json::number_float_t>(), 0));
						prev = cur;
					}
					break;
					case POD_TT_PRIM_CLOSE:
					{
						contour->addPoints(math::float3(cur->at(0).get<json::number_float_t>(), cur->at(1).get<json::number_float_t>(), 0));
						contour->addPoints(contour->at(0));

						prev = cur;
					}
					break;
					default:
						throw runtime::runtime_error::create("invalid bezier type");
					}
				}

				contour->isClockwise(hollow);
				contours.push_back(std::move(contour));
			}
		}

		math::float3 offset = (max - min) * center;

		for (auto& contour : contours)
		{
			for (auto& it : contour->points())
				it -= offset;
		}

		if (wireframe)
		{
			object_ = std::make_shared<octoon::GameObject>();
			object_->setParent(this->getGameObject());
			object_->getComponent<TransformComponent>()->setLocalTranslate(offset);
			object_->addComponent<octoon::MeshFilterComponent>(model::makeMeshWireframe(octoon::model::ContourGroup(std::move(contours))));
		}
		else
		{
			auto mesh = model::makeMesh(octoon::model::ContourGroup(std::move(contours)));
			mesh.computeVertexNormals();
			mesh.computeBoundingBox();

			object_ = std::make_shared<octoon::GameObject>();
			object_->setParent(this->getGameObject());
			object_->getComponent<TransformComponent>()->setLocalTranslate(offset);
			object_->addComponent<octoon::MeshFilterComponent>(std::move(mesh));
		}
	}

	auto& json_material = reader["material"];
	if (!json_material.is_null())
	{
		math::float3 baseColor = math::float3::Zero;
		baseColor << json_material["color"];

		if (wireframe)
		{
			auto material = std::make_shared<octoon::video::LineMaterial>();
			material->setColor(baseColor);

			if (object_)
				object_->addComponent<octoon::MeshRendererComponent>(std::move(material));
		}
		else
		{
			auto& lights = json_material["lights"];
			if (lights.size() > 0)
			{
				auto& light = lights[0];

				math::float3 direction = math::float3::UnitY;
				math::float3 color = math::float3::Zero;

				direction << light["direction"];
				color << light["darkcolor"];

				float intensity = light["intensity"].get<json::number_float_t>();
				float ambient = light["ambient"].get<json::number_float_t>();
				float power = light["power"].get<json::number_float_t>();
				float size = light["size"].get<json::number_float_t>();

				auto material = std::make_shared<octoon::video::PhongMaterial>();
				material->setBaseColor(baseColor * intensity);
				material->setAmbientColor(baseColor * ambient);
				material->setSpecularColor(math::float3::One * power);
				material->setShininess(size);
				material->setLightDir(math::normalize(direction));
				material->setDarkColor(color);

				if (object_)
					object_->addComponent<octoon::MeshRendererComponent>(std::move(material));
			}
			else
			{
				auto material = std::make_shared<octoon::video::TextMaterial>();
				material->setTextColor(octoon::video::TextColor::FrontColor, baseColor);
				material->setTextColor(octoon::video::TextColor::SideColor, baseColor);

				if (object_)
					object_->addComponent<octoon::MeshRendererComponent>(std::move(material));
			}
		}
	}
}