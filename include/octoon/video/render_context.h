#ifndef OCTOON_RENDER_CONTEXT_H_
#define OCTOON_RENDER_CONTEXT_H_

namespace octoon::video
{
	struct LightContext
	{
		std::size_t numDirectional;
		std::size_t numSpot;
		std::size_t numArea;
		std::size_t numPoint;
		std::size_t numAmbient;
		std::size_t numEnvironment;
	};

	struct RenderContext
	{
		LightContext light;
	};
}

#endif