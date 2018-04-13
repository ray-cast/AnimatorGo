#ifndef OCTOON_RENDER_TYPES_H_
#define OCTOON_RENDER_TYPES_H_

#include <octoon/math/math.h>
#include <octoon/runtime/rtti.h>
#include <octoon/runtime/rtti_interface.h>

namespace octoon
{
	namespace video
	{
		typedef std::shared_ptr<class Camera> CameraPtr;
		typedef std::shared_ptr<class Mesh> MeshPtr;
		typedef std::shared_ptr<class Geometry> GeometryPtr;
		typedef std::shared_ptr<class Material> MaterialPtr;
		typedef std::shared_ptr<class RenderObject> RenderObjectPtr;
		typedef std::shared_ptr<class RenderScene> RenderScenePtr;
		typedef std::unique_ptr<class FontContour> FontContourPtr;
		typedef std::shared_ptr<class FontFile> FontFilePtr;
		typedef std::shared_ptr<class FontMaterial> FontMaterialPtr;

		typedef std::vector<FontFilePtr> FontFiles;
		typedef std::vector<FontMaterialPtr> FontMaterials;
		typedef std::vector<FontContourPtr> FontContours;
		typedef std::vector<RenderScene*> RenderScenes;
		typedef std::vector<Camera*> CameraRaws;
		typedef std::vector<RenderObject*> RenderObjectRaws;

		enum class CameraType
		{
			Ortho,
			Perspective
		};

		enum class GraphicsIndexType
		{
			Uint16,
			Uint32,
		};

		enum class DrawType
		{
			Points,
			Lines,
			Triangles
		};
	}
}

#endif