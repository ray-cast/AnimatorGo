#include <octoon/line_renderer.h>
#include <octoon/mesh_filter_component.h>
#include <octoon/transform_component.h>
#include <octoon/game_object.h>

namespace octoon
{
	OctoonImplementSubClass(LineRenderer, Renderer, "LineRenderer")

	LineRenderer::LineRenderer() noexcept
		: geometry_(std::make_shared<video::Geometry>())
	{
		geometry_->setDrawType(video::DrawType::Lines);
	}

	LineRenderer::LineRenderer(video::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	LineRenderer::LineRenderer(const video::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	LineRenderer::~LineRenderer() noexcept
	{
	}

	GameComponentPtr
	LineRenderer::clone() const noexcept
	{
		auto instance = std::make_shared<LineRenderer>();
		instance->setMaterial(this->getMaterial());
		return instance;
	}

	void
	LineRenderer::on_activate() noexcept
	{
		this->add_component_dispatch(GameDispatchType::MoveAfter, this);

		auto transform = this->get_component<TransformComponent>();

		geometry_->setActive(true);
		geometry_->setMesh(this->get_component<MeshFilterComponent>()->getMesh());
		geometry_->setTransform(transform->get_transform(), transform->get_transform_inverse());
	}

	void
	LineRenderer::on_deactivate() noexcept
	{
		this->remove_component_dispatch(GameDispatchType::MoveAfter, this);

		geometry_->setActive(false);
	}

	void
	LineRenderer::on_move_before() noexcept
	{
	}

	void
	LineRenderer::on_move_after() noexcept
	{
		auto transform = this->get_component<TransformComponent>();
		geometry_->setTransform(transform->get_transform(), transform->get_transform_inverse());
	}
}