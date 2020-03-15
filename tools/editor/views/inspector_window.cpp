#include "inspector_window.h"

#include <octoon/octoon.h>
#include <octoon/ui/imgui.h>

namespace octoon
{
	namespace editor
	{
		OctoonImplementSubClass(InspectorWindow, GameComponent, "InspectorWindow")

		InspectorWindow::InspectorWindow() noexcept
			: isShowedInspectorWindow_(true)
		{
		}

		InspectorWindow::~InspectorWindow() noexcept
		{
		}

		void
		InspectorWindow::onActivate() noexcept
		{
			this->addComponentDispatch(GameDispatchType::Gui);
			this->addMessageListener("editor:object:selected", std::bind(&InspectorWindow::onObjectSelected, this, std::placeholders::_1));
		}

		void
		InspectorWindow::onDeactivate() noexcept
		{
			this->removeComponentDispatchs();
		}

		void 
		InspectorWindow::onGui() noexcept
		{
			if (!isShowedInspectorWindow_)
				return;

			if (imgui::beginDock("Inspector", &isShowedInspectorWindow_))
			{
				auto object = selected_.lock();
				if (object)
				{
					auto transform = object->getComponent<TransformComponent>();
					if (transform)
					{
						if (octoon::imgui::treeNodeEx("Transform", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
						{
							octoon::math::float3 matrixTranslation = transform->getTranslate();
							octoon::math::float3 matrixRotation = octoon::math::degress(octoon::math::eulerAngles(transform->getQuaternion()));
							octoon::math::float3 matrixScale = transform->getScale();

							octoon::imgui::dragFloat3("Tr", matrixTranslation.ptr(), 3);
							octoon::imgui::dragFloat3("Rt", matrixRotation.ptr(), 1);
							octoon::imgui::dragFloat3("Sc", matrixScale.ptr(), 1);

							transform->setLocalTranslate(matrixTranslation);
							transform->setLocalQuaternion(octoon::math::Quaternion(octoon::math::radians(matrixRotation)));
							transform->setLocalScale(matrixScale);

							octoon::imgui::treePop();
						}
					}

					auto camera = object->getComponent<PerspectiveCameraComponent>();
					if (camera)
					{
						if (octoon::imgui::treeNodeEx("Camera", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
						{
							float znear = camera->getNear();
							float zfar = camera->getFar();
							float fov = camera->getAperture();

							octoon::imgui::text("Field of view:");
							if (octoon::imgui::dragFloat("##fov", &fov, 0.1f, 1, 125))
								camera->setAperture(fov);

							octoon::imgui::text("Near:");
							if (octoon::imgui::dragFloat("##znear", &znear, 1e-3f, 1e-3, 1.0f, "%.03f"))
								camera->setNear(znear);

							octoon::imgui::text("Far:");
							if (octoon::imgui::dragFloat("##zfar", &zfar, 0.1f, 0, 99999))
								camera->setFar(zfar);

							octoon::imgui::treePop();
						}
					}

					auto orthoCamera = object->getComponent<OrthoCameraComponent>();
					if (orthoCamera)
					{
						if (octoon::imgui::treeNodeEx("Camera", octoon::imgui::GuiTreeNodeFlagBits::BulletBit | octoon::imgui::GuiTreeNodeFlagBits::DefaultOpenBit))
						{
							float znear = orthoCamera->getNear();
							float zfar = orthoCamera->getFar();

							octoon::imgui::text("Near:");
							if (octoon::imgui::dragFloat("##znear", &znear, 1e-3f, 1e-3, 1.0f, "%.03f"))
								orthoCamera->setNear(znear);

							octoon::imgui::text("Far:");
							if (octoon::imgui::dragFloat("##zfar", &zfar, 0.1f, 0, 99999))
								orthoCamera->setFar(zfar);

							octoon::imgui::treePop();
						}
					}
				}

				imgui::endDock();
			}
		}

		GameComponentPtr 
		InspectorWindow::clone() const noexcept
		{
			return std::make_shared<InspectorWindow>();
		}

		void
		InspectorWindow::onObjectSelected(const std::any& data) noexcept
		{
			auto object = std::any_cast<GameObject*>(data);
			if (object)
				selected_ = object->cast_pointer<GameObject>();
		}
	}
}