#ifndef OCTOON_RENDER_SCENE_H_
#define OCTOON_RENDER_SCENE_H_

#include <octoon/light/light.h>
#include <octoon/camera/camera.h>
#include <octoon/geometry/geometry.h>

namespace octoon::video
{
	class OCTOON_EXPORT RenderScene final
	{
	public:
		RenderScene() noexcept;
		~RenderScene() noexcept;

		void setMainCamera(camera::Camera* camera) noexcept;
		const camera::Camera* getMainCamera() const noexcept;

		void setGlobalIllumination(bool enable) noexcept;
		bool getGlobalIllumination() const noexcept;

		void addCamera(camera::Camera* camera) noexcept;
		void removeCamera(camera::Camera* camera) noexcept;
		const std::vector<camera::Camera*>& getCameras() const noexcept;

		void addLight(light::Light* light) noexcept;
		void removeLight(light::Light* light) noexcept;
		const std::vector<light::Light*>& getLights() const noexcept;

		void addGeometry(geometry::Geometry* geometry) noexcept;
		void removeGeometry(geometry::Geometry* geometry) noexcept;
		const std::vector<geometry::Geometry*>& getGeometries() const noexcept;

		void addRenderObject(RenderObject* object) noexcept;
		void removeRenderObject(RenderObject* object) noexcept;

		void sortCameras() noexcept;
		void sortGeometries() noexcept;

	private:
		RenderScene(const RenderScene&) = delete;
		RenderScene& operator=(const RenderScene&) = delete;

	private:
		bool enableGlobalIllumination_;

		camera::Camera* mainCamera_;

		std::vector<light::Light*> lights_;
		std::vector<camera::Camera*> cameras_;
		std::vector<geometry::Geometry*> renderables_;
	};
}

#endif