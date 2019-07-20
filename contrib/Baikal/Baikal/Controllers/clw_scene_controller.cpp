#include "Controllers/clw_scene_controller.h"
#include "SceneGraph/scene1.h"
#include "SceneGraph/camera.h"
#include "SceneGraph/light.h"
#include "SceneGraph/shape.h"
#include "SceneGraph/material.h"
#include "SceneGraph/texture.h"
#include "SceneGraph/Collector/collector.h"
#include "SceneGraph/iterator.h"
#include "SceneGraph/uberv2material.h"
#include "SceneGraph/inputmaps.h"
#include "Utils/distribution1d.h"
#include "Utils/log.h"
#include "Utils/cl_inputmap_generator.h"
#include "Utils/cl_program_manager.h"
#include "Utils/cl_uberv2_generator.h"


#include <chrono>
#include <memory>
#include <stack>
#include <vector>
#include <array>

using namespace RadeonRays;

namespace Baikal
{
    static std::size_t align16(std::size_t value)
    {
        return (value + 0xF) / 0x10 * 0x10;
    }

    static CameraType GetCameraType(Camera& camera)
    {
        auto perspective = dynamic_cast<PerspectiveCamera*>(&camera);

        if (perspective)
        {
            return perspective->GetAperture() > 0.f ? CameraType::kPhysicalPerspective : CameraType::kPerspective;
        }

        auto ortho = dynamic_cast<OrthographicCamera*>(&camera);

        if (ortho)
        {
            return CameraType::kOrthographic;
        }

        return CameraType::kPerspective;
    }


    ClwSceneController::ClwSceneController(CLWContext context, RadeonRays::IntersectionApi* api, const CLProgramManager *program_manager)
    : m_context(context)
    , m_api(api)
    , m_default_material(UberV2Material::Create())
    , m_program_manager(program_manager)
    {
        auto acc_type = "fatbvh";
        auto builder_type = "sah";
        LogInfo("Configuring acceleration structure: ", acc_type, " with ", builder_type, " builder\n");
        m_api->SetOption("acc.type", acc_type);

#ifdef ENABLE_RAYMASK
        m_api->SetOption("bvh.force2level", 1.f);
#endif

        m_api->SetOption("bvh.builder", builder_type);
        m_api->SetOption("bvh.sah.num_bins", 16.f);
    }

    Material::Ptr ClwSceneController::GetDefaultMaterial() const
    {
        return m_default_material;
    }

    ClwSceneController::~ClwSceneController()
    {
    }

    static void SplitMeshesAndInstances(Iterator& shape_iter, std::set<Mesh::Ptr>& meshes, std::set<Instance::Ptr>& instances, std::set<Mesh::Ptr>& excluded_meshes)
    {
        // Clear all sets
        meshes.clear();
        instances.clear();
        excluded_meshes.clear();

        // Prepare instance check lambda
        auto is_instance = [](Shape::Ptr shape)
        {
            if (std::dynamic_pointer_cast<Instance>(shape))
            {
                return true;
            }
            else
            {
                return false;
            }
        };

        for (; shape_iter.IsValid(); shape_iter.Next())
        {
            auto shape = shape_iter.ItemAs<Shape>();

            if (!is_instance(shape))
            {
                meshes.emplace(std::static_pointer_cast<Mesh>(shape));
            }
            else
            {
                instances.emplace(std::static_pointer_cast<Instance>(shape));
            }
        }

        for (auto& i : instances)
        {
            auto base_mesh = std::static_pointer_cast<Mesh>(i->GetBaseShape());
            if (meshes.find(base_mesh) == meshes.cend())
            {
                excluded_meshes.emplace(base_mesh);
            }
        }
    }

    static std::size_t GetShapeIdx(Iterator& shape_iter, Shape::Ptr shape)
    {
        std::set<Mesh::Ptr> meshes;
        std::set<Mesh::Ptr> excluded_meshes;
        std::set<Instance::Ptr> instances;
        SplitMeshesAndInstances(shape_iter, meshes, instances, excluded_meshes);

        std::size_t idx = 0;
        for (auto& i : meshes)
        {
            if (i == shape)
            {
                return idx;
            }

            ++idx;
        }

        for (auto& i : excluded_meshes)
        {
            if (i == shape)
            {
                return idx;
            }

            ++idx;
        }

        for (auto& i : instances)
        {
            if (i == shape)
            {
                return idx;
            }

            ++idx;
        }

        return -1;
    }

    void ClwSceneController::UpdateIntersector(Scene1 const& scene, ClwScene& out) const
    {
        // Detach and delete all shapes
        for (auto& shape : out.isect_shapes)
        {
            m_api->DetachShape(shape);
            m_api->DeleteShape(shape);
        }

        // Clear shapes cache
        out.isect_shapes.clear();
        // Only visible shapes are attached to the API.
        // So excluded meshes are pushed into isect_shapes, but
        // not to visible_shapes.
        out.visible_shapes.clear();

        // Create new shapes
        auto shape_iter = scene.CreateShapeIterator();

        if (!shape_iter->IsValid())
        {
            throw std::runtime_error("No shapes in the scene");
        }

        // Split all shapes into meshes and instances sets.
        std::set<Mesh::Ptr> meshes;
        // Excluded shapes are shapes which are not in the scene,
        // but references by at least one instance.
        std::set<Mesh::Ptr> excluded_meshes;
        std::set<Instance::Ptr> instances;
        SplitMeshesAndInstances(*shape_iter, meshes, instances, excluded_meshes);

        // Keep shape->rr shape association for
        // instance base shape lookup.
        std::map<Shape::Ptr, RadeonRays::Shape*> rr_shapes;

        // Start from ID 1
        // Handle meshes
        int id = 1;
        for (auto& iter : meshes)
        {
            auto mesh = iter;

            auto shape = m_api->CreateMesh(
                                           // Vertices starting from the first one
                                           (float*)mesh->GetVertices(),
                                           // Number of vertices
                                           static_cast<int>(mesh->GetNumVertices()),
                                           // Stride
                                           sizeof(float3),
                                           // TODO: make API signature const
                                           reinterpret_cast<int const*>(mesh->GetIndices()),
                                           // Index stride
                                           0,
                                           // All triangles
                                           nullptr,
                                           // Number of primitives
                                           static_cast<int>(mesh->GetNumIndices() / 3)
                                           );

            auto transform = mesh->GetTransform();
            shape->SetTransform(transform, inverse(transform));
            shape->SetId(id++);
            // shape->SetMask(iter->GetVisibilityMask());

            out.isect_shapes.push_back(shape);
            out.visible_shapes.push_back(shape);
            rr_shapes[mesh] = shape;
        }

        // Handle excluded meshes
        for (auto& iter : excluded_meshes)
        {
            auto mesh = iter;

            auto shape = m_api->CreateMesh(
                                           // Vertices starting from the first one
                                           (float*)mesh->GetVertices(),
                                           // Number of vertices
                                           static_cast<int>(mesh->GetNumVertices()),
                                           // Stride
                                           sizeof(float3),
                                           // TODO: make API signature const
                                           reinterpret_cast<int const*>(mesh->GetIndices()),
                                           // Index stride
                                           0,
                                           // All triangles
                                           nullptr,
                                           // Number of primitives
                                           static_cast<int>(mesh->GetNumIndices() / 3)
                                           );

            auto transform = mesh->GetTransform();
            shape->SetTransform(transform, inverse(transform));
            shape->SetId(id++);
            out.isect_shapes.push_back(shape);
            rr_shapes[mesh] = shape;
        }

        // Handle instances
        for (auto& iter: instances)
        {
            auto instance = iter;
            auto rr_mesh = rr_shapes[instance->GetBaseShape()];
            auto shape = m_api->CreateInstance(rr_mesh);

            auto transform = instance->GetTransform();
            shape->SetTransform(transform, inverse(transform));
            shape->SetId(id++);
            out.isect_shapes.push_back(shape);
            out.visible_shapes.push_back(shape);
        }
    }

    void ClwSceneController::UpdateIntersectorTransforms(Scene1 const& scene, ClwScene& out) const
    {
        // Create new shapes
        auto shape_iter = scene.CreateShapeIterator();

        if (!shape_iter->IsValid())
        {
            throw std::runtime_error("No shapes in the scene");
        }

        // Split all shapes into meshes and instances sets.
        std::set<Mesh::Ptr> meshes;
        // Excluded shapes are shapes which are not in the scene,
        // but references by at least one instance.
        std::set<Mesh::Ptr> excluded_meshes;
        std::set<Instance::Ptr> instances;
        SplitMeshesAndInstances(*shape_iter, meshes, instances, excluded_meshes);

        auto rr_iter = out.isect_shapes.begin();

        // Start from ID 1
        // Handle meshes
        for (auto& iter : meshes)
        {
            auto mesh = iter;
            auto transform = mesh->GetTransform();
            (*rr_iter)->SetTransform(transform, inverse(transform));
            ++rr_iter;
        }

        // Handle excluded meshes
        for (auto& iter : excluded_meshes)
        {
            auto mesh = iter;
            auto transform = mesh->GetTransform();
            (*rr_iter)->SetTransform(transform, inverse(transform));
            ++rr_iter;
        }

        // Handle instances
        for (auto& iter : instances)
        {
            auto instance = iter;
            auto transform = instance->GetTransform();
            (*rr_iter)->SetTransform(transform, inverse(transform));
            ++rr_iter;
        }

        m_api->Commit();
    }

    void ClwSceneController::UpdateCamera(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, Collector& vol_collector, ClwScene& out) const
    {
        // TODO: support different camera types here
        auto camera = scene.GetCamera();

        // Create light buffer if needed
        if (out.camera.GetElementCount() == 0)
        {
            out.camera = m_context.CreateBuffer<ClwScene::Camera>(1, CL_MEM_READ_ONLY);
        }

        // TODO: remove this
        out.camera_type = GetCameraType(*camera);

        // Update camera data
        ClwScene::Camera* data = nullptr;

        // Map GPU camera buffer
        m_context.MapBuffer(0, out.camera, CL_MAP_WRITE, &data).Wait();

        // Copy camera parameters
        data->forward = camera->GetForwardVector();
        data->up = camera->GetUpVector();
        data->right = camera->GetRightVector();
        data->p = camera->GetPosition();
        data->aspect_ratio = camera->GetAspectRatio();
        data->dim = camera->GetSensorSize();
        data->zcap = camera->GetDepthRange();

        if (out.camera_type == CameraType::kPerspective ||
            out.camera_type == CameraType::kPhysicalPerspective)
        {
            auto physical_camera = std::static_pointer_cast<PerspectiveCamera>(camera);
            data->aperture = physical_camera->GetAperture();
            data->focal_length = physical_camera->GetFocalLength();
            data->focus_distance = physical_camera->GetFocusDistance();
        }

        // Unmap camera buffer
        m_context.UnmapBuffer(0, out.camera, data);

        // Update volume index
        out.camera_volume_index = GetVolumeIndex(vol_collector, camera->GetVolume());
    }

    void ClwSceneController::UpdateShapes(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, Collector& vol_collector, ClwScene& out) const
    {
        std::size_t num_vertices = 0;
        std::size_t num_normals = 0;
        std::size_t num_uvs = 0;
        std::size_t num_indices = 0;

        std::size_t num_vertices_written = 0;
        std::size_t num_normals_written = 0;
        std::size_t num_uvs_written = 0;
        std::size_t num_indices_written = 0;
        std::size_t num_shapes_written = 0;

        auto shape_iter = scene.CreateShapeIterator();

        // Sort shapes into meshes and instances sets.
        std::set<Mesh::Ptr> meshes;
        // Excluded meshes are meshes which are not in the scene,
        // but are references by at least one instance.
        std::set<Mesh::Ptr> excluded_meshes;
        std::set<Instance::Ptr> instances;
        SplitMeshesAndInstances(*shape_iter, meshes, instances, excluded_meshes);

        // Calculate GPU array sizes. Do that only for meshes,
        // since instances do not occupy space in vertex buffers.
        // However instances still have their own material ids.
        for (auto& iter : meshes)
        {
            auto mesh = iter;

            num_vertices += mesh->GetNumVertices();
            num_normals += mesh->GetNumNormals();
            num_uvs += mesh->GetNumUVs();
            num_indices += mesh->GetNumIndices();
        }

        // Excluded meshes still occupy space in vertex buffers.
        for (auto& iter : excluded_meshes)
        {
            auto mesh = iter;

            num_vertices += mesh->GetNumVertices();
            num_normals += mesh->GetNumNormals();
            num_uvs += mesh->GetNumUVs();
            num_indices += mesh->GetNumIndices();
        }

        // Instances only occupy material IDs space.
        for (auto& iter : instances)
        {
            auto instance = iter;
            auto mesh = std::static_pointer_cast<Mesh>(instance->GetBaseShape());
        }

        LogInfo("Creating vertex buffer...\n");
        // Create CL arrays
        out.vertices = m_context.CreateBuffer<float3>(num_vertices, CL_MEM_READ_ONLY);

        LogInfo("Creating normal buffer...\n");
        out.normals = m_context.CreateBuffer<float3>(num_normals, CL_MEM_READ_ONLY);

        LogInfo("Creating UV buffer...\n");
        out.uvs = m_context.CreateBuffer<float2>(num_uvs, CL_MEM_READ_ONLY);

        LogInfo("Creating index buffer...\n");
        out.indices = m_context.CreateBuffer<int>(num_indices, CL_MEM_READ_ONLY);

        // Total number of entries in shapes GPU array
        auto num_shapes = meshes.size() + excluded_meshes.size() + instances.size();
        out.shapes = m_context.CreateBuffer<ClwScene::Shape>(num_shapes, CL_MEM_READ_ONLY);
        out.shapes_additional = m_context.CreateBuffer<ClwScene::ShapeAdditionalData>(num_shapes, CL_MEM_READ_ONLY);

        float3* vertices = nullptr;
        float3* normals = nullptr;
        float2* uvs = nullptr;
        int* indices = nullptr;
        ClwScene::Shape* shapes = nullptr;
        ClwScene::ShapeAdditionalData* shapes_additional = nullptr;

        // Map arrays and prepare to write data
        LogInfo("Mapping buffers...\n");
        m_context.MapBuffer(0, out.vertices, CL_MAP_WRITE, &vertices);
        m_context.MapBuffer(0, out.normals, CL_MAP_WRITE, &normals);
        m_context.MapBuffer(0, out.uvs, CL_MAP_WRITE, &uvs);
        m_context.MapBuffer(0, out.indices, CL_MAP_WRITE, &indices);
        m_context.MapBuffer(0, out.shapes, CL_MAP_WRITE, &shapes).Wait();
        m_context.MapBuffer(0, out.shapes_additional, CL_MAP_WRITE, &shapes_additional).Wait();

        // Keep associated shapes data for instance look up.
        // We retrieve data from here while serializing instances,
        // using base shape lookup.
        std::map<Mesh::Ptr, ClwScene::Shape> shape_data;

        // Handle meshes
        //int shape_id = 0;
        for (auto& iter : meshes)
        {
            auto mesh = iter;

            // Get pointers data
            auto mesh_vertex_array = mesh->GetVertices();
            auto mesh_num_vertices = mesh->GetNumVertices();

            auto mesh_normal_array = mesh->GetNormals();
            auto mesh_num_normals = mesh->GetNumNormals();

            auto mesh_uv_array = mesh->GetUVs();
            auto mesh_num_uvs = mesh->GetNumUVs();

            auto mesh_index_array = mesh->GetIndices();
            auto mesh_num_indices = mesh->GetNumIndices();

            // Prepare shape descriptor
            ClwScene::Shape shape;

            shape.id = iter->GetId();

            shape.startvtx = static_cast<int>(num_vertices_written);
            shape.startidx = static_cast<int>(num_indices_written);

            auto transform = mesh->GetTransform();
            shape.transform.m0 = { transform.m00, transform.m01, transform.m02, transform.m03 };
            shape.transform.m1 = { transform.m10, transform.m11, transform.m12, transform.m13 };
            shape.transform.m2 = { transform.m20, transform.m21, transform.m22, transform.m23 };
            shape.transform.m3 = { transform.m30, transform.m31, transform.m32, transform.m33 };

            shape.linearvelocity = float3(0.0f, 0.f, 0.f);
            shape.angularvelocity = float3(0.f, 0.f, 0.f, 1.f);
            shape.material.offset = GetMaterialIndex(mat_collector, mesh->GetMaterial());
            shape.material.layers = GetMaterialLayers(mesh->GetMaterial());

            shape.volume_idx = GetVolumeIndex(vol_collector, mesh->GetVolumeMaterial());

            shape_data[mesh] = shape;

            std::copy(mesh_vertex_array, mesh_vertex_array + mesh_num_vertices, vertices + num_vertices_written);
            num_vertices_written += mesh_num_vertices;

            std::copy(mesh_normal_array, mesh_normal_array + mesh_num_normals, normals + num_normals_written);
            num_normals_written += mesh_num_normals;

            std::copy(mesh_uv_array, mesh_uv_array + mesh_num_uvs, uvs + num_uvs_written);
            num_uvs_written += mesh_num_uvs;

            std::copy(mesh_index_array, mesh_index_array + mesh_num_indices, indices + num_indices_written);
            num_indices_written += mesh_num_indices;

            shapes[num_shapes_written] = shape;

            ClwScene::ShapeAdditionalData shape_additional;
            shape_additional.group_id = mesh->GetGroupId();
            shapes_additional[num_shapes_written++] = shape_additional;
        }

        // Excluded shapes are handled in almost the same way
        // except materials.
        for (auto& iter : excluded_meshes)
        {
            auto mesh = iter;

            // Get pointers data
            auto mesh_vertex_array = mesh->GetVertices();
            auto mesh_num_vertices = mesh->GetNumVertices();

            auto mesh_normal_array = mesh->GetNormals();
            auto mesh_num_normals = mesh->GetNumNormals();

            auto mesh_uv_array = mesh->GetUVs();
            auto mesh_num_uvs = mesh->GetNumUVs();

            auto mesh_index_array = mesh->GetIndices();
            auto mesh_num_indices = mesh->GetNumIndices();

            // Prepare shape descriptor
            ClwScene::Shape shape;

            shape.id = mesh->GetId();

            shape.startvtx = static_cast<int>(num_vertices_written);
            shape.startidx = static_cast<int>(num_indices_written);

            auto transform = mesh->GetTransform();
            shape.transform.m0 = { transform.m00, transform.m01, transform.m02, transform.m03 };
            shape.transform.m1 = { transform.m10, transform.m11, transform.m12, transform.m13 };
            shape.transform.m2 = { transform.m20, transform.m21, transform.m22, transform.m23 };
            shape.transform.m3 = { transform.m30, transform.m31, transform.m32, transform.m33 };

            shape.linearvelocity = float3(0.0f, 0.f, 0.f);
            shape.angularvelocity = float3(0.f, 0.f, 0.f, 1.f);
            shape.material.offset = GetMaterialIndex(mat_collector, mesh->GetMaterial());
            shape.material.layers = GetMaterialLayers(mesh->GetMaterial());

            shape.volume_idx = GetVolumeIndex(vol_collector, mesh->GetVolumeMaterial());

            shape_data[mesh] = shape;

            std::copy(mesh_vertex_array, mesh_vertex_array + mesh_num_vertices, vertices + num_vertices_written);
            num_vertices_written += mesh_num_vertices;

            std::copy(mesh_normal_array, mesh_normal_array + mesh_num_normals, normals + num_normals_written);
            num_normals_written += mesh_num_normals;

            std::copy(mesh_uv_array, mesh_uv_array + mesh_num_uvs, uvs + num_uvs_written);
            num_uvs_written += mesh_num_uvs;

            std::copy(mesh_index_array, mesh_index_array + mesh_num_indices, indices + num_indices_written);
            num_indices_written += mesh_num_indices;

            shapes[num_shapes_written] = shape;

            ClwScene::ShapeAdditionalData shape_additional;
            shape_additional.group_id = mesh->GetGroupId();
            shapes_additional[num_shapes_written++] = shape_additional;
        }

        // Handle instances
        for (auto& iter : instances)
        {
            auto instance = iter;
            auto base_shape = std::static_pointer_cast<Mesh>(instance->GetBaseShape());
            auto transform = instance->GetTransform();

            // Here shape_data is guaranteed to contain
            // info for base_shape since we have serialized it
            // above in a different pass.
            ClwScene::Shape shape = shape_data[base_shape];

            shape.id = iter->GetId();

            // Instance has its own transform.
            shape.transform.m0 = { transform.m00, transform.m01, transform.m02, transform.m03 };
            shape.transform.m1 = { transform.m10, transform.m11, transform.m12, transform.m13 };
            shape.transform.m2 = { transform.m20, transform.m21, transform.m22, transform.m23 };
            shape.transform.m3 = { transform.m30, transform.m31, transform.m32, transform.m33 };

            shape.linearvelocity = float3(0.0f, 0.f, 0.f);
            shape.angularvelocity = float3(0.f, 0.f, 0.f, 1.f);
            shape.material.offset = GetMaterialIndex(mat_collector, instance->GetMaterial());
            shape.material.layers = std::static_pointer_cast<UberV2Material>(instance->GetMaterial())->GetLayers();

            shape.volume_idx = GetVolumeIndex(vol_collector, instance->GetVolumeMaterial());

            shapes[num_shapes_written] = shape;

            ClwScene::ShapeAdditionalData shape_additional;
            shape_additional.group_id = iter->GetGroupId();
            shapes_additional[num_shapes_written++] = shape_additional;
        }

        LogInfo("Unmapping buffers...\n");
        m_context.UnmapBuffer(0, out.vertices, vertices);
        m_context.UnmapBuffer(0, out.normals, normals);
        m_context.UnmapBuffer(0, out.uvs, uvs);
        m_context.UnmapBuffer(0, out.indices, indices);
        m_context.UnmapBuffer(0, out.shapes, shapes).Wait();
        m_context.UnmapBuffer(0, out.shapes_additional, shapes_additional).Wait();

        LogInfo("Updating intersector...\n");

        UpdateIntersector(scene, out);

        ReloadIntersector(scene, out);
    }

    void ClwSceneController::UpdateShapeProperties(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, Collector& volume_collector, ClwScene& out) const
    {
        auto shape_iter = scene.CreateShapeIterator();

        // Sort shapes into meshes and instances sets.
        std::set<Mesh::Ptr> meshes;
        // Excluded meshes are meshes which are not in the scene,
        // but are references by at least one instance.
        std::set<Mesh::Ptr> excluded_meshes;
        std::set<Instance::Ptr> instances;
        SplitMeshesAndInstances(*shape_iter, meshes, instances, excluded_meshes);

        ClwScene::Shape* shapes = nullptr;
        ClwScene::ShapeAdditionalData* shapes_additional = nullptr;

        // Map arrays and prepare to write data
        m_context.MapBuffer(0, out.shapes, CL_MAP_READ | CL_MAP_WRITE, &shapes).Wait();
        m_context.MapBuffer(0, out.shapes_additional, CL_MAP_READ | CL_MAP_WRITE, &shapes_additional).Wait();

        auto current_shape = shapes;
        auto current_shape_additional = shapes_additional;
        for (auto& iter : meshes)
        {
            auto mesh = iter;

            auto transform = mesh->GetTransform();
            current_shape->transform.m0 = { transform.m00, transform.m01, transform.m02, transform.m03 };
            current_shape->transform.m1 = { transform.m10, transform.m11, transform.m12, transform.m13 };
            current_shape->transform.m2 = { transform.m20, transform.m21, transform.m22, transform.m23 };
            current_shape->transform.m3 = { transform.m30, transform.m31, transform.m32, transform.m33 };
            current_shape->material.offset = GetMaterialIndex(mat_collector, mesh->GetMaterial());
            current_shape->material.layers = GetMaterialLayers(mesh->GetMaterial());

            current_shape->volume_idx = GetVolumeIndex(volume_collector, mesh->GetVolumeMaterial());

            current_shape->id = iter->GetId();
            current_shape_additional->group_id = iter->GetGroupId();

            ++current_shape;
            ++current_shape_additional;
        }

        // Excluded shapes are handled in almost the same way
        // except materials.
        for (auto& iter : excluded_meshes)
        {
            auto mesh = iter;

            auto transform = mesh->GetTransform();
            current_shape->transform.m0 = { transform.m00, transform.m01, transform.m02, transform.m03 };
            current_shape->transform.m1 = { transform.m10, transform.m11, transform.m12, transform.m13 };
            current_shape->transform.m2 = { transform.m20, transform.m21, transform.m22, transform.m23 };
            current_shape->transform.m3 = { transform.m30, transform.m31, transform.m32, transform.m33 };
            current_shape->material.offset = GetMaterialIndex(mat_collector, mesh->GetMaterial());
            current_shape->material.layers = GetMaterialLayers(mesh->GetMaterial());

            current_shape->volume_idx = GetVolumeIndex(volume_collector, mesh->GetVolumeMaterial());

            current_shape->id = iter->GetId();
            current_shape_additional->group_id = iter->GetGroupId();

            ++current_shape;
            ++current_shape_additional;
        }

        // Handle instances
        for (auto& iter : instances)
        {
            auto instance = iter;
            auto base_shape = std::static_pointer_cast<Mesh>(instance->GetBaseShape());
            auto transform = instance->GetTransform();

            current_shape->transform.m0 = { transform.m00, transform.m01, transform.m02, transform.m03 };
            current_shape->transform.m1 = { transform.m10, transform.m11, transform.m12, transform.m13 };
            current_shape->transform.m2 = { transform.m20, transform.m21, transform.m22, transform.m23 };
            current_shape->transform.m3 = { transform.m30, transform.m31, transform.m32, transform.m33 };
            current_shape->material.offset = GetMaterialIndex(mat_collector, instance->GetMaterial());
            current_shape->material.layers = GetMaterialLayers(std::static_pointer_cast<UberV2Material>(instance->GetMaterial()));

            current_shape->volume_idx = GetVolumeIndex(volume_collector, instance->GetVolumeMaterial());

            current_shape->id = iter->GetId();
            current_shape_additional->group_id = iter->GetGroupId();

            ++current_shape;
            ++current_shape_additional;
        }

        m_context.UnmapBuffer(0, out.shapes, shapes).Wait();
        m_context.UnmapBuffer(0, out.shapes_additional, shapes_additional).Wait();
    }

    void ClwSceneController::UpdateCurrentScene(Scene1 const& scene, ClwScene& out) const
    {
        ReloadIntersector(scene, out);
    }

    void ClwSceneController::UpdateMaterials(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, ClwScene& out) const
    {
        // Get new buffer size
        std::vector<int> mat_buffer;
        mat_buffer.reserve(1024 * 1024); //Reserv 1M of ints for material buffer.

        // Cleanup material mapping
        m_materialid_to_offset.clear();

        CLUberV2Generator uberv2_generator;

        // Serialize materials
        {
            // Update material bundle first to be able to track differences
            out.material_bundle.reset(mat_collector.CreateBundle());

            // Create material iterator
            auto mat_iter = mat_collector.CreateIterator();

            // Iterate and serialize
            for (; mat_iter->IsValid(); mat_iter->Next())
            {
                WriteMaterial(*mat_iter->ItemAs<Material>(), mat_collector, tex_collector, mat_buffer);

                uberv2_generator.AddMaterial(mat_iter->ItemAs<UberV2Material>());
            }

        }

        std::string uberv2_source = uberv2_generator.BuildSource();
        m_program_manager->AddHeader("uberv2_generated.cl", uberv2_source);


        // Recreate material buffer if it needs resize
        if (mat_buffer.size() > out.material_attributes.GetElementCount())
        {
            // Create material buffer
            out.material_attributes = m_context.CreateBuffer<int32_t>(mat_buffer.size(), CL_MEM_READ_ONLY);
        }

        int32_t *materials = nullptr;

        // Map GPU materials buffer
        m_context.MapBuffer(0, out.material_attributes, CL_MAP_WRITE, &materials).Wait();

        memcpy(materials, mat_buffer.data(), mat_buffer.size() * sizeof(int32_t));

        // Unmap material buffer
        m_context.UnmapBuffer(0, out.material_attributes, materials);
    }

    void ClwSceneController::UpdateVolumes(Scene1 const& scene, Collector& volume_collector, Collector& tex_collector, ClwScene& out) const
    {
        if (!volume_collector.GetNumItems())
            return;

        // Get new buffer size
        std::size_t vol_buffer_size = volume_collector.GetNumItems();

        // Recreate material buffer if it needs resize
        if (vol_buffer_size > out.volumes.GetElementCount())
        {
            // Create material buffer
            out.volumes = m_context.CreateBuffer<ClwScene::Volume>(vol_buffer_size, CL_MEM_READ_ONLY);
        }

        ClwScene::Volume* volumes = nullptr;

        // Map GPU materials buffer
        m_context.MapBuffer(0, out.volumes, CL_MAP_WRITE, &volumes).Wait();

        // Create volume iterator
        auto volume_iter = volume_collector.CreateIterator();

        out.volume_bundle.reset(volume_collector.CreateBundle());
        // Serialize
        size_t num_volumes_copied = 0;
        for (; volume_iter->IsValid(); volume_iter->Next())
        {
            WriteVolume(*volume_iter->ItemAs<VolumeMaterial>(), tex_collector, volumes + num_volumes_copied);
            ++num_volumes_copied;
        }

        // Unmap serial buffer
        m_context.UnmapBuffer(0, out.volumes, volumes);

        // Update number of volumes
        out.num_volumes = static_cast<int>(num_volumes_copied);
    }

    void ClwSceneController::ReloadIntersector(Scene1 const& scene, ClwScene& inout) const
    {
        m_api->DetachAll();

        for (auto& s : inout.visible_shapes)
        {
            m_api->AttachShape(s);
        }

        m_api->Commit();
    }

    void ClwSceneController::UpdateTextures(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, ClwScene& out) const
    {
        // Get new buffer size
        std::size_t tex_buffer_size = tex_collector.GetNumItems();
        std::size_t tex_data_buffer_size = 0;

        if (tex_buffer_size == 0)
        {
            out.textures = m_context.CreateBuffer<ClwScene::Texture>(1, CL_MEM_READ_ONLY);
            out.texturedata = m_context.CreateBuffer<char>(1, CL_MEM_READ_ONLY);
            return;
        }

        // Recreate material buffer if it needs resize
        if (tex_buffer_size > out.textures.GetElementCount())
        {
            // Create material buffer
            out.textures = m_context.CreateBuffer<ClwScene::Texture>(tex_buffer_size, CL_MEM_READ_ONLY);
        }

        ClwScene::Texture* textures = nullptr;
        std::size_t num_textures_written = 0;

        // Map GPU materials buffer
        m_context.MapBuffer(0, out.textures, CL_MAP_WRITE, &textures).Wait();

        // Update material bundle first to be able to track differences
        out.texture_bundle.reset(tex_collector.CreateBundle());

        // Create material iterator
        std::unique_ptr<Iterator> tex_iter(tex_collector.CreateIterator());

        // Iterate and serialize
        for (; tex_iter->IsValid(); tex_iter->Next())
        {
            auto tex = tex_iter->ItemAs<Texture>();

            WriteTexture(*tex, tex_data_buffer_size, textures + num_textures_written);

            ++num_textures_written;

            tex_data_buffer_size += align16(tex->GetSizeInBytes());
        }

        // Unmap material buffer
        m_context.UnmapBuffer(0, out.textures, textures);

        // Recreate material buffer if it needs resize
        if (tex_data_buffer_size > out.texturedata.GetElementCount())
        {
            // Create material buffer
            out.texturedata = m_context.CreateBuffer<char>(tex_data_buffer_size, CL_MEM_READ_ONLY);
        }

        char* data = nullptr;
        std::size_t num_bytes_written = 0;

        tex_iter->Reset();

        // Map GPU materials buffer
        m_context.MapBuffer(0, out.texturedata, CL_MAP_WRITE, &data).Wait();

        // Write texture data for all textures
        for (; tex_iter->IsValid(); tex_iter->Next())
        {
            auto tex = tex_iter->ItemAs<Texture>();

            WriteTextureData(*tex, data + num_bytes_written);

            num_bytes_written += align16(tex->GetSizeInBytes());
        }

        // Unmap material buffer
        m_context.UnmapBuffer(0, out.texturedata, data);
    }

#ifndef NDEBUG
    // We're not using this function on release
    // Convert Material:: types to ClwScene:: types
    static ClwScene::Bxdf GetMaterialType(Material const& material)
    {
        // Distinguish between single bxdf materials and compound ones
        if (dynamic_cast<UberV2Material const*>(&material))
        {
            return ClwScene::Bxdf::kUberV2;
        }

        return ClwScene::Bxdf::kZero;
    }
#endif

    void ClwSceneController::WriteMaterial(Material const& material, Collector& mat_collector, Collector& tex_collector, std::vector<std::int32_t> &material_data) const
    {
        assert(GetMaterialType(material) == ClwScene::Bxdf::kUberV2);
        const UberV2Material &uber_material = static_cast<const UberV2Material&>(material);

        std::uint32_t layers = uber_material.GetLayers();
        
        m_materialid_to_offset[material.GetId()] = static_cast<int>(material_data.size());

        // Pack material parameters
        std::int32_t params = 0;
        params |= ((uber_material.IsLinkRefractionIOR()) ? 1 : 0) << 0;
        params |= ((uber_material.IsThin()) ? 1 : 0) << 1;
        params |= ((uber_material.isDoubleSided()) ? 1 : 0) << 2;
        params |= ((uber_material.IsMultiscatter()) ? 1 : 0) << 3;
        material_data.push_back(params);

        // Describe order of fields and layers
        static const std::vector<std::pair<UberV2Material::Layers, std::vector<std::string>>> uberv2_ordered_fields =
        {
            {
                UberV2Material::Layers::kEmissionLayer,
                { "uberv2.emission.color" }
            },
            {
                UberV2Material::Layers::kCoatingLayer,
                { "uberv2.coating.color", "uberv2.coating.ior"}
            },
            {
                UberV2Material::Layers::kReflectionLayer,
                {
                    "uberv2.reflection.color",
                    "uberv2.reflection.roughness",
                    "uberv2.reflection.anisotropy",
                    "uberv2.reflection.anisotropy_rotation",
                    "uberv2.reflection.ior",
                    "uberv2.reflection.metalness",
                }
            },
            {
                UberV2Material::Layers::kDiffuseLayer,
                { "uberv2.diffuse.color"}
            },
            {
                UberV2Material::Layers::kRefractionLayer,
                {
                    "uberv2.refraction.color",
                    "uberv2.refraction.roughness",
                    "uberv2.refraction.ior"
                }
            },
            {
                UberV2Material::Layers::kTransparencyLayer,
                { "uberv2.transparency" }
            },
            {
                UberV2Material::Layers::kShadingNormalLayer,
                { "uberv2.shading_normal" }
            },
            {
                UberV2Material::Layers::kSSSLayer,
                {
                    "uberv2.sss.absorption_color",
                    "uberv2.sss.scatter_color",
                    "uberv2.sss.subsurface_color",
                    "uberv2.sss.absorption_distance",
                    "uberv2.sss.scatter_distance",
                    "uberv2.sss.scatter_direction"
                }
            },
        };

        // Write material layers. Order matters.
        for (auto &layer_info : uberv2_ordered_fields)
        {
            if ((layers & layer_info.first) == layer_info.first)
            {
                for (auto &layer_param : layer_info.second)
                {
                    auto value = material.GetInputValue(layer_param);
                    assert(value.type == Material::InputType::kInputMap);
                    material_data.push_back(value.input_map_value ? value.input_map_value->GetId() : -1);
                }
            }
        }
    }

    // Convert Light:: types to ClwScene:: types
    static int GetLightType(Light const& light)
    {

        if (dynamic_cast<PointLight const*>(&light))
        {
            return ClwScene::kPoint;
        }
        else if (dynamic_cast<DirectionalLight const*>(&light))
        {
            return ClwScene::kDirectional;
        }
        else if (dynamic_cast<SpotLight const*>(&light))
        {
            return ClwScene::kSpot;
        }
        else if (dynamic_cast<ImageBasedLight const*>(&light))
        {
            return ClwScene::kIbl;
        }
        else
        {
            return ClwScene::LightType::kArea;
        }
    }

    void ClwSceneController::WriteLight(Scene1 const& scene, Light const& light, Collector& tex_collector, void* data) const
    {
        auto clw_light = reinterpret_cast<ClwScene::Light*>(data);

        auto type = GetLightType(light);

        clw_light->type = type;

        switch (type)
        {
            case ClwScene::kPoint:
            {
                clw_light->p = light.GetPosition();
                clw_light->intensity = light.GetEmittedRadiance();
                break;
            }

            case ClwScene::kDirectional:
            {
                clw_light->d = light.GetDirection();
                clw_light->intensity = light.GetEmittedRadiance();
                break;
            }

            case ClwScene::kSpot:
            {
                clw_light->p = light.GetPosition();
                clw_light->d = light.GetDirection();
                clw_light->intensity = light.GetEmittedRadiance();

                auto cone_shape = static_cast<SpotLight const&>(light).GetConeShape();
                clw_light->ia = cone_shape.x;
                clw_light->oa = cone_shape.y;
                break;
            }

            case ClwScene::kIbl:
            {
                auto& ibl = static_cast<ImageBasedLight const&>(light);
                clw_light->multiplier = ibl.GetMultiplier();
                auto main_tex = ibl.GetTexture();
                clw_light->tex = main_tex ? tex_collector.GetItemIndex(main_tex) : -1;
                auto reflection_tex = ibl.GetReflectionTexture();
                clw_light->tex_reflection = reflection_tex ? tex_collector.GetItemIndex(reflection_tex) : -1;
                auto refraction_tex = ibl.GetRefractionTexture();
                clw_light->tex_refraction = refraction_tex ? tex_collector.GetItemIndex(refraction_tex) : -1;
                auto transparency_tex = ibl.GetTransparencyTexture();
                clw_light->tex_transparency = transparency_tex ? tex_collector.GetItemIndex(transparency_tex) : -1;
                auto background_tex = ibl.GetBackgroundTexture();
                clw_light->tex_background = background_tex ? tex_collector.GetItemIndex(background_tex) : -1;
                clw_light->ibl_mirror_x = ibl.GetMirrorX();
                break;
            }

            case ClwScene::kArea:
            {
                // TODO: optimize this linear search
                auto shape = static_cast<AreaLight const&>(light).GetShape();

                auto shape_iter = scene.CreateShapeIterator();

                auto idx = GetShapeIdx(*shape_iter, shape);

                clw_light->id = shape->GetId();
                clw_light->shapeidx = static_cast<int>(idx);
                clw_light->primidx = static_cast<int>(static_cast<AreaLight const&>(light).GetPrimitiveIdx());
                break;
            }

            default:
            assert(false);
            break;
        }
    }

    void ClwSceneController::UpdateLights(Scene1 const& scene, Collector& mat_collector, Collector& tex_collector, ClwScene& out) const
    {
        std::size_t num_lights_written = 0;

        auto env_override = scene.GetEnvironmentOverride();

        auto num_lights = scene.GetNumLights();
        auto distribution_buffer_size = (1 + 1 + num_lights + num_lights);

        // Create light buffer if needed
        if (num_lights > out.lights.GetElementCount())
        {
            out.lights = m_context.CreateBuffer<ClwScene::Light>(num_lights, CL_MEM_READ_ONLY);
            out.light_distributions = m_context.CreateBuffer<int>(distribution_buffer_size, CL_MEM_READ_ONLY);
        }

        ClwScene::Light* lights = nullptr;

        m_context.MapBuffer(0, out.lights, CL_MAP_WRITE, &lights).Wait();
        std::unique_ptr<Iterator> light_iter(scene.CreateLightIterator());

        // Disable IBL by default
        out.envmapidx = -1;

        // Allocate intermediate storage for lights power distribution
        std::vector<float> light_power(num_lights);
        std::uint32_t k = 0;

        // Serialize
        {
            for (; light_iter->IsValid(); light_iter->Next())
            {
                auto light = light_iter->ItemAs<Light>();
                WriteLight(scene, *light, tex_collector, lights + num_lights_written);


                // Find and update IBL idx
                auto ibl = std::dynamic_pointer_cast<ImageBasedLight>(light_iter->ItemAs<Light>());
                if (ibl)
                {
                    out.envmapidx = static_cast<int>(num_lights_written);
                }

                ++num_lights_written;

                auto power = light->GetPower(scene);

                // TODO: move luminance calculation into utility function
                light_power[k++] = 0.2126f * power.x + 0.7152f * power.y + 0.0722f * power.z;
            }
        }

        m_context.UnmapBuffer(0, out.lights, lights);

        // Create distribution over light sources based on their power
        Distribution1D light_distribution(&light_power[0], (std::uint32_t)light_power.size());

        // Write distribution data
        int* distribution_ptr = nullptr;
        m_context.MapBuffer(0, out.light_distributions, CL_MAP_WRITE, &distribution_ptr).Wait();
        auto current = distribution_ptr;

        // Write the number of segments first
        *current++ = (int)light_distribution.m_num_segments;

        // Then write num_segments  + 1 CDF values
        auto values = reinterpret_cast<float*>(current);
        for (auto i = 0u; i < light_distribution.m_num_segments + 1; ++i)
        {
            values[i] = light_distribution.m_cdf[i];
        }

        // Then write num_segments PDF values
        values += light_distribution.m_num_segments + 1;

        for (auto i = 0u; i < light_distribution.m_num_segments; ++i)
        {
            values[i] = light_distribution.m_func_values[i] / light_distribution.m_func_sum;
        }

        m_context.UnmapBuffer(0, out.light_distributions, distribution_ptr);

        out.num_lights = static_cast<int>(num_lights_written);
    }


    // Convert texture format into ClwScene:: types
    static ClwScene::TextureFormat GetTextureFormat(Texture const& texture)
    {
        switch (texture.GetFormat())
        {
            case Texture::Format::kRgba8: return ClwScene::TextureFormat::RGBA8;
            case Texture::Format::kRgba16: return ClwScene::TextureFormat::RGBA16;
            case Texture::Format::kRgba32: return ClwScene::TextureFormat::RGBA32;
            default: return ClwScene::TextureFormat::RGBA8;
        }
    }

    void ClwSceneController::WriteTexture(Texture const& texture, std::size_t data_offset, void* data) const
    {
        auto clw_texture = reinterpret_cast<ClwScene::Texture*>(data);

        auto dim = texture.GetSize();

        clw_texture->w = dim.x;
        clw_texture->h = dim.y;
        clw_texture->d = dim.z;
        clw_texture->fmt = GetTextureFormat(texture);
        clw_texture->dataoffset = static_cast<int>(data_offset);
    }

    void ClwSceneController::WriteTextureData(Texture const& texture, void* data) const
    {
        auto begin = texture.GetData();
        auto end = begin + texture.GetSizeInBytes();
        std::copy(begin, end, static_cast<char*>(data));
    }

    void ClwSceneController::WriteVolume(VolumeMaterial const& volume, Collector& tex_collector, void* data) const
    {
        auto clw_volume = reinterpret_cast<ClwScene::Volume*>(data);

        clw_volume->type = ClwScene::VolumeType::kHomogeneous;
        clw_volume->data = -1;
        clw_volume->extra = -1;

        auto absorption_value = volume.GetInputValue("absorption");

        if (absorption_value.type == Material::InputType::kFloat4)
        {
            clw_volume->sigma_a.float_value.value = absorption_value.float_value;
            clw_volume->sigma_a.int_value.value[3] = -1;
        }
        else if (absorption_value.type == Material::InputType::kTexture)
        {
            clw_volume->sigma_a.float_value.value = absorption_value.float_value;
            clw_volume->sigma_a.int_value.value[3] = tex_collector.GetItemIndex(absorption_value.tex_value);
        }
        else
        {
            // TODO: should not happen
            assert(false);
        }

        
        clw_volume->sigma_e.float_value.value = volume.GetInputValue("emission").float_value;
        clw_volume->sigma_e.int_value.value[3] = -1;
        clw_volume->sigma_s.float_value.value = volume.GetInputValue("scattering").float_value;
        clw_volume->sigma_s.int_value.value[3] = -1;
        clw_volume->g = volume.GetInputValue("g").float_value.x;
    }

    int ClwSceneController::GetTextureIndex(Collector const& collector, Texture::Ptr texture) const
    {
        return texture ? collector.GetItemIndex(texture) : (-1);
    }

    int ClwSceneController::GetMaterialIndex(Collector const& collector, Material::Ptr material) const
    {
        auto m = material ? material : m_default_material;
        return ResolveMaterialPtr(m);
    }

    int ClwSceneController::GetVolumeIndex(Collector const& collector, VolumeMaterial::Ptr volume) const
    {
        return volume ? collector.GetItemIndex(volume) : (-1);
    }

    void ClwSceneController::UpdateSceneAttributes(Scene1 const& scene, Collector& tex_collector, ClwScene& out) const
    {
        auto bg_image = scene.GetBackgroundImage();
        out.background_idx = (bg_image) ? tex_collector.GetItemIndex(bg_image) : -1;
    }

    void Baikal::ClwSceneController::UpdateInputMaps(const Baikal::Scene1& scene, Baikal::Collector& input_map_collector, Collector& input_map_leafs_collector, ClwScene& out) const
    {
        CLInputMapGenerator generator;
        generator.Generate(input_map_collector, input_map_leafs_collector);
        std::string source = generator.GetGeneratedSource();
        m_program_manager->AddHeader("inputmaps.cl", source);
    }

    void Baikal::ClwSceneController::UpdateLeafsData(Scene1 const& scene, Collector& input_map_leafs_collector, Collector& tex_collector, ClwScene& out) const
    {
        // Get new buffer size
        std::size_t buffer_size = input_map_leafs_collector.GetNumItems();

        // Recreate input map leafs buffer if it needs resize
        if (buffer_size > out.input_map_data.GetElementCount())
        {
            // Create material buffer
            out.input_map_data = m_context.CreateBuffer<ClwScene::InputMapData>(buffer_size, CL_MEM_READ_ONLY);
        }

        if (buffer_size > 0)
        {
            ClwScene::InputMapData *input_map_data = nullptr;

            // Map GPU input map buffer
            m_context.MapBuffer(0, out.input_map_data, CL_MAP_WRITE, &input_map_data).Wait();

            // Update input map leafs bundle to be able to track differences
            out.input_map_leafs_bundle.reset(input_map_leafs_collector.CreateBundle());

            // leaf iterator
            auto iter = input_map_leafs_collector.CreateIterator();
            std::size_t num_inputmap_leafs_written = 0;

            // Iterate and serialize
            for (; iter->IsValid(); iter->Next())
            {
                WriteInputMapLeaf(*iter->ItemAs<InputMap>(), tex_collector, input_map_data + num_inputmap_leafs_written);
                ++num_inputmap_leafs_written;
            }

            //Unmap buffer
            m_context.UnmapBuffer(0, out.input_map_data, input_map_data);
        }
    }

    void Baikal::ClwSceneController::WriteInputMapLeaf(InputMap const& leaf, Collector& tex_collector, void* data) const
    {
        ClwScene::InputMapData *data_pointer = static_cast<ClwScene::InputMapData*>(data);
        if (!leaf.IsLeaf())
        {
            //Shouldn't happen
            assert(false);
            return;
        }

        switch (leaf.m_type)
        {
            case InputMap::InputMapType::kConstantFloat3:
            {
                const InputMap_ConstantFloat3 &i = static_cast<const InputMap_ConstantFloat3&>(leaf);
                data_pointer->float_value.value = i.GetValue();
                data_pointer->int_values.type = ClwScene::InputMapDataType::kFloat3;
                break;
            }
            case InputMap::InputMapType::kConstantFloat:
            {
                const InputMap_ConstantFloat &i = static_cast<const InputMap_ConstantFloat&>(leaf);
                data_pointer->float_value.value.x = i.GetValue();
                data_pointer->float_value.value.y = i.GetValue();
                data_pointer->float_value.value.z = i.GetValue();
                data_pointer->int_values.type = ClwScene::InputMapDataType::kFloat;
                break;
            }
            case InputMap::InputMapType::kSampler:
            {
                const InputMap_Sampler &i = static_cast<const InputMap_Sampler&>(leaf);
                data_pointer->int_values.idx = tex_collector.GetItemIndex(i.GetTexture());
                data_pointer->int_values.type = ClwScene::InputMapDataType::kInt;
                break;
            }
            case InputMap::InputMapType::kSamplerBumpmap:
            {
                const InputMap_SamplerBumpMap &i = static_cast<const InputMap_SamplerBumpMap&>(leaf);
                data_pointer->int_values.idx = tex_collector.GetItemIndex(i.GetTexture());
                data_pointer->int_values.type = ClwScene::InputMapDataType::kInt;
                break;
            }
            default:
                //Shouldn't happen
                assert(false);
        }
    }

    std::int32_t ClwSceneController::ResolveMaterialPtr(Material::Ptr material) const
    {
        auto it = m_materialid_to_offset.find(material->GetId());
        return (it == m_materialid_to_offset.end()) ? -1 : it->second;
    }

    int ClwSceneController::GetMaterialLayers(Material::Ptr material) const
    {
        auto m = material ? material : m_default_material;
        return std::static_pointer_cast<UberV2Material>(m)->GetLayers();
    }

}
