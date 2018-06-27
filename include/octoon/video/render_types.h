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
		typedef std::shared_ptr<class OrthoCamera> OrthoCameraPtr;
		typedef std::shared_ptr<class PerspectiveCamera> PerspectiveCameraPtr;
		typedef std::shared_ptr<class FilmCamera> FilmCameraPtr;
		typedef std::shared_ptr<class Geometry> GeometryPtr;
		typedef std::shared_ptr<class Material> MaterialPtr;
		typedef std::shared_ptr<class RenderObject> RenderObjectPtr;
		typedef std::shared_ptr<class RenderScene> RenderScenePtr;
		typedef std::shared_ptr<class TextMaterial> TextMaterialPtr;

		typedef std::vector<TextMaterialPtr> TextMaterials;
		typedef std::vector<RenderScene*> RenderScenes;
		typedef std::vector<Camera*> CameraRaws;
		typedef std::vector<RenderObject*> RenderObjectRaws;

		enum class CameraOrder
		{
			Custom,
			Main,
			UI
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

		struct TextColor
		{
			enum Type
			{
				FrontColor,
				SideColor,
				BeginRange_ = FrontColor,
				EndRange_ = SideColor,
				RangeSize_ = (EndRange_ - BeginRange_) + 1,
			};
		};

		enum class TextShaingMode
		{
			PureColor,
		};

		typedef std::uint32_t TextColors;

		typedef void* WindHandle;
	}
}

#endif