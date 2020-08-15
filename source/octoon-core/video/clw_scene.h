#ifndef OCTOON_VIDEO_CLW_SCENE_H_
#define OCTOON_VIDEO_CLW_SCENE_H_

#include <CLW.h>
#include <radeon_rays.h>
#undef PI
#include <octoon/video/compiled_scene.h>

namespace octoon::video
{
    enum class CameraType
    {
        kPerspective,
        kPhysicalPerspective,
        kSpherical,
        kFisheye,
        kOrthographic
    };

    class Bundle
    {
    public:
        virtual ~Bundle() = 0;
    };

    using namespace RadeonRays;

	class ClwScene : public CompiledScene
	{
	public:
		ClwScene(CLWContext context);

        #include "../../lib/system/Kernels/CL/payload.cl"

        CLWBuffer<math::float4> vertices;
        CLWBuffer<math::float4> normals;
        CLWBuffer<math::float2> uvs;
        CLWBuffer<int> indices;

        CLWBuffer<Shape> shapes;
        CLWBuffer<ShapeAdditionalData> shapesAdditional;

        CLWBuffer<std::int32_t> materialAttributes;
        CLWBuffer<Light> lights;
        CLWBuffer<Volume> volumes;
        CLWBuffer<Texture> textures;
        CLWBuffer<char> texturedata;

        CLWBuffer<Camera> camera;
        CLWBuffer<int> lightDistributions;
        CLWBuffer<InputMapData> inputMapData;

        std::unique_ptr<Bundle> materialBundle;
        std::unique_ptr<Bundle> volumeBundle;
        std::unique_ptr<Bundle> textureBundle;
        std::unique_ptr<Bundle> inputMapLeafsBundle;
        std::unique_ptr<Bundle> inputMapBundle;

        int numLights;
        int numVolumes;
        int envmapidx;
        int backgroundIdx;
        int cameraVolumeIndex;
        CameraType cameraType;

        std::vector<RadeonRays::Shape*> isectShapes;
        std::vector<RadeonRays::Shape*> visibleShapes;

	private:
		CLWContext context_;
	};
}

#endif