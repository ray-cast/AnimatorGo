#include <octoon/point_renderer.h>
#include <octoon/transform_component.h>
#include <octoon/game_object.h>

namespace octoon
{
	OctoonImplementSubClass(PointRenderer, Renderer, "PointRenderer")

	PointRenderer::PointRenderer() noexcept
		: geometry_(std::make_shared<video::Geometry>())
	{
		geometry_->setDrawType(video::DrawType::Points);
	}

	PointRenderer::PointRenderer(video::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	PointRenderer::PointRenderer(const video::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	PointRenderer::~PointRenderer() noexcept
	{
	}

	GameComponentPtr
	PointRenderer::clone() const noexcept
	{
		auto instance = std::make_shared<PointRenderer>();
		instance->setMaterial(this->getMaterial());
		return instance;
	}

	void
	PointRenderer::on_activate() noexcept
	{
		this->add_component_dispatch(GameDispatchType::MoveAfter, this);

		auto transform = this->get_component<TransformComponent>();

		geometry_->setActive(true);

		// geometry_->setMesh(this->get_component<video::Mesh>());
		geometry_->setTransform(transform->get_transform(), transform->get_transform_inverse());
	}

	void
	PointRenderer::on_deactivate() noexcept
	{
		this->remove_component_dispatch(GameDispatchType::MoveAfter, this);

		geometry_->setActive(false);
	}

	void
	PointRenderer::on_move_before() noexcept
	{
	}

	void
	PointRenderer::on_move_after() noexcept
	{
		auto transform = this->get_component<TransformComponent>();
		geometry_->setTransform(transform->get_transform(), transform->get_transform_inverse());
	}
}