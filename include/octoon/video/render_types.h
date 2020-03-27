#ifndef OCTOON_RENDER_TYPES_H_
#define OCTOON_RENDER_TYPES_H_

#include <octoon/math/math.h>
#include <octoon/runtime/rtti.h>
#include <octoon/runtime/rtti_interface.h>

namespace octoon
{
	namespace camera
	{
		class Camera;
	}

	namespace video
	{
		typedef std::shared_ptr<class RenderPipeline> RenderPipelinePtr;
		typedef std::shared_ptr<class RenderObject> RenderObjectPtr;
		typedef std::shared_ptr<class RenderScene> RenderScenePtr;

		typedef std::vector<RenderPipelinePtr> RenderPipelines;
		typedef std::vector<RenderScene*> RenderScenes;
		typedef std::vector<RenderObject*> RenderObjectRaws;
	}
}

#endif