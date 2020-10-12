#ifndef OCTOON_VIDEO_CLW_SCENE_H_
#define OCTOON_VIDEO_CLW_SCENE_H_

#include <CLW.h>
#include <radeon_rays.h>
#undef PI

#include <octoon/video/collector.h>
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

    using namespace RadeonRays;

	class ClwScene : public CompiledScene
	{
	public:
		ClwScene(CLWContext context);

        #include "../../lib/system/Kernels/CL/payload.cl"

        bool dirty;
        bool showBackground;

        CLWBuffer<math::float4> vertices;
        CLWBuffer<math::float4> normals;
        CLWBuffer<math::float2> uvs;
        CLWBuffer<int> indices;

        CLWBuffer<Shape> shapes;
        CLWBuffer<ShapeAdditionalData> shapesAdditional;

        CLWBuffer<Light> lights;
        CLWBuffer<Volume> volumes;
        CLWBuffer<Texture> textures;
        CLWBuffer<Material> materials;
        CLWBuffer<std::int32_t> materialAttributes;
        CLWBuffer<char> texturedata;

        CLWBuffer<Camera> camera;
        CLWBuffer<int> lightDistributions;
        CLWBuffer<InputMapData> inputMapData;

        std::unique_ptr<Bundle> material_bundle;
        std::unique_ptr<Bundle> volume_bundle;
        std::unique_ptr<Bundle> texture_bundle;
        std::unique_ptr<Bundle> input_map_leafs_bundle;
        std::unique_ptr<Bundle> input_map_bundle;

        int numLights;
        int numGeometries;
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