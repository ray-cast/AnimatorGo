#include "path_meshing_component.h"

#include <octoon/model/mesh.h>
#include <octoon/model/contour_group.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/mesh_renderer_component.h>
#include <octoon/transform_component.h>

#include <octoon/runtime/json.h>
#include <octoon/runtime/except.h>
#include <octoon/runtime/uuid.h>
#include <octoon/io/fcntl.h>

#include <octoon/video/text_material.h>
#include <octoon/video/line_material.h>
#include <octoon/video/phong_material.h>
#include <octoon/video/render_system.h>
#include <octoon/camera_component.h>
#include <octoon/image/image.h>
#include <octoon/image/image_util.h>

#include <cstring>
#include <chrono>
#include <sstream>
#include <fstream>

#define POD_TT_PRIM_NONE 0
#define POD_TT_PRIM_LINE 1   	// line to, 一个点［x,y]
#define POD_TT_PRIM_QSPLINE 2	// qudratic bezier to, 三个点［[controlX,controlY]，[endX,endY]］
#define POD_TT_PRIM_CSPLINE 3	// cubic bezier to, 三个点［[control1X,control1Y]，[control2X,control2Y]，[endX,endY]］
#define POD_TT_PRIM_ARCLINE 4	// arc to, 三个点［[startX,startY]，[endX,endY]，[startDegree,sweepAngle]］
#define POD_TT_PRIM_MOVE 8   	// move to, 一个点［x,y]
#define POD_TT_PRIM_CLOSE 9  	// close path, equal to fisrt point of path

#define MATERIAL_TYPE_LINE 0
#define MATERIAL_TYPE_BASIC 1
#define MATERIAL_TYPE_PHONGSHADING 2

OctoonImplementSubClass(PathMeshingComponent, octoon::GameComponent, "PathMeshingComponent")

using namespace octoon;
using namespace octoon::graphics;
using namespace octoon::video;

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
	: bezierSteps_(5)
{
}

PathMeshingComponent::PathMeshingComponent(std::string&& json, std::uint16_t bezierSteps, bool clockwise) noexcept
	: bezierSteps_(5)
{
	this->setBezierPath(json);
	this->setBezierSteps(bezierSteps);
}

PathMeshingComponent::PathMeshingComponent(const std::string& json, std::uint16_t bezierSteps, bool clockwise) noexcept
	: bezierSteps_(5)
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
	this->addComponentDispatch(octoon::GameDispatchType::FrameEnd);

	if (!json_.empty())
		this->updateContour(json_);
}

void
PathMeshingComponent::onDeactivate() noexcept
{
	this->removeComponentDispatch(octoon::GameDispatchType::FrameEnd);
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
	PathMeshing& params = params_;

	auto reader = json::parse(data);

	auto transform = reader["transform"];
	if (!transform.is_null())
	{
		params.transform.translate << transform["translate"];
		params.transform.scale << transform["scale"];
		params.transform.rotation << transform["rotation"];
	}

	auto& bound = reader["boundingBox"];
	if (!bound.is_null())
	{
		params.aabb.center << bound["center"];
		params.aabb.aabb.min << bound["min"];
		params.aabb.aabb.max << bound["max"];
	}

	auto& json_material = reader["material"];
	if (!json_material.is_null())
	{
		params.material.type = (PathMeshing::Material::Type)json_material["type"].get<json::number_integer_t>();
		params.material.color << reader["color"];
		params.material.hollow = json_material["hollow"].get<json::boolean_t>();
		params.material.thickness = json_material["thickness"].get<json::number_float_t>();

		switch (params.material.type)
		{
		case PathMeshing::Material::Line:
		{
			auto& lineSize = json_material["lineWidth"];
			if (!lineSize.is_null())
				params.material.line.lineWidth = lineSize.get<json::number_float_t>();
		}
		break;
		case PathMeshing::Material::Basic:
		case PathMeshing::Material::PhongShading:
		{
			auto& lights = json_material["lights"];
			if (lights.size() > 0)
			{
				auto& light = lights[0];

				params.material.phong.direction << light["direction"];
				params.material.phong.darkcolor << light["darkcolor"];
				params.material.phong.intensity = light["intensity"].get<json::number_float_t>();
				params.material.phong.ambient = light["ambient"].get<json::number_float_t>();
				params.material.phong.highLight = light["highLight"].get<json::number_float_t>();
				params.material.phong.highLightSize = light["highLightSize"].get<json::number_float_t>();
			}
		}
		default:
			break;
		}
	}

	auto& text = reader["text"];
	if (!text.is_null())
	{
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

						prev = cur;
					}
					break;
					default:
						throw runtime::runtime_error::create("invalid bezier type");
					}
				}

				contour->isClockwise(!params.material.hollow);
				contours.push_back(std::move(contour));
			}
		}

		math::float3 offset = (params.aabb.aabb.max - params.aabb.aabb.min) * params.aabb.center;

		for (auto& contour : contours)
		{
			for (auto& it : contour->points())
				it -= offset;
		}

		params.aabb.aabb.min.z = -params.material.thickness;
		params.aabb.aabb.max.z = params.material.thickness;
		params.aabb.aabb -= offset;
		params.aabb.aabb = math::transform(params.aabb.aabb, math::float4x4().makeRotation(math::Quaternion(math::radians(params.transform.rotation))));

		object_ = std::make_shared<octoon::GameObject>();
		object_->setParent(this->getGameObject());
		object_->getComponent<TransformComponent>()->setLocalTranslate(-params.aabb.aabb.min);
		object_->getComponent<TransformComponent>()->setLocalQuaternion(math::Quaternion(math::radians(params.transform.rotation)));
		object_->getComponent<TransformComponent>()->setLocalScale(params.transform.scale);

		if (params.material.type == PathMeshing::Material::Line)
			object_->addComponent<octoon::MeshFilterComponent>(model::makeMeshWireframe(contours, params.material.thickness));
		else
		{
			auto mesh = model::makeMesh(contours, params.material.thickness);
			mesh.computeVertexNormals();

			object_->addComponent<octoon::MeshFilterComponent>(std::move(mesh));
		}

		switch (params.material.type)
		{
		case PathMeshing::Material::Line:
		{
			auto material = std::make_shared<octoon::video::LineMaterial>(params.material.line.lineWidth);
			material->setColor(params.material.color);

			object_->addComponent<octoon::MeshRendererComponent>(std::move(material));
		}
		break;
		case PathMeshing::Material::Basic:
		{
			auto material = std::make_shared<octoon::video::TextMaterial>();
			material->setTextColor(octoon::video::TextColor::FrontColor, params.material.color);
			material->setTextColor(octoon::video::TextColor::SideColor, params.material.color);

			object_->addComponent<octoon::MeshRendererComponent>(std::move(material));
		}
		break;
		case PathMeshing::Material::PhongShading:
		{
			auto material = std::make_shared<octoon::video::PhongMaterial>();
			material->setBaseColor(params.material.color * params.material.phong.intensity);
			material->setAmbientColor(params.material.color * params.material.phong.ambient);
			material->setSpecularColor(math::float3::One * params.material.phong.highLight);
			material->setShininess(params.material.phong.highLightSize);
			material->setLightDir(math::normalize(params.material.phong.direction));
			material->setDarkColor(params.material.phong.darkcolor);

			object_->addComponent<octoon::MeshRendererComponent>(std::move(material));
		}
		break;
		}
	}

	camera_ = std::make_shared<octoon::GameObject>();

	auto cameraComponent = camera_->addComponent<octoon::CameraComponent>();
	cameraComponent->setCameraOrder(octoon::video::CameraOrder::Custom);
	cameraComponent->setCameraType(octoon::video::CameraType::Ortho);
	cameraComponent->setOrtho(octoon::math::float4(0.0, 1.0, 0.0, 1.0));
	cameraComponent->setClearColor(octoon::math::float4(1.0, 1.0, 1.0, 0.0));
	cameraComponent->setupFramebuffers(params.aabb.aabb.size().x, params.aabb.aabb.size().y, 4);
	cameraComponent->setupSwapFramebuffers(params.aabb.aabb.size().x, params.aabb.aabb.size().y);
}

void
PathMeshingComponent::onFrameEnd() except
{
	auto framebuffer = camera_->getComponent<octoon::CameraComponent>()->getSwapFramebuffer();
	if (framebuffer)
	{
		std::uint32_t w = framebuffer->getGraphicsFramebufferDesc().getWidth();
		std::uint32_t h = framebuffer->getGraphicsFramebufferDesc().getHeight();

		image::Image image(image::Format::R8G8B8A8UNorm, w, h);

		void* data = nullptr;
		auto texture = framebuffer->getGraphicsFramebufferDesc().getColorAttachments().at(0).getBindingTexture();
		if (texture->map(0, 0, w, h, 0, &data))
		{
			std::memcpy((void*)image.data(), data, image.size());
			texture->unmap();
		}

		auto now = std::chrono::system_clock::now();
		auto time = std::chrono::system_clock::to_time_t(now);

		std::ostringstream stream;
#if __WINDOWS__
		stream << "D:\\media\\images\\";
		stream << std::put_time(std::localtime(&time), "%Y\\%m\\%d\\");
#elif __LINUX__
		stream << "/media/images/";
		stream << std::put_time(std::localtime(&time), "%Y/%m/%d/");
#endif

		auto dir = stream.str();
		io::fcntl::mkdir(dir.c_str());

		char uuid[64] = {};
		runtime::GUID guid;
		runtime::guid_generate(guid);
		runtime::guid_to_string(guid, uuid);

		dir +=uuid;

		image.save(dir + ".png", "png");

		std::ofstream file(dir + ".json", std::ios_base::out | std::ios_base::binary);
		if (file)
		{
			std::ostringstream json;
			json << R"({"x":)" << params_.transform.translate.x + params_.aabb.aabb.center().x << ",";
			json << R"("y":)" << params_.transform.translate.y + params_.aabb.aabb.center().y << ",";
			json << R"("path":")" << dir << R"("})";

			file.write(json.str().c_str(), json.str().size());
		}
	}
}