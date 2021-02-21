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

		void setMainCamera(Camera* camera) noexcept;
		const Camera* getMainCamera() const noexcept;

		void setGlobalIllumination(bool enable) noexcept;
		bool getGlobalIllumination() const noexcept;

		void addCamera(Camera* camera) noexcept;
		void removeCamera(Camera* camera) noexcept;
		const std::vector<Camera*>& getCameras() const noexcept;

		void addLight(Light* light) noexcept;
		void removeLight(Light* light) noexcept;
		const std::vector<Light*>& getLights() const noexcept;

		void addGeometry(Geometry* geometry) noexcept;
		void removeGeometry(Geometry* geometry) noexcept;
		const std::vector<Geometry*>& getGeometries() const noexcept;

		void addRenderObject(RenderObject* object) noexcept;
		void removeRenderObject(RenderObject* object) noexcept;

		void sortCameras() noexcept;
		void sortGeometries() noexcept;

	private:
		RenderScene(const RenderScene&) = delete;
		RenderScene& operator=(const RenderScene&) = delete;

	private:
		bool enableGlobalIllumination_;

		Camera* mainCamera_;

		std::vector<Light*> lights_;
		std::vector<Camera*> cameras_;
		std::vector<Geometry*> renderables_;
	};
}

#endif