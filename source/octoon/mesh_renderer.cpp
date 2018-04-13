#include <octoon/mesh_renderer.h>
#include <octoon/transform_component.h>
#include <octoon/game_object.h>

namespace octoon
{
	OctoonImplementSubClass(MeshRenderer, Renderer, "MeshRenderer")

	MeshRenderer::MeshRenderer() noexcept
		: geometry_(std::make_shared<video::Geometry>())
	{
		geometry_->setDrawType(video::DrawType::Triangles);
	}

	MeshRenderer::MeshRenderer(video::MaterialPtr&& material) noexcept
	{
		this->setMaterial(std::move(material));
	}

	MeshRenderer::MeshRenderer(const video::MaterialPtr& material) noexcept
	{
		this->setMaterial(material);
	}

	MeshRenderer::~MeshRenderer() noexcept
	{
	}

	GameComponentPtr
	MeshRenderer::clone() const noexcept
	{
		auto instance = std::make_shared<MeshRenderer>();
		instance->setMaterial(this->getMaterial());
		return instance;
	}

	void
	MeshRenderer::on_activate() noexcept
	{
		this->add_component_dispatch(GameDispatchType::MoveAfter, this);

		auto transform = this->get_component<TransformComponent>();

		geometry_->setActive(true);

		// geometry_->setMesh(this->get_component<Mesh>());
		geometry_->setTransform(transform->get_transform(), transform->get_transform_inverse());
	}

	void
	MeshRenderer::on_deactivate() noexcept
	{
		this->remove_component_dispatch(GameDispatchType::MoveAfter, this);

		geometry_->setActive(false);
	}

	void
	MeshRenderer::on_move_before() noexcept
	{
	}

	void
	MeshRenderer::on_move_after() noexcept
	{
		auto transform = this->get_component<TransformComponent>();
		geometry_->setTransform(transform->get_transform(), transform->get_transform_inverse());
	}
}