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

			this->color_ = octoon::math::srgb2linear(color);
			this->opacity_ = 0.7f;
			this->highlightColor = octoon::math::srgb2linear(highlight);

			this->material_ = std::make_shared<octoon::material::MeshBasicMaterial>();
			this->material_->setColor(this->color_);
			this->material_->setDepthEnable(false);
			this->material_->setDepthWriteEnable(false);
			this->material_->setCullMode(octoon::hal::GraphicsCullMode::None);
			this->material_->setColorBlends(std::move(blends));
			this->material_->setOpacity(this->opacity_);
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

	class GizmoLineMaterial final
	{
	public:
		GizmoLineMaterial(const octoon::math::float3& color, const octoon::math::float3& highlight)
		{
			octoon::hal::GraphicsColorBlend blend;
			blend.setBlendEnable(true);
			blend.setBlendSrc(octoon::hal::GraphicsBlendFactor::SrcAlpha);
			blend.setBlendDest(octoon::hal::GraphicsBlendFactor::OneMinusSrcAlpha);

			std::vector<octoon::hal::GraphicsColorBlend> blends;
			blends.push_back(blend);

			this->material_ = std::make_shared<octoon::material::LineBasicMaterial>();
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
		std::shared_ptr<octoon::material::LineBasicMaterial> material_;
	};

	void
	TransformGizmo::init()
	{
		auto planeGeometry = octoon::mesh::PlaneMesh::create(50, 50, 2, 2);

		auto material = std::make_shared<octoon::material::MeshBasicMaterial>();
		material->setCullMode(octoon::hal::GraphicsCullMode::None);

		auto XY = octoon::GameObject::create();
		XY->addComponent<octoon::MeshFilterComponent>(planeGeometry);
		XY->addComponent<octoon::MeshRendererComponent>(material);

		auto YZ = octoon::GameObject::create();
		YZ->addComponent<octoon::MeshFilterComponent>(planeGeometry);
		YZ->addComponent<octoon::MeshRendererComponent>(material);

		auto XZ = octoon::GameObject::create();
		XZ->addComponent<octoon::MeshFilterComponent>(planeGeometry);
		XZ->addComponent<octoon::MeshRendererComponent>(material);

		auto XYZE = octoon::GameObject::create();
		XYZE->addComponent<octoon::MeshFilterComponent>(planeGeometry);
		XYZE->addComponent<octoon::MeshRendererComponent>(material);

		this->planes = {
			{ "XY", XY },
			{ "YZ", YZ },
			{ "XZ", XZ },
			{ "XYZE", XYZE }
		};

		this->activePlane = this->planes["XYZE"];

		planes["YZ"]->getComponent<octoon::TransformComponent>()->setEulerAngles(octoon::math::float3(0, octoon::math::PI / 2, 0));
		planes["XZ"]->getComponent<octoon::TransformComponent>()->setEulerAngles(octoon::math::float3(-octoon::math::PI / 2, 0, 0));

		for (auto plane : planes) {
			plane.second->setName(plane.first);
		}
	}

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

			auto cylinderGizmos = octoon::ConeHelper::create(0.03, 1.0, 4, 1, false);
			auto cylinderGeometry = octoon::ConeHelper::create(0.2, 1.0, 4, 1, false);

			auto arrowGeometry = std::make_shared<octoon::mesh::Mesh>();
			arrowGeometry->setVertexArray(arrowVertices);

			auto lineXGeometry = std::make_shared<octoon::mesh::Mesh>();
			auto lineYGeometry = std::make_shared<octoon::mesh::Mesh>();
			auto lineZGeometry = std::make_shared<octoon::mesh::Mesh>();

			lineXGeometry->setVertexArray({ octoon::math::float3(0,0,0), octoon::math::float3(1,0,0) });
			lineYGeometry->setVertexArray({ octoon::math::float3(0,0,0), octoon::math::float3(0,1,0) });
			lineZGeometry->setVertexArray({ octoon::math::float3(0,0,0), octoon::math::float3(0,0,1) });

			auto gizmoXMaterial = std::make_shared<GizmoMaterial>(octoon::math::float3(255 / 255.f, 162 / 255.f, 115 / 255.f), octoon::math::float3(1.0f, 1.0f, 0));
			auto gizmoYMaterial = std::make_shared<GizmoMaterial>(octoon::math::float3(156 / 255.f, 239 / 255.f, 108 / 255.f), octoon::math::float3(1.0f, 1.0f, 0));
			auto gizmoZMaterial = std::make_shared<GizmoMaterial>(octoon::math::float3(104 / 255.f, 156 / 255.f, 210 / 255.f), octoon::math::float3(1.0f, 1.0f, 0));

			auto gizmoLineXMaterial = std::make_shared<GizmoLineMaterial>(octoon::math::float3(255 / 255.f, 162 / 255.f, 115 / 255.f), octoon::math::float3(1.0f, 1.0f, 0));
			auto gizmoLineYMaterial = std::make_shared<GizmoLineMaterial>(octoon::math::float3(156 / 255.f, 239 / 255.f, 108 / 255.f), octoon::math::float3(1.0f, 1.0f, 0));
			auto gizmoLineZMaterial = std::make_shared<GizmoLineMaterial>(octoon::math::float3(104 / 255.f, 156 / 255.f, 210 / 255.f), octoon::math::float3(1.0f, 1.0f, 0));

			auto handleX = octoon::GameObject::create();
			handleX->addComponent<octoon::MeshFilterComponent>(arrowGeometry);
			handleX->addComponent<octoon::MeshRendererComponent>(*gizmoXMaterial);
			handleX->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(1, 0, 0));
			handleX->getComponent<octoon::TransformComponent>()->setEulerAngles(octoon::math::float3(-octoon::math::PI / 2, 0, -octoon::math::PI / 2));
			
			auto handleY = octoon::GameObject::create();
			handleY->addComponent<octoon::MeshFilterComponent>(arrowGeometry);
			handleY->addComponent<octoon::MeshRendererComponent>(*gizmoYMaterial);
			handleY->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 1, 0));

			auto handleZ = octoon::GameObject::create();
			handleZ->addComponent<octoon::MeshFilterComponent>(arrowGeometry);
			handleZ->addComponent<octoon::MeshRendererComponent>(*gizmoZMaterial);
			handleZ->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 1));
			handleZ->getComponent<octoon::TransformComponent>()->setEulerAngles(octoon::math::float3(octoon::math::PI / 2, 0, 0));

			auto handleLineX = octoon::GameObject::create();
			handleLineX->addComponent<octoon::MeshFilterComponent>(lineXGeometry);
			handleLineX->addComponent<octoon::MeshRendererComponent>(*gizmoLineXMaterial);

			auto handleLineY = octoon::GameObject::create();
			handleLineY->addComponent<octoon::MeshFilterComponent>(lineYGeometry);
			handleLineY->addComponent<octoon::MeshRendererComponent>(*gizmoLineYMaterial);

			auto handleLineZ = octoon::GameObject::create();
			handleLineZ->addComponent<octoon::MeshFilterComponent>(lineZGeometry);
			handleLineZ->addComponent<octoon::MeshRendererComponent>(*gizmoLineZMaterial);

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
				{ "X", { handleX, handleLineX } },
				{ "Y", { handleY, handleLineY }  },
				{ "Z", { handleZ, handleLineY }  }
			};

			this->pickerGizmos = {
				{ "X", { handleX, handleLineX } },
				{ "Y", { handleY, handleLineY }  },
				{ "Z", { handleZ, handleLineY }  }
			};

			//this->init();
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

	void
	GizmoComponent::onEnable() noexcept
	{
		//translateGizmo_ = std::make_unique<TransformGizmoTranslate>();
	}

	void
	GizmoComponent::onDisable() noexcept
	{
	}
}