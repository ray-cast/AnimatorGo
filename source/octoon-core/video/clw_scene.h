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

        #include "Kernels/CL/payload.cl"

        CLWBuffer<math::float4> vertices;
        CLWBuffer<math::float4> normals;
        CLWBuffer<math::float2> uvs;
        CLWBuffer<int> indices;

        CLWBuffer<Shape> shapes;
        CLWBuffer<ShapeAdditionalData> shapes_additional;

        CLWBuffer<std::int32_t> material_attributes;
        CLWBuffer<Light> lights;
        CLWBuffer<Volume> volumes;
        CLWBuffer<Texture> textures;
        CLWBuffer<char> texturedata;

        CLWBuffer<Camera> camera;
        CLWBuffer<int> light_distributions;
        CLWBuffer<InputMapData> input_map_data;

        std::unique_ptr<Bundle> material_bundle;
        std::unique_ptr<Bundle> volume_bundle;
        std::unique_ptr<Bundle> texture_bundle;
        std::unique_ptr<Bundle> input_map_leafs_bundle;
        std::unique_ptr<Bundle> input_map_bundle;

        int num_lights;
        int num_volumes;
        int envmapidx;
        int background_idx;
        int camera_volume_index;
        CameraType camera_type;

        std::vector<RadeonRays::Shape*> isect_shapes;
        std::vector<RadeonRays::Shape*> visible_shapes;

	private:
		CLWContext context_;
	};
}

#endif