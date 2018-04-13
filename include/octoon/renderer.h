#ifndef OCTOON_RENDERER_H_
#define OCTOON_RENDERER_H_

#include <octoon/video/material.h>
#include <octoon/game_component.h>

namespace octoon
{
	class OCTOON_EXPORT Renderer : public GameComponent
	{
		OctoonDeclareSubInterface(Renderer, GameComponent)
	public:
		Renderer() noexcept;
		Renderer(video::MaterialPtr&& material) noexcept;
		Renderer(const video::MaterialPtr& material) noexcept;
		virtual ~Renderer() noexcept;

		void setMaterial(video::MaterialPtr&& material) noexcept;
		void setMaterial(const video::MaterialPtr& material) noexcept;
		const video::MaterialPtr& getMaterial() const noexcept;

	private:
		Renderer(const Renderer&) = delete;
		Renderer& operator=(const Renderer&) = delete;

	private:
		video::MaterialPtr material_;
	};
}

#endif