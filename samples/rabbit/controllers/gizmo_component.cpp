#include "gizmo_component.h"
#include "rabbit_behaviour.h"

namespace rabbit
{
	class GizmoMaterial final
	{
	public:
		GizmoMaterial(const octoon::math::float3& color, const octoon::math::float3& highlight)
		{
			octoon::hal::GraphicsColorBlend blend;
			blend.setBlendEnable(true);
			blend.setBlendSrc(octoon::hal::GraphicsBlendFactor::SrcAlpha);
			blend.setBlendDest(octoon::hal::GraphicsBlendFactor::OneMinusSrcAlpha);

			std::vector<octoon::hal::GraphicsColorBlend> blends;
			blends.push_back(blend);

			this->material_ = std::make_shared<octoon::material::MeshBasicMaterial>();
			this->material_->setColor(color);
			this->material_->setDepthEnable(false);
			this->material_->setDepthWriteEnable(false);
			this->material_->setCullMode(octoon::hal::GraphicsCullMode::None);
			this->material_->setColorBlends(std::move(blends));
			this->material_->setOpacity(0.7f);

			this->color_ = color;
			this->opacity_ = this->material_->getOpacity();
			this->highlightColor = highlight;
		}

		void highlight(bool highlighted)
		{
			if (highlighted)
			{
				this->material_->setColor(this->highlightColor);
				this->material_->setOpacity(1.0f);
			}
			else
			{
				this->material_->setColor(this->color_);
				this->material_->setOpacity(this->opacity_);
			}
		}

		operator std::shared_ptr<octoon::material::Material>()
		{
			return this->material_;
		}

	private:
		float opacity_;
		octoon::math::float3 color_;
		octoon::math::float3 highlightColor;
		std::shared_ptr<octoon::material::MeshBasicMaterial> material_;
	};

	class TransformGizmo
	{
	public:
		void init()
		{
			auto planeGeometry = octoon::mesh::PlaneMesh::create(50, 50, 2, 2);

			auto material = std::make_shared<octoon::material::MeshBasicMaterial>();
			material->setCullMode(octoon::hal::GraphicsCullMode::None);

			auto planeObject = octoon::GameObject::create();
			planeObject->addComponent<octoon::MeshFilterComponent>(planeGeometry);
			planeObject->addComponent<octoon::MeshRendererComponent>(material);

			this->planes = {
				{ "XY", planeObject },
				{ "YZ", planeObject },
				{ "XZ", planeObject },
				{ "XYZE", planeObject }
			};

			this->activePlane = this->planes["XYZE"];

			planes["YZ"]->getComponent<octoon::TransformComponent>()->setEulerAngles(octoon::math::float3(0, octoon::math::PI / 2, 0));
			planes["XZ"]->getComponent<octoon::TransformComponent>()->setEulerAngles(octoon::math::float3(-octoon::math::PI / 2, 0, 0));

			for (auto plane : planes) {
				plane.second->setName(plane.first);
			}
		}

		octoon::GameObjectPtr activePlane;
		std::map<std::string, octoon::GameObjectPtr> planes;
		std::map<std::string, octoon::GameObjectPtr> handleGizmos;
		std::map<std::string, octoon::GameObjectPtr> pickerGizmos;
	};

	class TransformGizmoTranslate : public TransformGizmo
	{
	public:
		TransformGizmoTranslate()
		{
			octoon::math::float3s arrowVertices = {
				octoon::math::float3( 0.5f, 1.f, 0),
				octoon::math::float3(  0.f, 2.f, 0),
				octoon::math::float3(-0.5f, 1.f, 0),
				octoon::math::float3( 0.25f, 0.f, 0),
				octoon::math::float3( 0.25f, 1.f, 0),
				octoon::math::float3(-0.25f, 1.f, 0),
				octoon::math::float3( 0.25f, 0.f, 0),
				octoon::math::float3(-0.25f, 1.f, 0),
				octoon::math::float3(-0.25f, 0.f, 0),
			};

			for (auto& vertices : arrowVertices) {
				vertices *= 0.35f;
			}

			auto lineXGeometry = std::make_shared<octoon::mesh::Mesh>();
			auto lineYGeometry = std::make_shared<octoon::mesh::Mesh>();
			auto lineZGeometry = std::make_shared<octoon::mesh::Mesh>();

			lineXGeometry->setVertexArray({ octoon::math::float3(0,0,0), octoon::math::float3(1,0,0) });
			lineXGeometry->setVertexArray({ octoon::math::float3(0,0,0), octoon::math::float3(0,1,0) });
			lineXGeometry->setVertexArray({ octoon::math::float3(0,0,0), octoon::math::float3(0,0,1) });

			auto cylinderGizmos = octoon::ConeHelper::create(0.03, 1.0, 4, 1, false);
			auto cylinderGeometry = octoon::ConeHelper::create(0.2, 1.0, 4, 1, false);

			auto arrowXGeometry = std::make_shared<octoon::mesh::Mesh>();
			arrowXGeometry->setVertexArray(arrowVertices);
			arrowXGeometry->mergeMeshes(*lineXGeometry);

			auto arrowYGeometry = std::make_shared<octoon::mesh::Mesh>();
			arrowYGeometry->setVertexArray(arrowVertices);
			arrowYGeometry->mergeMeshes(*lineYGeometry);

			auto arrowZGeometry = std::make_shared<octoon::mesh::Mesh>();
			arrowZGeometry->setVertexArray(arrowVertices);
			arrowZGeometry->mergeMeshes(*lineZGeometry);

			auto gizmoXMaterial = std::make_shared<GizmoMaterial>(octoon::math::float3(255, 162, 115), octoon::math::float3(255, 255, 0));
			auto gizmoYMaterial = std::make_shared<GizmoMaterial>(octoon::math::float3(156, 239, 108), octoon::math::float3(255, 255, 0));
			auto gizmoZMaterial = std::make_shared<GizmoMaterial>(octoon::math::float3(104, 156, 210), octoon::math::float3(255, 255, 0));

			auto handleX = octoon::GameObject::create();
			handleX->addComponent<octoon::MeshFilterComponent>(arrowXGeometry);
			handleX->addComponent<octoon::MeshRendererComponent>(*gizmoXMaterial);
			
			auto handleY = octoon::GameObject::create();
			handleY->addComponent<octoon::MeshFilterComponent>(arrowYGeometry);
			handleY->addComponent<octoon::MeshRendererComponent>(*gizmoYMaterial);

			auto handleZ = octoon::GameObject::create();
			handleZ->addComponent<octoon::MeshFilterComponent>(arrowZGeometry);
			handleZ->addComponent<octoon::MeshRendererComponent>(*gizmoZMaterial);

			octoon::math::float3s rotateVertices = {
				octoon::math::float3(0.5f, 1.f, 0),
				octoon::math::float3(0.f, 0.f, 0),
				octoon::math::float3(-0.5f, 1.f, 0),
			};

			for (auto& vertices : rotateVertices) {
				vertices *= 0.25f;
			}

			auto rotateGeometry = std::make_shared<octoon::mesh::Mesh>();
			rotateGeometry->setVertexArray(rotateVertices);

			this->handleGizmos = {
				{ "X", handleX },
				{ "Y", handleY },
				{ "Z", handleZ }
			};

			this->pickerGizmos = {
				{ "X", handleX },
				{ "Y", handleY },
				{ "Z", handleZ }
			};

			this->init();
		}

		void setActivePlane(const std::string& axis, const octoon::math::float3& eye)
		{
			auto q = this->planes["XY"]->getComponent<octoon::TransformComponent>()->getQuaternion();
			auto qinv = octoon::math::inverse(q);
			auto localEye = octoon::math::rotate(qinv, eye);

			if (axis == "X") {
				this->activePlane = this->planes["XY"];
				if (std::abs(localEye.y) > std::abs(localEye.z)) this->activePlane = this->planes["XZ"];
			}
			else if (axis == "Y") {
				this->activePlane = this->planes["XY"];
				if (std::abs(localEye.x) > std::abs(localEye.z)) this->activePlane = this->planes["YZ"];
			}
			else if (axis == "Z") {
				this->activePlane = this->planes["XZ"];
				if (std::abs(localEye.x) > std::abs(localEye.y)) this->activePlane = this->planes["YZ"];
			}
			else if (axis == "RY") {
				this->activePlane = this->planes["XZ"];
			}
		}

	private:
		std::shared_ptr<octoon::material::MeshBasicMaterial> pickerMaterial_;
	};

	GizmoComponent::GizmoComponent() noexcept
	{
	}

	GizmoComponent::~GizmoComponent() noexcept
	{
	}
}