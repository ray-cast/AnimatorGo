#include "gizmo_component.h"
#include "unreal_behaviour.h"

namespace unreal
{
	class GizmoMaterial final : public octoon::MeshBasicMaterial
	{
	public:
		GizmoMaterial(const octoon::math::float3& color, const octoon::math::float3& highlight)
		{
			this->opacity_ = 0.7f;
			this->color_ = octoon::math::srgb2linear(color);
			this->highlightColor_ = octoon::math::srgb2linear(highlight);

			this->setColor(this->color_);
			this->setDepthEnable(false);
			this->setDepthWriteEnable(false);
			this->setCullMode(octoon::CullMode::Off);
			this->setBlendEnable(true);
			this->setBlendSrc(octoon::BlendMode::SrcAlpha);
			this->setBlendDest(octoon::BlendMode::OneMinusSrcAlpha);
			this->setOpacity(this->opacity_);
		}

		void highlight(bool highlighted)
		{
			if (highlighted)
			{
				this->setColor(this->highlightColor_);
				this->setOpacity(1.0f);
			}
			else
			{
				this->setColor(this->color_);
				this->setOpacity(this->opacity_);
			}
		}

	private:
		GizmoMaterial(const GizmoMaterial&) = delete;
		GizmoMaterial& operator=(const GizmoMaterial&) = delete;

	private:
		float opacity_;
		octoon::math::float3 color_;
		octoon::math::float3 highlightColor_;
	};

	TransformGizmo::TransformGizmo()
	{
		auto planeGeometry = std::make_shared<octoon::PlaneMesh>(50.0f, 50.0f);

		auto material = std::make_shared<octoon::MeshBasicMaterial>();
		material->setCullMode(octoon::CullMode::Off);

		auto XY = std::make_shared<octoon::GameObject>(std::string_view("XY"));
		XY->setRaycastEnable(false);
		XY->addComponent<octoon::MeshFilterComponent>(planeGeometry);

		auto YZ = std::make_shared<octoon::GameObject>(std::string_view("YZ"));
		YZ->setRaycastEnable(false);
		YZ->addComponent<octoon::MeshFilterComponent>(planeGeometry);
		YZ->getComponent<octoon::TransformComponent>()->setEulerAngles(octoon::math::float3(0, octoon::math::PI / 2, 0));

		auto XZ = std::make_shared<octoon::GameObject>(std::string_view("XZ"));
		XZ->setRaycastEnable(false);
		XZ->addComponent<octoon::MeshFilterComponent>(planeGeometry);
		XZ->getComponent<octoon::TransformComponent>()->setEulerAngles(octoon::math::float3(-octoon::math::PI / 2, 0, 0));
		//XZ->addComponent<octoon::MeshRendererComponent>(material);

		this->planes = std::make_shared<octoon::GameObject>();
		this->planes->addChild(XY);
		this->planes->addChild(YZ);
		this->planes->addChild(XZ);

		this->activePlane = this->planes->findChild("XY");
	}

	void
	TransformGizmo::highlight(std::string_view axis) noexcept
	{
		for (auto& handle : this->handleGizmos->getChildren())
		{
			for (auto& child : handle->getChildren())
			{
				auto material = std::dynamic_pointer_cast<GizmoMaterial>(child->getComponent<octoon::MeshRendererComponent>()->getMaterial());
				material->highlight(handle->getName() == axis);
			}
		}
	}

	class TransformGizmoTranslate : public TransformGizmo
	{
	public:
		TransformGizmoTranslate()
		{
			auto gizmoXMaterial = std::make_shared<GizmoMaterial>(octoon::math::float3(255, 162, 115) / 255.f, octoon::math::float3(1.0f, 1.0f, 0));
			auto gizmoYMaterial = std::make_shared<GizmoMaterial>(octoon::math::float3(156, 239, 108) / 255.f, octoon::math::float3(1.0f, 1.0f, 0));
			auto gizmoZMaterial = std::make_shared<GizmoMaterial>(octoon::math::float3(104, 156, 210) / 255.f, octoon::math::float3(1.0f, 1.0f, 0));

			octoon::math::float3s arrowVertices = {
				octoon::math::float3(  0.5f, 1.f, 0),
				octoon::math::float3(  0.f,  2.f, 0),
				octoon::math::float3(-0.5f,  1.f, 0),
				octoon::math::float3( 0.25f, 0.f, 0),
				octoon::math::float3( 0.25f, 1.f, 0),
				octoon::math::float3(-0.25f, 1.f, 0),
				octoon::math::float3( 0.25f, 0.f, 0),
				octoon::math::float3(-0.25f, 1.f, 0),
				octoon::math::float3(-0.25f, 0.f, 0)
			};

			for (auto& vertices : arrowVertices) {
				vertices *= 0.35f;
			}

			auto arrowGeometry = std::make_shared<octoon::Mesh>();
			arrowGeometry->setVertexArray(arrowVertices);

			auto handleX = std::make_shared<octoon::GameObject>();
			handleX->setRaycastEnable(false);
			handleX->addComponent<octoon::MeshFilterComponent>(arrowGeometry);
			handleX->addComponent<octoon::MeshRendererComponent>(gizmoXMaterial)->setGlobalIllumination(false);
			handleX->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(1.0, 0, 0));
			handleX->getComponent<octoon::TransformComponent>()->setEulerAngles(octoon::math::float3(-octoon::math::PI / 2, 0, -octoon::math::PI / 2));
			
			auto handleY = std::make_shared<octoon::GameObject>();
			handleY->setRaycastEnable(false);
			handleY->addComponent<octoon::MeshFilterComponent>(arrowGeometry);
			handleY->addComponent<octoon::MeshRendererComponent>(gizmoYMaterial)->setGlobalIllumination(false);
			handleY->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 1.0, 0));

			auto handleZ = std::make_shared<octoon::GameObject>();
			handleZ->setRaycastEnable(false);
			handleZ->addComponent<octoon::MeshFilterComponent>(arrowGeometry);
			handleZ->addComponent<octoon::MeshRendererComponent>(gizmoZMaterial)->setGlobalIllumination(false);
			handleZ->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 1.0));
			handleZ->getComponent<octoon::TransformComponent>()->setEulerAngles(octoon::math::float3(octoon::math::PI / 2, 0, 0));

			octoon::math::float3s rotateVertices = {
				octoon::math::float3( 0.5f, 1.f, 0),
				octoon::math::float3( 0.0f, 0.f, 0),
				octoon::math::float3(-0.5f, 1.f, 0),
			};

			for (auto& vertices : rotateVertices) {
				vertices *= 0.25f;
			}

			auto lineXGeometry = std::make_shared<octoon::CubeMesh>(1.0f, 0.0f, 0.05f);
			auto lineYGeometry = std::make_shared<octoon::CubeMesh>(0.05f, 1.0f, 0.0f);
			auto lineZGeometry = std::make_shared<octoon::CubeMesh>(0.05f, 0.0f, 1.0f);

			auto handleLineX = std::make_shared<octoon::GameObject>();
			handleLineX->setRaycastEnable(false);
			handleLineX->addComponent<octoon::MeshFilterComponent>(lineXGeometry);
			handleLineX->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0.5, 0, 0));
			handleLineX->addComponent<octoon::MeshRendererComponent>(gizmoXMaterial)->setGlobalIllumination(false);

			auto handleLineY = std::make_shared<octoon::GameObject>();
			handleLineY->setRaycastEnable(false);
			handleLineY->addComponent<octoon::MeshFilterComponent>(lineYGeometry);
			handleLineY->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0.5, 0));
			handleLineY->addComponent<octoon::MeshRendererComponent>(gizmoYMaterial)->setGlobalIllumination(false);

			auto handleLineZ = std::make_shared<octoon::GameObject>();
			handleLineZ->setRaycastEnable(false);
			handleLineZ->addComponent<octoon::MeshFilterComponent>(lineZGeometry);
			handleLineZ->getComponent<octoon::TransformComponent>()->setTranslate(octoon::math::float3(0, 0, 0.5));
			handleLineZ->addComponent<octoon::MeshRendererComponent>(gizmoZMaterial)->setGlobalIllumination(false);
			
			auto X = std::make_shared<octoon::GameObject>(std::string_view("X"));
			X->addChild(handleX);
			X->addChild(handleLineX);

			auto Y = std::make_shared<octoon::GameObject>(std::string_view("Y"));
			Y->addChild(handleY);
			Y->addChild(handleLineY);

			auto Z = std::make_shared<octoon::GameObject>(std::string_view("Z"));
			Z->addChild(handleZ);
			Z->addChild(handleLineZ);

			this->handleGizmos = std::make_shared<octoon::GameObject>();
			this->handleGizmos->addChild(X);
			this->handleGizmos->addChild(Y);
			this->handleGizmos->addChild(Z);

			this->pickerGizmos = this->handleGizmos;
		}

		void setActivePlane(const std::string& axis, const octoon::math::float3& eye)
		{
			auto q = this->planes->findChild("XY")->getComponent<octoon::TransformComponent>()->getRotation();
			auto qinv = octoon::math::inverse(q);
			auto localEye = octoon::math::rotate(qinv, eye);

			if (axis == "X") {
				this->activePlane = this->planes->findChild("XY");
				if (std::abs(localEye.y) > std::abs(localEye.z)) this->activePlane = this->planes->findChild("XZ");
			}
			else if (axis == "Y") {
				this->activePlane = this->planes->findChild("XY");
				if (std::abs(localEye.x) > std::abs(localEye.z)) this->activePlane = this->planes->findChild("YZ");
			}
			else if (axis == "Z") {
				this->activePlane = this->planes->findChild("XZ");
				if (std::abs(localEye.x) > std::abs(localEye.y)) this->activePlane = this->planes->findChild("YZ");
			}
			else if (axis == "RY") {
				this->activePlane = this->planes->findChild("XZ");
			}
		}

	private:
		std::shared_ptr<octoon::MeshBasicMaterial> pickerMaterial_;
	};

	GizmoComponent::GizmoComponent() noexcept
		: gizmoMode_("translate")
		, offset_(octoon::math::float3::Zero)
	{
	}

	GizmoComponent::~GizmoComponent() noexcept
	{
	}

	std::optional<octoon::RaycastHit>
	GizmoComponent::intersectObjects(float x, float y, octoon::GameObjects& pickerGizmos) noexcept
	{
		auto profile = this->getContext()->profile;

		auto& camera = profile->cameraModule->camera.getValue();
		if (camera)
		{
			auto cameraComponent = camera->getComponent<octoon::CameraComponent>();
			if (cameraComponent)
			{
				octoon::Raycaster raycaster(cameraComponent->screenToRay(octoon::math::float2(x, y)));

				for (auto& axis : pickerGizmos)
				{
					auto& intersects = raycaster.intersectObjects(axis->getChildren());
					if (!intersects.empty())
						return intersects.front();
				}
			}
		}

		return std::nullopt;
	}

	std::optional<octoon::RaycastHit>
	GizmoComponent::intersectObjects(float x, float y, octoon::GameObjectPtr& pickerGizmos) noexcept
	{
		auto profile = this->getContext()->profile;

		auto& camera = profile->cameraModule->camera.getValue();
		if (camera)
		{
			auto cameraComponent = camera->getComponent<octoon::CameraComponent>();
			if (cameraComponent)
			{
				octoon::Raycaster raycaster(cameraComponent->screenToRay(octoon::math::float2(x, y)));

				auto& intersects = raycaster.intersectObject(*pickerGizmos);
				if (!intersects.empty())
					return intersects.front();
			}
		}

		return std::nullopt;
	}

	void
	GizmoComponent::handleMouseDown(const octoon::input::InputEvent& event) noexcept
	{
		auto pickerObject = this->getContext()->profile->selectorModule->selectedItem_.getValue().value().object.lock();
		if (!pickerObject)
			return;

		auto intersect = this->intersectObjects(event.button.x, event.button.y, gizmo_[gizmoMode_]->pickerGizmos->getChildren());
		if (intersect)
		{
			this->axis_ = intersect.value().object.lock()->getParent()->getName();

			auto& camera = this->getContext()->profile->cameraModule->camera.getValue();
			if (camera)
			{
				auto eye = camera->getComponent<octoon::TransformComponent>()->getTranslate();
				eye -= pickerObject->getComponent<octoon::TransformComponent>()->getTranslate();

				this->gizmo_[this->gizmoMode_]->highlight(this->axis_);
				this->gizmo_[this->gizmoMode_]->setActivePlane(this->axis_, eye);

				auto planeIntersect = this->intersectObjects(event.button.x, event.button.y, gizmo_[gizmoMode_]->activePlane);
				if (planeIntersect)
				{
					this->oldScale_ = pickerObject->getComponent<octoon::TransformComponent>()->getScale();
					this->oldRotation_ = pickerObject->getComponent<octoon::TransformComponent>()->getRotation();
					this->oldPosition_ = pickerObject->getComponent<octoon::TransformComponent>()->getTranslate();

					this->offset_ = planeIntersect.value().point;

					this->captureEvent();
				}
			}
		}
	}

	void
	GizmoComponent::handleMouseUp(const octoon::input::InputEvent& event) noexcept
	{
		this->releaseEvent();
	}

	void
	GizmoComponent::handleMouseMove(const octoon::input::InputEvent& event) noexcept
	{
		auto pickerObject = this->getContext()->profile->selectorModule->selectedItem_.getValue().value().object.lock();
		if (!pickerObject)
			return;

		auto intersect = this->intersectObjects(event.motion.x, event.motion.y, gizmo_[gizmoMode_]->activePlane);
		if (intersect)
		{
			auto point = intersect.value().point;

			if (this->axis_ == "X" || this->axis_ == "Y" || this->axis_ == "Z")
			{
				auto offset = point - this->offset_;
				offset.z *= -1;

				if (this->axis_ != "X") offset.x = 0;
				if (this->axis_ != "Y") offset.y = 0;
				if (this->axis_ != "Z") offset.z = 0;

				gizmo_[gizmoMode_]->handleGizmos->getComponent<octoon::TransformComponent>()->setTranslate(oldPosition_ + offset);
				gizmo_[gizmoMode_]->planes->getComponent<octoon::TransformComponent>()->setTranslate(oldPosition_ + offset);
				pickerObject->getComponent<octoon::TransformComponent>()->setTranslate(oldPosition_ + offset);
			}
		}
	}

	void
	GizmoComponent::handleMouseHover(const octoon::input::InputEvent& event) noexcept
	{
		auto intersect = this->intersectObjects(event.motion.x, event.motion.y, gizmo_[gizmoMode_]->pickerGizmos->getChildren());

		std::string axis = "";
		if (intersect.has_value())
			axis = intersect.value().object.lock()->getParent()->getName();

		if (this->axis_ != axis)
		{
			this->axis_ = axis;
			gizmo_[gizmoMode_]->highlight(this->axis_);
		}
	}

	void
	GizmoComponent::onEnable() noexcept
	{
		gizmo_["translate"] = std::make_unique<TransformGizmoTranslate>();
	}

	void
	GizmoComponent::onDisable() noexcept
	{
	}

	void
	GizmoComponent::onMouseDown(const octoon::input::InputEvent& event) noexcept
	{
		if (this->getContext()->profile->selectorModule->selectedItem_.getValue())
		{
			if (event.button.button == octoon::input::InputButton::Code::Left)
				this->handleMouseDown(event);
		}
	}

	void
	GizmoComponent::onMouseUp(const octoon::input::InputEvent& event) noexcept
	{
		this->handleMouseUp(event);
	}

	void
	GizmoComponent::onMouseMotion(const octoon::input::InputEvent& event) noexcept
	{
		if (this->getContext()->profile->selectorModule->selectedItem_.getValue())
		{
			if (this->isCapture())
				this->handleMouseMove(event);
			else
				this->handleMouseHover(event);
		}
	}

	void
	GizmoComponent::onLateUpdate() noexcept
	{
		auto& profile = this->getContext()->profile;

		if (profile->selectorModule->selectedItem_.getValue() && !profile->playerModule->isPlaying)
		{
			auto item = profile->selectorModule->selectedItem_.getValue()->object.lock();
			if (item)
				gizmo_[gizmoMode_]->handleGizmos->setActiveDownwards(true);
			else
				gizmo_[gizmoMode_]->handleGizmos->setActiveDownwards(false);
		}
		else
		{
			gizmo_[gizmoMode_]->handleGizmos->setActiveDownwards(false);
		}
	}
}