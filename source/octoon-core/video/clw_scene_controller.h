#ifndef OCTOON_VIDEO_CLW_SCENE_CONTROLLER_H_
#define OCTOON_VIDEO_CLW_SCENE_CONTROLLER_H_

#include <CLW.h>

#include "scene_controller.h"

namespace octoon::video
{
	class ClwSceneController : public SceneController
	{
	public:
		ClwSceneController(CLWContext context);

	private:
		CLWContext context_;
	};
}

#endif