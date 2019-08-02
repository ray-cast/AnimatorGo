/**********************************************************************
Copyright (c) 2016 Advanced Micro Devices, Inc. All rights reserved.

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.  IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
********************************************************************/
#include "RadeonProRender.h"
#include "WrapObject/WrapObject.h"
#include "WrapObject/ContextObject.h"
#include "WrapObject/CameraObject.h"
#include "WrapObject/FramebufferObject.h"
#include "WrapObject/LightObject.h"
#include "WrapObject/Materials/MaterialObject.h"
#include "WrapObject/MatSysObject.h"
#include "WrapObject/SceneObject.h"
#include "WrapObject/ShapeObject.h"
#include "WrapObject/PostEffectObject.h"
#include "WrapObject/Exception.h"
#include "math/float2.h"
#include "math/float3.h"
#include "math/matrix.h"
#include "math/mathutils.h"

//defines behavior for unimplemented API part
//#define UNIMLEMENTED_FUNCTION return RPR_SUCCESS;
#define UNIMLEMENTED_FUNCTION return RPR_ERROR_UNIMPLEMENTED;

#define UNSUPPORTED_FUNCTION return RPR_SUCCESS;
//#define UNSUPPORTED_FUNCTION return RPR_ERROR_UNSUPPORTED;

static const std::map<rpr_material_node_input, std::string> kRPRInputStrings =
{
    { RPR_UBER_MATERIAL_DIFFUSE_COLOR, "uberv2.diffuse.color" },
    { RPR_UBER_MATERIAL_LAYERS, "uberv2.layers" },
    { RPR_UBER_MATERIAL_REFLECTION_COLOR, "uberv2.reflection.color" },
    { RPR_UBER_MATERIAL_REFLECTION_ROUGHNESS, "uberv2.reflection.roughness" },
    { RPR_UBER_MATERIAL_REFLECTION_ANISOTROPY, "uberv2.reflection.anisotropy" },
    { RPR_UBER_MATERIAL_REFLECTION_ANISOTROPY_ROTATION, "uberv2.reflection.anisotropy_rotation" },
    { RPR_UBER_MATERIAL_REFLECTION_IOR, "uberv2.reflection.ior" },
    { RPR_UBER_MATERIAL_REFLECTION_METALNESS, "uberv2.reflection.metalness" },
    { RPR_UBER_MATERIAL_REFRACTION_COLOR, "uberv2.refraction.color" },
    { RPR_UBER_MATERIAL_REFRACTION_ROUGHNESS, "uberv2.refraction.roughness" },
    { RPR_UBER_MATERIAL_REFRACTION_IOR, "uberv2.refraction.ior" },
    { RPR_UBER_MATERIAL_REFRACTION_IOR_MODE, "uberv2.refraction.ior_mode" },
    { RPR_UBER_MATERIAL_REFRACTION_THIN_SURFACE, "uberv2.refraction.thin_surface" },
    { RPR_UBER_MATERIAL_COATING_COLOR, "uberv2.coating.color" },
    { RPR_UBER_MATERIAL_COATING_IOR, "uberv2.coating.ior" },
    { RPR_UBER_MATERIAL_EMISSION_COLOR, "uberv2.emission.color" },
    { RPR_UBER_MATERIAL_EMISSION_WEIGHT, "uberv2.emission.weight" },
    { RPR_UBER_MATERIAL_EMISSION_MODE, "uberv2.emission.mode" },
    { RPR_UBER_MATERIAL_TRANSPARENCY, "uberv2.transparency" },
    { RPR_UBER_MATERIAL_NORMAL, "uberv2.normal" },
    { RPR_UBER_MATERIAL_BUMP, "uberv2.bump" },
    { RPR_UBER_MATERIAL_DISPLACEMENT, "uberv2.displacement" },
    { RPR_UBER_MATERIAL_SSS_ABSORPTION_COLOR, "uberv2.sss.absorption_color" },
    { RPR_UBER_MATERIAL_SSS_SCATTER_COLOR, "uberv2.sss.scatter_color" },
    { RPR_UBER_MATERIAL_SSS_ABSORPTION_DISTANCE, "uberv2.sss.absorption_distance" },
    { RPR_UBER_MATERIAL_SSS_SCATTER_DISTANCE, "uberv2.sss.scatter_distance" },
    { RPR_UBER_MATERIAL_SSS_SCATTER_DIRECTION, "uberv2.sss.scatter_direction" },
    { RPR_UBER_MATERIAL_SSS_SUBSURFACE_COLOR, "uberv2.sss.subsurface_color" },
    { RPR_UBER_MATERIAL_SSS_MULTISCATTER, "uberv2.sss.multiscatter" }
};

rpr_int rprLightSetGroupId(rpr_light light, rpr_uint groupId)
{
    UNIMLEMENTED_FUNCTION
}


rpr_int rprRegisterPlugin(rpr_char const * path)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprCreateContext(rpr_int api_version, rpr_int * pluginIDs, size_t pluginCount, rpr_creation_flags creation_flags, rpr_context_properties const * props, rpr_char const * cache_path, rpr_context * out_context)
{
    if (api_version != RPR_API_VERSION)
    {
        return RPR_ERROR_INVALID_API_VERSION;
    }

    rpr_int result = RPR_SUCCESS;
    try
    {
        //TODO: handle other options
        *out_context = new ContextObject(creation_flags);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }
    return result;
}

rpr_int rprContextSetActivePlugin(rpr_context context, rpr_int pluginID)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprContextGetInfo(rpr_context in_context, rpr_context_info in_context_info, size_t in_size, void * out_data, size_t * out_size_ret)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    switch (in_context_info)
    {
    case RPR_CONTEXT_RENDER_STATISTICS:
        context->GetRenderStatistics(out_data, out_size_ret);
        break;
    case RPR_CONTEXT_PARAMETER_COUNT:
        break;
    case RPR_OBJECT_NAME:
    {
        std::string name = context->GetName();
        if (out_data)
        {
            memcpy(out_data, name.c_str(), name.size() + 1);
        }
        if (out_size_ret)
        {
            *out_size_ret = name.size() + 1;
        }
        break;
    }
    default:
        UNIMLEMENTED_FUNCTION

    }
    return RPR_SUCCESS;
}

rpr_int rprContextGetParameterInfo(rpr_context context, int param_idx, rpr_parameter_info parameter_info, size_t size, void * data, size_t * size_ret)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprContextGetAOV(rpr_context in_context, rpr_aov in_aov, rpr_framebuffer * out_fb)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    rpr_int result = RPR_SUCCESS;
    try
    {
        *out_fb = context->GetAOV(in_aov);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }

    return result;
}

rpr_int rprContextSetAOV(rpr_context in_context, rpr_aov in_aov, rpr_framebuffer in_frame_buffer)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    FramebufferObject* buffer = WrapObject::Cast<FramebufferObject>(in_frame_buffer);

    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    if (!buffer)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    rpr_int result = RPR_SUCCESS;
    try
    {
        context->SetAOV(in_aov, buffer);
    }
    catch(Exception& e)
    {
        result = e.m_error;
    }

    return result;
}

rpr_int rprContextSetScene(rpr_context in_context, rpr_scene in_scene)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);

    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    context->SetCurrenScene(scene);

    return RPR_SUCCESS;
}


rpr_int rprContextGetScene(rpr_context in_context, rpr_scene * out_scene)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    SceneObject* scene = context->GetCurrentScene();
    *out_scene = scene;

    return RPR_SUCCESS;
}

rpr_int rprContextSetParameter1u(rpr_context in_context, rpr_char const * name, rpr_uint x)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    try
    {
        context->SetParameter(name, x);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }

    return RPR_SUCCESS;
}

rpr_int rprContextSetParameter1f(rpr_context in_context, rpr_char const * name, rpr_float x)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    try
    {
        context->SetParameter(name, x);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }

    return RPR_SUCCESS;
}

rpr_int rprContextSetParameter3f(rpr_context in_context, rpr_char const * name, rpr_float x, rpr_float y, rpr_float z)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    try
    {
        context->SetParameter(name, x, y, z);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }

    return RPR_SUCCESS;
}

rpr_int rprContextSetParameter4f(rpr_context in_context, rpr_char const * name, rpr_float x, rpr_float y, rpr_float z, rpr_float w)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    try
    {
        context->SetParameter(name, x, y, z, w);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }

    return RPR_SUCCESS;
}

rpr_int rprContextSetParameterString(rpr_context in_context, rpr_char const * name, rpr_char const * value)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    try
    {
        context->SetParameter(name, value);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }

    return RPR_SUCCESS;
}

rpr_int rprContextRender(rpr_context in_context)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    context->Render();

    return RPR_SUCCESS;
}

rpr_int rprContextRenderTile(rpr_context in_context, rpr_uint xmin, rpr_uint xmax, rpr_uint ymin, rpr_uint ymax)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    context->RenderTile(xmin, xmax, ymin, ymax);

    return RPR_SUCCESS;
}

rpr_int rprContextClearMemory(rpr_context context)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprContextCreateImage(rpr_context in_context, rpr_image_format const in_format, rpr_image_desc const * in_image_desc, void const * in_data, rpr_image * out_image)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }
    rpr_int result = RPR_SUCCESS;
    try
    {
        *out_image = context->CreateImage(in_format, in_image_desc, in_data);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }

    return result;
}

rpr_int rprContextCreateBuffer(rpr_context context, rpr_buffer_desc const * buffer_desc, void const * data, rpr_buffer * out_buffer)
{
    UNIMLEMENTED_FUNCTION
}


rpr_int rprContextCreateImageFromFile(rpr_context in_context, rpr_char const * in_path, rpr_image * out_image)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }
    rpr_int result = RPR_SUCCESS;
    try
    {
        *out_image = context->CreateImageFromFile(in_path);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }

    return result;
}

rpr_int rprContextCreateScene(rpr_context in_context, rpr_scene * out_scene)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    rpr_int result = RPR_SUCCESS;
    try
    {
        *out_scene = context->CreateScene();
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }

    return result;
}

rpr_int rprContextCreateInstance(rpr_context in_context, rpr_shape shape, rpr_shape * out_instance)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    ShapeObject* mesh = WrapObject::Cast<ShapeObject>(shape);

    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }
    
    if (!mesh || !out_instance)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    rpr_int result = RPR_SUCCESS;
    try
    {
        *out_instance = context->CreateShapeInstance(mesh);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }

    return result;
}

rpr_int rprContextCreateMesh(rpr_context in_context, 
                            rpr_float const * in_vertices, size_t in_num_vertices, rpr_int in_vertex_stride,
                            rpr_float const * in_normals, size_t in_num_normals, rpr_int in_normal_stride,
                            rpr_float const * in_texcoords, size_t in_num_texcoords, rpr_int in_texcoord_stride,
                            rpr_int const * in_vertex_indices, rpr_int in_vidx_stride,
                            rpr_int const * in_normal_indices, rpr_int in_nidx_stride,
                            rpr_int const * in_texcoord_indices, rpr_int in_tidx_stride,
                            rpr_int const * in_num_face_vertices, size_t in_num_faces, rpr_shape * out_mesh)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    rpr_int result = RPR_SUCCESS;
    try
    {
        *out_mesh = context->CreateShape(in_vertices, in_num_vertices, in_vertex_stride,
                                        in_normals, in_num_normals, in_normal_stride,
                                        in_texcoords, in_num_texcoords, in_texcoord_stride,
                                        in_vertex_indices, in_vidx_stride,
                                        in_normal_indices, in_nidx_stride,
                                        in_texcoord_indices, in_tidx_stride,
                                        in_num_face_vertices, in_num_faces);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }

    return result;
}

rpr_int rprContextCreateMeshEx(rpr_context context, 
                                                    rpr_float const * vertices, size_t num_vertices, rpr_int vertex_stride, 
                                                    rpr_float const * normals, size_t num_normals, rpr_int normal_stride, 
                                                    rpr_int const * perVertexFlag, size_t num_perVertexFlags, rpr_int perVertexFlag_stride, rpr_int numberOfTexCoordLayers, 
                                                    rpr_float const ** texcoords, size_t const * num_texcoords, rpr_int const * texcoord_stride, 
                                                    rpr_int const * vertex_indices, rpr_int vidx_stride, rpr_int const * normal_indices, rpr_int nidx_stride, 
                                                    rpr_int const ** texcoord_indices, rpr_int const * tidx_stride, 
                                                    rpr_int const * num_face_vertices, size_t num_faces, rpr_shape * out_mesh)
{
    if (num_perVertexFlags == 0 && numberOfTexCoordLayers == 1)
    {
        //can use rprContextCreateMesh
        return rprContextCreateMesh(context,
                                vertices, num_vertices, vertex_stride,
                                normals, num_normals, normal_stride,
                                texcoords[0], num_texcoords[0], texcoord_stride[0],
                                vertex_indices, vidx_stride,
                                normal_indices, nidx_stride,
                                texcoord_indices[0], tidx_stride[0],
                                num_face_vertices, num_faces, out_mesh);
    }
    UNIMLEMENTED_FUNCTION
}

rpr_int rprContextCreateMeshEx2(rpr_context context,
    rpr_float const * vertices, size_t num_vertices, rpr_int vertex_stride,
    rpr_float const * normals, size_t num_normals, rpr_int normal_stride,
    rpr_int const * perVertexFlag, size_t num_perVertexFlags, rpr_int perVertexFlag_stride, rpr_int numberOfTexCoordLayers,
    rpr_float const ** texcoords, size_t const * num_texcoords, rpr_int const * texcoord_stride,
    rpr_int const * vertex_indices, rpr_int vidx_stride, rpr_int const * normal_indices, rpr_int nidx_stride,
    rpr_int const ** texcoord_indices, rpr_int const * tidx_stride,
    rpr_int const * num_face_vertices, size_t num_faces, 
    rpr_mesh_info const * mesh_properties, rpr_shape * out_mesh)
{
    return rprContextCreateMeshEx(context,
        vertices, num_vertices, vertex_stride,
        normals, num_normals, normal_stride,
        perVertexFlag, num_perVertexFlags, perVertexFlag_stride, numberOfTexCoordLayers,
        texcoords, num_texcoords, texcoord_stride,
        vertex_indices, vidx_stride, normal_indices, nidx_stride,
        texcoord_indices, tidx_stride,
        num_face_vertices, num_faces,
        out_mesh);
}

rpr_int rprContextCreateCamera(rpr_context in_context, rpr_camera * out_camera)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    rpr_int result = RPR_SUCCESS;
    try
    {
        *out_camera = context->CreateCamera();
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }
    return result;
}

rpr_int rprContextCreateFrameBuffer(rpr_context in_context, rpr_framebuffer_format const in_format, rpr_framebuffer_desc const * in_fb_desc, rpr_framebuffer * out_fb)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    rpr_int result = RPR_SUCCESS;
    try
    {
        *out_fb = context->CreateFrameBuffer(in_format, in_fb_desc);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }
    return result;

    return RPR_SUCCESS;
}

rpr_int rprContextCreateFramebufferFromGLTexture2D(rpr_context in_context, rpr_GLenum target, rpr_GLint miplevel, rpr_GLuint texture, rpr_framebuffer * out_fb)
{
    //cast data
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    rpr_int result = RPR_SUCCESS;
    try
    {
        *out_fb = context->CreateFrameBufferFromGLTexture(target, miplevel, texture);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }
    return result;

    return RPR_SUCCESS;
}


rpr_int rprCameraGetInfo(rpr_camera in_camera, rpr_camera_info in_camera_info, size_t in_size, void * out_data, size_t * out_size_ret)
{
    CameraObject* cam = WrapObject::Cast<CameraObject>(in_camera);
    if (!cam)
    {
        return RPR_ERROR_INVALID_OBJECT;
    }

    std::vector<char> data;
    size_t size_ret = 0;
    switch (in_camera_info)
    {
    case RPR_CAMERA_FSTOP:
    {
        rpr_float value = cam->GetAperture();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_CAMERA_FOCAL_LENGTH:
    {
        rpr_float value = cam->GetFocalLength();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_CAMERA_SENSOR_SIZE:
    {
        RadeonRays::float2 value = cam->GetSensorSize();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_CAMERA_MODE:
    {
        //TODO: only prespective camera supported now
        rpr_camera_mode value = cam->GetMode();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_CAMERA_FOCUS_DISTANCE:
    {
        //TODO: only prespective camera supported now
        rpr_float value = cam->GetFocusDistance();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_CAMERA_POSITION:
    {
        RadeonRays::float3 pos;
        RadeonRays::float3 at;
        RadeonRays::float3 up;
        cam->GetLookAt(pos, at, up);
        size_ret = sizeof(pos);
        data.resize(size_ret);
        memcpy(&data[0], &pos, size_ret);
        break;
    }
    case RPR_CAMERA_LOOKAT:
    {
        RadeonRays::float3 pos;
        RadeonRays::float3 at;
        RadeonRays::float3 up;
        cam->GetLookAt(pos, at, up);
        size_ret = sizeof(at);
        data.resize(size_ret);
        memcpy(&data[0], &at, size_ret);
        break;
    }
    case RPR_CAMERA_UP:
    {
        RadeonRays::float3 pos;
        RadeonRays::float3 at;
        RadeonRays::float3 up;
        cam->GetLookAt(pos, at, up);
        size_ret = sizeof(up);
        data.resize(size_ret);
        memcpy(&data[0], &up, size_ret);
        break;
    }
    case RPR_OBJECT_NAME:
    {
        std::string name = cam->GetName();
        size_ret = name.size() + 1;
        data.resize(size_ret);
        memcpy(&data[0], name.c_str(), size_ret);
        break;
    }
    case RPR_CAMERA_ORTHO_WIDTH:
    {
        rpr_float value = cam->GetOrthoWidth();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_CAMERA_ORTHO_HEIGHT:
    {
        rpr_float value = cam->GetOrthoHeight();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_CAMERA_APERTURE_BLADES:
    case RPR_CAMERA_EXPOSURE:
    case RPR_CAMERA_FOCAL_TILT:
    case RPR_CAMERA_IPD:
    case RPR_CAMERA_LENS_SHIFT:
        UNSUPPORTED_FUNCTION
        break;
    default:
        UNIMLEMENTED_FUNCTION
    }

    if (out_size_ret)
    {
        *out_size_ret = size_ret;
    }
    if (out_data)
    {
        memcpy(out_data, &data[0], size_ret);
    }
    return RPR_SUCCESS;
}

rpr_int rprCameraSetFocalLength(rpr_camera in_camera, rpr_float flength)
{
    //cast data
    CameraObject* camera = WrapObject::Cast<CameraObject>(in_camera);
    if (!camera)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //translate meters to mm 
    camera->SetFocalLength(flength);

    return RPR_SUCCESS;
}

rpr_int rprCameraSetFocusDistance(rpr_camera in_camera, rpr_float fdist)
{
    //cast data
    CameraObject* camera = WrapObject::Cast<CameraObject>(in_camera);
    if (!camera)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    camera->SetFocusDistance(fdist);

    return RPR_SUCCESS;
}

rpr_int rprCameraSetTransform(rpr_camera in_camera, rpr_bool transpose, const rpr_float * transform)
{
    //cast data
    CameraObject* camera = WrapObject::Cast<CameraObject>(in_camera);
    if (!camera)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    RadeonRays::matrix m;
    //fill matrix
    memcpy(m.m, transform, 16 * sizeof(rpr_float));

    if (!transpose)
    {
        m = m.transpose();
    }

    camera->SetTransform(m);
    return RPR_SUCCESS;
}

rpr_int rprCameraSetSensorSize(rpr_camera in_camera, rpr_float in_width, rpr_float in_height)
{
    //cast data
    CameraObject* camera = WrapObject::Cast<CameraObject>(in_camera);
    if (!camera)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //convedrt meters to mm
    RadeonRays::float2 size = { in_width, in_height};
    camera->SetSensorSize(size);

    return RPR_SUCCESS;
}

rpr_int rprCameraLookAt(rpr_camera in_camera, rpr_float posx, rpr_float posy, rpr_float posz, rpr_float atx, rpr_float aty, rpr_float atz, rpr_float upx, rpr_float upy, rpr_float upz)
{
    //cast data
    CameraObject* camera = WrapObject::Cast<CameraObject>(in_camera);
    if (!camera)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //get camera and set values
    const RadeonRays::float3 pos = { posx, posy, posz };
    const RadeonRays::float3 at = { atx, aty, atz };
    const RadeonRays::float3 up = { upx, upy, upz};
    camera->LookAt(pos, at, up);

    return RPR_SUCCESS;
}

rpr_int rprCameraSetFStop(rpr_camera in_camera, rpr_float fstop)
{
    //cast data
    CameraObject* camera = WrapObject::Cast<CameraObject>(in_camera);
    if (!camera)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //translate m -> to mm
    camera->SetAperture(fstop);

    return RPR_SUCCESS;
}

rpr_int rprCameraSetApertureBlades(rpr_camera camera, rpr_uint num_blades)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprCameraSetExposure(rpr_camera camera, rpr_float exposure)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprCameraSetMode(rpr_camera in_camera, rpr_camera_mode mode)
{
    //cast data
    CameraObject* camera = WrapObject::Cast<CameraObject>(in_camera);
    if (!camera)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //TODO: only perspective camera used for now
    switch (mode)
    {
    case RPR_CAMERA_MODE_PERSPECTIVE:
    case RPR_CAMERA_MODE_ORTHOGRAPHIC:
        camera->SetMode(mode);
        break;
    default:
        UNIMLEMENTED_FUNCTION
    }
    
    return RPR_SUCCESS;
}

rpr_int rprCameraSetOrthoWidth(rpr_camera in_camera, rpr_float width)
{
    CameraObject* camera = WrapObject::Cast<CameraObject>(in_camera);
    if (!camera)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    camera->SetOrthoWidth(width);
    return RPR_SUCCESS;
}

rpr_int rprCameraSetFocalTilt(rpr_camera camera, rpr_float tilt)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprCameraSetIPD(rpr_camera camera, rpr_float ipd)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprCameraSetLensShift(rpr_camera camera, rpr_float shiftx, rpr_float shifty)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprCameraSetTiltCorrection(rpr_camera camera, rpr_float tiltX, rpr_float tiltY)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprCameraSetFarPlane(rpr_camera camera, rpr_float far)
{
    UNSUPPORTED_FUNCTION
}


rpr_int rprCameraSetNearPlane(rpr_camera camera, rpr_float near)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprCameraSetOrthoHeight(rpr_camera in_camera, rpr_float height)
{
    CameraObject* camera = WrapObject::Cast<CameraObject>(in_camera);
    if (!camera)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    camera->SetOrthoHeight(height);
    return RPR_SUCCESS;

}

rpr_int rprImageGetInfo(rpr_image in_image, rpr_image_info in_image_info, size_t in_size, void * in_data, size_t * in_size_ret)
{
    MaterialObject* img = WrapObject::Cast<MaterialObject>(in_image);
    if (!img || !img->IsImg())
    {
        return RPR_ERROR_INVALID_IMAGE;
    }

    std::vector<char> data;
    size_t size_ret = 0;
    switch (in_image_info)
    {
    case RPR_IMAGE_FORMAT:
    {
        //texture data always stored as 4 component FLOAT32
        rpr_image_format value = img->GetImageFormat();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_IMAGE_DESC:
    {
        rpr_image_desc value = img->GetImageDesc();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_IMAGE_DATA:
    {
        rpr_image_desc desc = img->GetImageDesc();
        const char* value = img->GetImageData();
        size_ret = desc.image_width * desc.image_height * desc.image_depth;
        data.resize(size_ret);
        memcpy(&data[0], value, size_ret);
        break;
    }
    case RPR_OBJECT_NAME:
    {
        std::string name = img->GetName();
        size_ret = name.size() + 1;
        data.resize(size_ret);
        memcpy(&data[0], name.c_str(), size_ret);
        break;
    }
    case RPR_IMAGE_WRAP:
        UNSUPPORTED_FUNCTION
        break;
    default:
        UNIMLEMENTED_FUNCTION
    }

    if (in_size_ret)
    {
        *in_size_ret = size_ret;
    }
    if (in_data)
    {
        memcpy(in_data, &data[0], size_ret);
    }

    return RPR_SUCCESS;
}

rpr_int rprImageSetWrap(rpr_image image, rpr_image_wrap_type type)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetTransform(rpr_shape in_shape, rpr_bool transpose, rpr_float const * transform)
{
    //cast data
    ShapeObject* shape = WrapObject::Cast<ShapeObject>(in_shape);
    if (!shape)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    RadeonRays::matrix m;
    //fill matrix
    memcpy(m.m, transform, 16  * sizeof(rpr_float));

    if (!transpose)
    {
        m = m.transpose();
    }

    RadeonRays::matrix rtol(-1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f);
    m = rtol * m;

    shape->SetTransform(m);
    return RPR_SUCCESS;
}

rpr_int rprShapeSetSubdivisionFactor(rpr_shape shape, rpr_uint factor)
{	
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetSubdivisionCreaseWeight(rpr_shape shape, rpr_float factor)
{	
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetSubdivisionBoundaryInterop(rpr_shape shape, rpr_subdiv_boundary_interfop_type type)
{	
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetDisplacementScale(rpr_shape shape, rpr_float minscale, rpr_float maxscale)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetObjectGroupID(rpr_shape shape, rpr_uint objectGroupID)
{
    //cast data
    ShapeObject* mesh = WrapObject::Cast<ShapeObject>(shape);
    if (!shape)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    mesh->GetShape()->SetGroupId(objectGroupID);
    return RPR_SUCCESS;
}

rpr_int rprShapeSetDisplacementMaterial(rpr_shape shape, rpr_material_node materialNode)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetMaterialFaces(rpr_shape shape, rpr_material_node node, rpr_int* face_indices, size_t num_faces)
{
    UNSUPPORTED_FUNCTION
}


rpr_int rprShapeSetDisplacementImage(rpr_shape shape, rpr_image image)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetMaterial(rpr_shape in_shape, rpr_material_node in_node)
{
    //cast data
    ShapeObject* shape = WrapObject::Cast<ShapeObject>(in_shape);
    MaterialObject* mat = WrapObject::Cast<MaterialObject>(in_node);
    if (!shape)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }
    
    rpr_int result = RPR_SUCCESS;
    try
    {
        //can throw exception if mat is a Texture
        shape->SetMaterial(mat);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }
    return result;
}

rpr_int rprShapeSetMaterialOverride(rpr_shape shape, rpr_material_node node)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetVolumeMaterial(rpr_shape shape, rpr_material_node node)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetLinearMotion(rpr_shape in_shape, rpr_float x, rpr_float y, rpr_float z)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetAngularMotion(rpr_shape shape, rpr_float x, rpr_float y, rpr_float z, rpr_float w)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetScaleMotion(rpr_shape shape, rpr_float x, rpr_float y, rpr_float z)
{
    UNSUPPORTED_FUNCTION
}


rpr_int rprShapeSetVisibility(rpr_shape shape, rpr_bool visible)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetVisibilityPrimaryOnly(rpr_shape shape, rpr_bool visible)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetVisibilityInSpecular(rpr_shape shape, rpr_bool visible)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetShadowCatcher(rpr_shape shape, rpr_bool shadowCatcher)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprShapeSetShadow(rpr_shape shape, rpr_bool casts_shadow)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprLightSetTransform(rpr_light in_light, rpr_bool in_transpose, rpr_float const * in_transform)
{
    //cast data
    LightObject* light = WrapObject::Cast<LightObject>(in_light);
    if (!light)
        return RPR_ERROR_INVALID_PARAMETER;
    
    RadeonRays::matrix m;
    memcpy(&m.m00, in_transform, 16 * sizeof(rpr_float));
    if (!in_transpose)
    {
        m = m.transpose();
    }

    RadeonRays::matrix rtol(-1.0f, 0.0f, 0.0f, 0.0f,
                            0.0f, 1.0f, 0.0f, 0.0f,
                            0.0f, 0.0f, 1.0f, 0.0f,
                            0.0f, 0.0f, 0.0f, 1.0f);
    m = rtol * m;

    light->SetTransform(m);

    return RPR_SUCCESS;
}

rpr_int rprShapeGetInfo(rpr_shape in_shape, rpr_shape_info in_info, size_t in_size, void * in_data, size_t * in_size_ret)
{
    ShapeObject* shape = WrapObject::Cast<ShapeObject>(in_shape);
    if (!shape)
    {
        return RPR_ERROR_INVALID_OBJECT;
    }

    std::vector<char> data;
    size_t size_ret = 0;
    switch (in_info)
    {
    case RPR_SHAPE_TYPE:
    {   
        int value = shape->IsInstance() ? RPR_SHAPE_TYPE_INSTANCE : RPR_SHAPE_TYPE_MESH;
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_SHAPE_MATERIAL:
    {
        MaterialObject* value = shape->GetMaterial();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_SHAPE_TRANSFORM:
    {
        RadeonRays::matrix value = shape->GetTransform();
        value = value.transpose();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_OBJECT_NAME:
    {
        std::string name = shape->GetName();
        size_ret = name.size() + 1;
        data.resize(size_ret);
        memcpy(&data[0], name.c_str(), size_ret);
        break;
    }
    //these properties of shape are unsupported
    case RPR_SHAPE_LINEAR_MOTION:
    case RPR_SHAPE_ANGULAR_MOTION:
    case RPR_SHAPE_VISIBILITY_FLAG:
    case RPR_SHAPE_SHADOW_FLAG:
    case RPR_SHAPE_SHADOW_CATCHER_FLAG:
    case RPR_SHAPE_SUBDIVISION_FACTOR:
    case RPR_SHAPE_SUBDIVISION_CREASEWEIGHT:
    case RPR_SHAPE_SUBDIVISION_BOUNDARYINTEROP:
    case RPR_SHAPE_DISPLACEMENT_SCALE:
    case RPR_SHAPE_OBJECT_GROUP_ID:
    case RPR_SHAPE_VIDMEM_USAGE:
    case RPR_SHAPE_VISIBILITY_PRIMARY_ONLY_FLAG:
    case RPR_SHAPE_VISIBILITY_IN_SPECULAR_FLAG:
    case RPR_SHAPE_VOLUME_MATERIAL:
    case RPR_SHAPE_DISPLACEMENT_MATERIAL:
    case RPR_SHAPE_MATERIALS_PER_FACE:
        UNSUPPORTED_FUNCTION
    default:
        UNIMLEMENTED_FUNCTION
    }

    if (in_size_ret)
    {
        *in_size_ret = size_ret;
    }
    if (in_data)
    {
        memcpy(in_data, &data[0], size_ret);
    }
    return RPR_SUCCESS;
}

rpr_int rprMeshGetInfo(rpr_shape in_mesh, rpr_mesh_info in_mesh_info, size_t in_size, void * in_data, size_t * in_size_ret)
{
    ShapeObject* mesh = WrapObject::Cast<ShapeObject>(in_mesh);
    if (!mesh || mesh->IsInstance())
    {
        return RPR_ERROR_INVALID_OBJECT;
    }

    std::vector<char> data;
    size_t size_ret = 0;
    switch (in_mesh_info)
    {
    case RPR_MESH_POLYGON_COUNT:
    {
        uint64_t value = mesh->GetVertexCount() / 3;
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_MESH_VERTEX_COUNT:
    {
        uint64_t value = mesh->GetVertexCount();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_MESH_NORMAL_COUNT:
    {
        uint64_t value = mesh->GetNormalCount();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_MESH_UV_COUNT:
    {
        uint64_t value = mesh->GetUVCount();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_MESH_UV2_COUNT:
    {
        //UV2 unsupported
        uint64_t value = 0;
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_MESH_VERTEX_ARRAY:
    {
        size_ret = sizeof(float) * 3 * mesh->GetVertexCount();
        data.resize(size_ret);
        mesh->GetVertexData(reinterpret_cast<float*>(data.data()));
        break;
    }
    case RPR_MESH_NORMAL_ARRAY:
    {
        size_ret = sizeof(float) * 3 * mesh->GetNormalCount();
        data.resize(size_ret);
        mesh->GetNormalData(reinterpret_cast<float*>(data.data()));
        break;

    }
    case RPR_MESH_UV_ARRAY:
    {
        const RadeonRays::float2* value = mesh->GetUVData();
        size_ret = sizeof(RadeonRays::float2) * mesh->GetUVCount();
        data.resize(size_ret);
        memcpy(&data[0], value, size_ret);
        break;
    }
    case RPR_MESH_VERTEX_INDEX_ARRAY:
    case RPR_MESH_UV_INDEX_ARRAY:
    case RPR_MESH_NORMAL_INDEX_ARRAY:
    {
        const uint32_t* value = mesh->GetIndicesData();
        size_ret = sizeof(uint32_t) * mesh->GetIndicesCount();
        data.resize(size_ret);
        memcpy(&data[0], value, size_ret);
        break;
    }
    case RPR_MESH_NUM_FACE_VERTICES_ARRAY:
    {
        //only triangles used in Baikal mesh
        std::vector<int32_t> value(mesh->GetIndicesCount() / 3, 3);
        size_ret = sizeof(int32_t) * value.size();
        data.resize(size_ret);
        memcpy(&data[0], value.data(), size_ret);
        break;
    }
    default:
        UNIMLEMENTED_FUNCTION
    }

    if (in_size_ret)
    {
        *in_size_ret = size_ret;
    }
    if (in_data)
    {
        memcpy(in_data, &data[0], size_ret);
    }
    return RPR_SUCCESS;
}

rpr_int rprMeshPolygonGetInfo(rpr_shape mesh, size_t polygon_index, rpr_mesh_polygon_info polygon_info, size_t size, void * data, size_t * size_ret)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprInstanceGetBaseShape(rpr_shape in_shape, rpr_shape * out_shape)
{
    ShapeObject* instance = WrapObject::Cast<ShapeObject>(in_shape);
    if (!instance || !instance->IsInstance())
    {
        return RPR_ERROR_INVALID_OBJECT;
    }

    *out_shape = instance->GetBaseShape();
    return RPR_SUCCESS;
}

rpr_int rprContextCreatePointLight(rpr_context in_context, rpr_light * out_light)
{
    //cast
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);

    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }
    if (!out_light)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    *out_light = context->CreateLight(LightObject::Type::kPointLight);

    return RPR_SUCCESS;
}

rpr_int rprPointLightSetRadiantPower3f(rpr_light in_light, rpr_float in_r, rpr_float in_g, rpr_float in_b)
{
    //cast
    LightObject* light = WrapObject::Cast<LightObject>(in_light);
    if (!light || light->GetType() != LightObject::Type::kPointLight)
    {
        return RPR_ERROR_INVALID_LIGHT;
    }

    RadeonRays::float3 radiant_power = { in_r, in_g, in_b };
    light->SetRadiantPower(radiant_power);

    return RPR_SUCCESS;
}

rpr_int rprContextCreateSpotLight(rpr_context in_context, rpr_light * out_light)
{
    //cast
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }
    if (!out_light)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    *out_light = context->CreateLight(LightObject::Type::kSpotLight);

    return RPR_SUCCESS;
}

rpr_int rprSpotLightSetRadiantPower3f(rpr_light in_light, rpr_float in_r, rpr_float in_g, rpr_float in_b)
{
    //cast
    LightObject* light = WrapObject::Cast<LightObject>(in_light);
    if (!light || light->GetType() != LightObject::Type::kSpotLight)
    {
        return RPR_ERROR_INVALID_LIGHT;
    }

    RadeonRays::float3 radiant_power = { in_r, in_g, in_b };
    light->SetRadiantPower(radiant_power);

    return RPR_SUCCESS;
}

rpr_int rprSpotLightSetConeShape(rpr_light in_light, rpr_float in_iangle, rpr_float in_oangle)
{
    //cast
    LightObject* light = WrapObject::Cast<LightObject>(in_light);
    if (!light || light->GetType() != LightObject::Type::kSpotLight)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //set angles
    RadeonRays::float2 angle = {in_iangle, in_oangle};
    rpr_int result = RPR_SUCCESS;
    try
    {
        light->SetSpotConeShape(angle);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }
    return result;
}

rpr_int rprContextCreateDirectionalLight(rpr_context in_context, rpr_light * out_light)
{
    //cast
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }
    if (!out_light)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    *out_light = context->CreateLight(LightObject::Type::kDirectionalLight);

    return RPR_SUCCESS;
}

rpr_int rprDirectionalLightSetRadiantPower3f(rpr_light in_light, rpr_float in_r, rpr_float in_g, rpr_float in_b)
{
    //cast
    LightObject* light = WrapObject::Cast<LightObject>(in_light);
    if (!light || light->GetType() != LightObject::Type::kDirectionalLight)
    {
        return RPR_ERROR_INVALID_LIGHT;
    }

    RadeonRays::float3 radiant_power = { in_r, in_g, in_b };
    light->SetRadiantPower(radiant_power);

    return RPR_SUCCESS;
}

rpr_int rprDirectionalLightSetShadowSoftness(rpr_light in_light, rpr_float in_coeff)
{
    //cast
    LightObject* light = WrapObject::Cast<LightObject>(in_light);
    if (!light || light->GetType() != LightObject::Type::kDirectionalLight)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    UNIMLEMENTED_FUNCTION
}

rpr_int rprContextCreateEnvironmentLight(rpr_context in_context, rpr_light * out_light)
{
    //cast
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }
    if (!out_light)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    *out_light = context->CreateLight(LightObject::Type::kEnvironmentLight);

    return RPR_SUCCESS;
}

rpr_int rprEnvironmentLightSetImage(rpr_light in_env_light, rpr_image in_image)
{
    //cast
    LightObject* light = WrapObject::Cast<LightObject>(in_env_light);
    MaterialObject* img = WrapObject::Cast<MaterialObject>(in_image);
    if (!light || light->GetType() != LightObject::Type::kEnvironmentLight || !img || !img->IsImg())
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    rpr_int result = RPR_SUCCESS;
    try
    {
        //set image
        light->SetEnvTexture(img);
    }
    catch (Exception& e)
    {
        result = e.m_error;
    }
    return result;
}

rpr_int rprEnvironmentLightSetIntensityScale(rpr_light in_env_light, rpr_float intensity_scale)
{
    //cast
    LightObject* light = WrapObject::Cast<LightObject>(in_env_light);
    if (!light || light->GetType() != LightObject::Type::kEnvironmentLight)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }
    
    //set data
    light->SetEnvMultiplier(intensity_scale);

    return RPR_SUCCESS;
}

rpr_int rprEnvironmentLightAttachPortal(rpr_scene scene, rpr_light env_light, rpr_shape portal)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprEnvironmentLightDetachPortal(rpr_scene scene, rpr_light env_light, rpr_shape portal)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprEnvironmentLightAttachPortal(rpr_light env_light, rpr_shape portal)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprEnvironmentLightDetachPortal(rpr_light env_light, rpr_shape portal)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprContextCreateSkyLight(rpr_context context, rpr_light * out_light)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprSkyLightSetTurbidity(rpr_light skylight, rpr_float turbidity)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprSkyLightSetAlbedo(rpr_light skylight, rpr_float albedo)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprSkyLightSetScale(rpr_light skylight, rpr_float scale)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprSkyLightAttachPortal(rpr_scene scene, rpr_light skylight, rpr_shape portal)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprSkyLightDetachPortal(rpr_scene scene, rpr_light skylight, rpr_shape portal)
{
    UNIMLEMENTED_FUNCTION
}


rpr_int rprSkyLightAttachPortal(rpr_light skylight, rpr_shape portal)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprSkyLightDetachPortal(rpr_light skylight, rpr_shape portal)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprContextCreateIESLight(rpr_context context, rpr_light * light)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprIESLightSetRadiantPower3f(rpr_light light, rpr_float r, rpr_float g, rpr_float b)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprIESLightSetImageFromFile(rpr_light env_light, rpr_char const * imagePath, rpr_int nx, rpr_int ny)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprIESLightSetImageFromIESdata(rpr_light env_light, rpr_char const * iesData, rpr_int nx, rpr_int ny)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprLightGetInfo(rpr_light in_light, rpr_light_info in_info, size_t in_size, void * out_data, size_t * out_size_ret)
{
    //cast
    LightObject* light = WrapObject::Cast<LightObject>(in_light);
    if (!light)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    std::vector<char> data;
    size_t size_ret = 0;
    switch (in_info)
    {
    case RPR_LIGHT_TYPE:
    {
        rpr_light_type value = (rpr_light_type)light->GetType();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_LIGHT_TRANSFORM:
    {
        RadeonRays::matrix value = light->GetTransform();
        value = value.transpose();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_ENVIRONMENT_LIGHT_IMAGE:
    {
        MaterialObject* value = light->GetEnvTexture();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_POINT_LIGHT_RADIANT_POWER:
    case RPR_SPOT_LIGHT_RADIANT_POWER:
    case RPR_DIRECTIONAL_LIGHT_RADIANT_POWER:
    {
        RadeonRays::float3 value = light->GetRadiantPower();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_SPOT_LIGHT_CONE_SHAPE:
    {
        RadeonRays::float2 value = light->GetSpotConeShape();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_ENVIRONMENT_LIGHT_INTENSITY_SCALE:
    {
        rpr_float value = light->GetEnvMultiplier();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_OBJECT_NAME:
    {
        std::string name = light->GetName();
        size_ret = name.size() + 1;
        data.resize(size_ret);
        memcpy(&data[0], name.c_str(), size_ret);
        break;
    }
    case RPR_DIRECTIONAL_LIGHT_SHADOW_SOFTNESS:
    case RPR_ENVIRONMENT_LIGHT_PORTAL_COUNT:
    case RPR_ENVIRONMENT_LIGHT_PORTAL_LIST:
    case RPR_SKY_LIGHT_SCALE:
    case RPR_SKY_LIGHT_ALBEDO:
    case RPR_SKY_LIGHT_TURBIDITY:
    case RPR_SKY_LIGHT_PORTAL_COUNT:
    case RPR_SKY_LIGHT_PORTAL_LIST:
    case RPR_IES_LIGHT_RADIANT_POWER:
    case RPR_IES_LIGHT_IMAGE_DESC:        
        UNSUPPORTED_FUNCTION
        break;
    default:
        return RPR_ERROR_INVALID_PARAMETER;
    }

    if (out_size_ret)
    {
        *out_size_ret = size_ret;
    }
    if (out_data)
    {
        memcpy(out_data, &data[0], size_ret);
    }

    return RPR_SUCCESS;
}

rpr_int rprSceneClear(rpr_scene in_scene)
{
    //cast
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);
    if (!scene)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    scene->Clear();
    return RPR_SUCCESS;
}

rpr_int rprSceneAttachShape(rpr_scene in_scene, rpr_shape in_shape)
{
    //cast
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);
    ShapeObject* shape = WrapObject::Cast<ShapeObject>(in_shape);
    if (!scene || !shape)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //cast input data
    scene->AttachShape(shape);

    return RPR_SUCCESS;
}

rpr_int rprSceneDetachShape(rpr_scene in_scene, rpr_shape in_shape)
{
    //cast
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);
    ShapeObject* shape = WrapObject::Cast<ShapeObject>(in_shape);
    if (!scene || !shape)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //cast input data
    scene->DetachShape(shape);

    return RPR_SUCCESS;
}

rpr_int rprSceneAttachHeteroVolume(rpr_scene scene, rpr_hetero_volume heteroVolume)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprSceneDetachHeteroVolume(rpr_scene scene, rpr_hetero_volume heteroVolume)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprSceneAttachLight(rpr_scene in_scene, rpr_light in_light)
{
    //cast
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);
    LightObject* light = WrapObject::Cast<LightObject>(in_light);
    if (!scene || !light)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //attach
    scene->AttachLight(light);

    return RPR_SUCCESS;
}

rpr_int rprSceneDetachLight(rpr_scene in_scene, rpr_light in_light)
{
    //cast
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);
    LightObject* light = WrapObject::Cast<LightObject>(in_light);
    if (!scene || !light)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //attach
    scene->DetachLight(light);

    return RPR_SUCCESS;
}

rpr_int rprSceneGetInfo(rpr_scene in_scene, rpr_scene_info in_info, size_t in_size, void * out_data, size_t * out_size_ret)
{
    //cast
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);
    if (!scene)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }
    
    std::vector<char> data;
    size_t size_ret = 0;
    switch (in_info)
    {
    case RPR_SCENE_SHAPE_COUNT:
    {
        size_t value = scene->GetShapeCount();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_SCENE_SHAPE_LIST:
    {
        size_t value = scene->GetShapeCount();
        size_ret = sizeof(rpr_shape) * value;
        data.resize(size_ret);
        scene->GetShapeList(data.data());
        break;
    }
    case RPR_SCENE_LIGHT_COUNT:
    {
        size_t value = scene->GetLightCount();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_SCENE_LIGHT_LIST:
    {
        size_t value = scene->GetLightCount();
        size_ret = sizeof(rpr_light) * value;
        data.resize(size_ret);
        scene->GetLightList(data.data());
        break;
    }
    case RPR_SCENE_CAMERA:
    {
        rpr_camera value = scene->GetCamera();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_OBJECT_NAME:
    {
        std::string name = scene->GetName();
        size_ret = name.size() + 1;
        data.resize(size_ret);
        memcpy(&data[0], name.c_str(), size_ret);
        break;
    }
    case RPR_SCENE_AABB:
    {
        RadeonRays::bbox bb = scene->GetBBox();
        size_ret = 6 * sizeof(float);
        data.resize(size_ret);
        memcpy(&data[0], &bb.pmin.x, size_ret/2); // copy pmin
        memcpy(data.data() + size_ret/2, &bb.pmax.x, size_ret/2); //copy pmax

        break;
    }
    case RPR_SCENE_BACKGROUND_IMAGE:
    case RPR_SCENE_ENVIRONMENT_OVERRIDE_REFLECTION:
    case RPR_SCENE_ENVIRONMENT_OVERRIDE_REFRACTION:
    case RPR_SCENE_ENVIRONMENT_OVERRIDE_TRANSPARENCY:
    case RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND:

        UNSUPPORTED_FUNCTION
    default:
        UNIMLEMENTED_FUNCTION
    }

    if (out_size_ret)
    {
        *out_size_ret = size_ret;
    }
    if (out_data)
    {
        memcpy(out_data, &data[0], size_ret);
    }

    return RPR_SUCCESS;
}

rpr_int rprSceneGetEnvironmentOverride(rpr_scene in_scene, rpr_environment_override overrride, rpr_light * out_light)
{
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);

    if (!scene)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    static const std::map<rpr_environment_override, SceneObject::OverrideType> override_type_conversion =
    {
        { RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND, SceneObject::OverrideType::kBackground },
        { RPR_SCENE_ENVIRONMENT_OVERRIDE_REFLECTION, SceneObject::OverrideType::kReflection },
        { RPR_SCENE_ENVIRONMENT_OVERRIDE_REFRACTION, SceneObject::OverrideType::kRefraction },
        { RPR_SCENE_ENVIRONMENT_OVERRIDE_TRANSPARENCY, SceneObject::OverrideType::kTransparency }
    };

    auto overryde_type = override_type_conversion.find(overrride);
    if (overryde_type == override_type_conversion.end())
        return RPR_ERROR_INVALID_PARAMETER;

    try
    {
        *out_light = scene->GetEnvironmentOverride(overryde_type->second);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }

    return RPR_SUCCESS;
}

rpr_int rprSceneSetEnvironmentOverride(rpr_scene in_scene, rpr_environment_override overrride, rpr_light in_light)
{
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);
    LightObject* light = WrapObject::Cast<LightObject>(in_light);

    if (!scene)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    static const std::map<rpr_environment_override, SceneObject::OverrideType> override_type_conversion =
    {
        { RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND, SceneObject::OverrideType::kBackground },
        { RPR_SCENE_ENVIRONMENT_OVERRIDE_REFLECTION, SceneObject::OverrideType::kReflection },
        { RPR_SCENE_ENVIRONMENT_OVERRIDE_REFRACTION, SceneObject::OverrideType::kRefraction },
        { RPR_SCENE_ENVIRONMENT_OVERRIDE_TRANSPARENCY, SceneObject::OverrideType::kTransparency }
    };

    auto override_type = override_type_conversion.find(overrride);
    if (override_type  == override_type_conversion.end())
        return RPR_ERROR_INVALID_PARAMETER;

    try
    {
        if ((overrride == RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND) && light)
        {
            rprSceneSetBackgroundImage(scene, light->GetEnvTexture());
        }
        else
        {
            scene->SetEnvironmentOverride(override_type->second, light);
        }
    }
    catch (Exception& e)
    {
        return e.m_error;
    }

    return RPR_SUCCESS;
}

rpr_int rprSceneSetBackgroundImage(rpr_scene in_scene, rpr_image in_image)
{
    MaterialObject* img = WrapObject::Cast<MaterialObject>(in_image);
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);
    if (!scene)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    try
    {
        if (img)
            rprSceneSetEnvironmentOverride(scene, RPR_SCENE_ENVIRONMENT_OVERRIDE_BACKGROUND, nullptr);
        scene->SetBackgroundImage(img);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }
    
    return RPR_SUCCESS;
}

rpr_int rprSceneGetBackgroundImage(rpr_scene in_scene, rpr_image * out_image)
{
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);

    if (!scene || !out_image)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    try
    {
        *out_image = scene->GetBackgroundImage();
    }
    catch (Exception& e)
    {
        return e.m_error;
    }

    return RPR_SUCCESS;
}

rpr_int rprSceneSetCamera(rpr_scene in_scene, rpr_camera in_camera)
{
    //cast
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);
    CameraObject* camera = WrapObject::Cast<CameraObject>(in_camera);
    if (!scene)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    scene->SetCamera(camera);

    return RPR_SUCCESS;
}

rpr_int rprSceneGetCamera(rpr_scene in_scene, rpr_camera * out_camera)
{
    //cast
    SceneObject* scene = WrapObject::Cast<SceneObject>(in_scene);
    if (!scene || !out_camera)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    *out_camera = scene->GetCamera();

    return RPR_SUCCESS;
}

rpr_int rprFrameBufferGetInfo(rpr_framebuffer in_frame_buffer, rpr_framebuffer_info in_info, size_t in_size, void * out_data, size_t * out_size)
{
    //cast
    FramebufferObject* buff = WrapObject::Cast<FramebufferObject>(in_frame_buffer);
    if (!buff)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }
    std::size_t buff_size = sizeof(RadeonRays::float3) * buff->Width() * buff->Height();
    switch (in_info)
    {
    case RPR_FRAMEBUFFER_DATA:
        if (out_size)
        {
            *out_size = buff_size;
        }
        if (out_data && in_size < buff_size)
        {
            return RPR_ERROR_INVALID_PARAMETER;
        }
        if (out_data)
        {
            buff->GetData(static_cast<RadeonRays::float3*>(out_data));
        }
        break;
    default:
        UNIMLEMENTED_FUNCTION
    }

    return RPR_SUCCESS;
}

rpr_int rprFrameBufferClear(rpr_framebuffer in_frame_buffer)
{
    //cast
    FramebufferObject* buff = WrapObject::Cast<FramebufferObject>(in_frame_buffer);
    if (!buff)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    buff->Clear();

    return RPR_SUCCESS;
}

rpr_int rprFrameBufferSaveToFile(rpr_framebuffer in_frame_buffer, rpr_char const * file_path)
{
    //cast
    FramebufferObject* buff = WrapObject::Cast<FramebufferObject>(in_frame_buffer);
    if (!buff)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    try
    {
        buff->SaveToFile(file_path);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }
    return RPR_SUCCESS;
}

rpr_int rprContextResolveFrameBuffer(rpr_context context, rpr_framebuffer src_frame_buffer, rpr_framebuffer dst_frame_buffer, rpr_bool normalizeOnly)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprContextCreateMaterialSystem(rpr_context in_context, rpr_material_system_type type, rpr_material_system * out_matsys)
{
    //cast
    ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
    if (!context)
    {
        return RPR_ERROR_INVALID_CONTEXT;
    }

    *out_matsys = context->CreateMaterialSystem();

    return RPR_SUCCESS;
}

rpr_int rprMaterialSystemGetSize(rpr_context in_context, rpr_uint * out_size)
{
    UNIMLEMENTED_FUNCTION
}


rpr_int rprMaterialSystemCreateNode(rpr_material_system in_matsys, rpr_material_node_type in_type, rpr_material_node * out_node)
{
    //cast
    MatSysObject* sys = WrapObject::Cast<MatSysObject>(in_matsys);
    if (!sys)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }
    
    //create material
    try
    {
        *out_node = sys->CreateMaterial(in_type);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }
    return RPR_SUCCESS;
}

rpr_int rprMaterialNodeSetInputN(rpr_material_node in_node, rpr_char const * in_input, rpr_material_node in_input_node)
{
    //cast
    MaterialObject* mat = WrapObject::Cast<MaterialObject>(in_node);
    MaterialObject* input_node = WrapObject::Cast<MaterialObject>(in_input_node);

    if (!mat || !in_input || !input_node)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    try
    {
        mat->SetInputValue(in_input, input_node);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }
    return RPR_SUCCESS;
}

rpr_int rprMaterialNodeSetInputF(rpr_material_node in_node, rpr_char const * in_input, rpr_float in_value_x, rpr_float in_value_y, rpr_float in_value_z, rpr_float in_value_w)
{
    //cast
    MaterialObject* mat = WrapObject::Cast<MaterialObject>(in_node);
    if (!mat)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }
    //collect input
    RadeonRays::float4 input = { in_value_x, in_value_y, in_value_z, in_value_w };
    
    try
    {
        mat->SetInputValue(in_input, input);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }
    return RPR_SUCCESS;
}

rpr_int rprMaterialNodeSetInputU(rpr_material_node in_node, rpr_char const * in_input, rpr_uint in_value)
{
    //cast
    MaterialObject* mat = WrapObject::Cast<MaterialObject>(in_node);
    if (!mat)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    try
    {
        mat->SetInputValue(in_input, in_value);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }
    return RPR_SUCCESS;
}

rpr_int rprMaterialNodeSetInputImageData(rpr_material_node in_node, rpr_char const * in_input, rpr_image in_image)
{
    //cast
    MaterialObject* mat = WrapObject::Cast<MaterialObject>(in_node);
    MaterialObject* img = WrapObject::Cast<MaterialObject>(in_image);
    if (!mat || !img || !img->IsImg())
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }
    
    try
    {
        mat->SetInputValue(in_input, img);
    }
    catch (Exception& e)
    {
        return e.m_error;
    }

    return RPR_SUCCESS;
}

rpr_int rprMaterialNodeSetInputBufferData(rpr_material_node in_node, rpr_char const * in_input, rpr_buffer buffer)
{
    UNIMLEMENTED_FUNCTION
}


rpr_int rprMaterialNodeGetInfo(rpr_material_node in_node, rpr_material_node_info in_info, size_t in_size, void * out_data, size_t * out_size)
{
    MaterialObject* mat = WrapObject::Cast<MaterialObject>(in_node);
    if (!mat)
    {
        return RPR_ERROR_INVALID_OBJECT;
    }

    std::vector<char> data;
    size_t size_ret = 0;
    switch (in_info)
    {
    case RPR_MATERIAL_NODE_TYPE:
    {
        rpr_material_node_type value = mat->GetType();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_MATERIAL_NODE_INPUT_COUNT:
    {
        uint64_t value = mat->GetInputCount();
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_OBJECT_NAME:
    {
        std::string name = mat->GetName();
        size_ret = name.size() + 1;
        data.resize(size_ret);
        memcpy(&data[0], name.c_str(), size_ret);
        break;
    }
    default:
        UNIMLEMENTED_FUNCTION
    }

    if (out_size)
    {
        *out_size = size_ret;
    }
    if (out_data)
    {
        memcpy(out_data, &data[0], size_ret);
    }
    return RPR_SUCCESS;
}

rpr_int rprMaterialNodeGetInputInfo(rpr_material_node in_node, rpr_int in_input_idx, rpr_material_node_input_info in_info, size_t in_size, void * in_data, size_t * in_out_size)
{
    MaterialObject* mat = WrapObject::Cast<MaterialObject>(in_node);
    if (!mat)
    {
        return RPR_ERROR_INVALID_OBJECT;
    }

    std::vector<char> data;
    size_t size_ret = 0;
    switch (in_info)
    {
    case RPR_MATERIAL_NODE_INPUT_NAME_STRING:
    {
        std::string name = mat->GetInputName(in_input_idx);
        size_ret = name.size() + 1;
        data.resize(size_ret);
        memcpy(&data[0], name.data(), size_ret);
        break;
    }
    case RPR_MATERIAL_NODE_INPUT_TYPE:
    {
        rpr_uint value = mat->GetInputType(in_input_idx);
        size_ret = sizeof(value);
        data.resize(size_ret);
        memcpy(&data[0], &value, size_ret);
        break;
    }
    case RPR_MATERIAL_NODE_INPUT_VALUE:
    {
        //sizeof(RadeonRays::float4) should be enough to store any input data
        data.resize(sizeof(RadeonRays::float4));
        mat->GetInput(in_input_idx, data.data(), &size_ret);
        break;
    }
    default:
        UNIMLEMENTED_FUNCTION
    }

    if (in_out_size)
    {
        *in_out_size = size_ret;
    }
    if (in_data)
    {
        memcpy(in_data, &data[0], size_ret);
    }
    return RPR_SUCCESS;
}

rpr_int rprContextCreateComposite(rpr_context context, rpr_composite_type in_type, rpr_composite * out_composite)
{
    UNIMLEMENTED_FUNCTION
}
rpr_int rprCompositeSetInputFb(rpr_composite composite, const char * inputName, rpr_framebuffer input)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprCompositeSetInputC(rpr_composite composite, const char * inputName, rpr_composite input)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprCompositeSetInput4f(rpr_composite composite, const char * inputName, float x, float y, float z, float w)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprCompositeSetInput1u(rpr_composite composite, const char * inputName, unsigned int value)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprCompositeSetInputOp(rpr_composite composite, const char * inputName, rpr_material_node_arithmetic_operation op)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprCompositeCompute(rpr_composite composite, rpr_framebuffer fb)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprCompositeGetInfo(rpr_composite composite, rpr_composite_info composite_info, size_t size, void *  data, size_t * size_ret)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprObjectDelete(void * in_obj)
{
    WrapObject* obj = static_cast<WrapObject*>(in_obj);
    delete obj;

    return RPR_SUCCESS;
}

rpr_int rprObjectSetName(void * in_node, rpr_char const * in_name)
{
    //check nullptr
    if (!in_node)
    {
        return RPR_ERROR_INVALID_OBJECT;
    }

    if (!in_name)
    {
        return RPR_ERROR_INVALID_PARAMETER;
    }

    //set name
    WrapObject* node = WrapObject::Cast<WrapObject>(in_node);
    node->SetName(in_name);

    return RPR_SUCCESS;
}

rpr_int rprContextCreatePostEffect(rpr_context in_context, rpr_post_effect_type type, rpr_post_effect * out_effect)
{
	//cast
	ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
	if (!context)
	{
		return RPR_ERROR_INVALID_CONTEXT;
	}
	if (!out_effect)
	{
		return RPR_ERROR_INVALID_PARAMETER;
	}

	*out_effect = context->CreatePostEffect(type);

	return RPR_SUCCESS;
}

rpr_int rprContextAttachPostEffect(rpr_context in_context, rpr_post_effect in_effect)
{
	//cast
	ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
	if (!context)
	{
		return RPR_ERROR_INVALID_CONTEXT;
	}
	if (!in_effect)
	{
		return RPR_ERROR_INVALID_PARAMETER;
	}
	PostEffectObject* effect = WrapObject::Cast<PostEffectObject>(in_effect);
	if (!effect)
	{
		return RPR_ERROR_INVALID_PARAMETER;
	}

	context->AttachPostEffect(effect);

	return RPR_SUCCESS;
}

rpr_int rprContextDetachPostEffect(rpr_context in_context, rpr_post_effect in_effect)
{
	//cast
	ContextObject* context = WrapObject::Cast<ContextObject>(in_context);
	if (!context)
	{
		return RPR_ERROR_INVALID_CONTEXT;
	}
	if (!in_effect)
	{
		return RPR_ERROR_INVALID_PARAMETER;
	}
	PostEffectObject* effect = WrapObject::Cast<PostEffectObject>(in_effect);
	if (!effect)
	{
		return RPR_ERROR_INVALID_PARAMETER;
	}

	context->DetachPostEffect(effect);

	return RPR_SUCCESS;
}

rpr_int rprPostEffectSetParameter1u(rpr_post_effect effect, rpr_char const * name, rpr_uint x)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprPostEffectSetParameter1f(rpr_post_effect effect, rpr_char const * name, rpr_float x)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprPostEffectSetParameter3f(rpr_post_effect effect, rpr_char const * name, rpr_float x, rpr_float y, rpr_float z)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprPostEffectSetParameter4f(rpr_post_effect in_effect, rpr_char const * name, rpr_float x, rpr_float y, rpr_float z, rpr_float w)
{
	PostEffectObject* effect = WrapObject::Cast<PostEffectObject>(in_effect);
	if (!effect)
	{
		return RPR_ERROR_INVALID_PARAMETER;
	}

	effect->GetPostEffect()->SetParameter(name, RadeonRays::float4(x, y, z, w));
}

rpr_int rprContextGetAttachedPostEffectCount(rpr_context context, rpr_uint *  nb)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprContextGetAttachedPostEffect(rpr_context context, rpr_uint i, rpr_post_effect * out_effect)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprPostEffectGetInfo(rpr_post_effect effect, rpr_post_effect_info info, size_t size, void *  data, size_t *  size_ret)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprContextCreateHeteroVolume(rpr_context context, rpr_hetero_volume * out_heteroVolume, size_t gridSizeX, size_t gridSizeY, size_t gridSizeZ, void * indicesList, size_t numberOfIndices, rpr_hetero_volume_indices_topology indicesListTopology, void * gridData, size_t gridDataSizeByte, rpr_uint gridDataTopology___unused)
{
    UNIMLEMENTED_FUNCTION
}
rpr_int rprShapeSetHeteroVolume(rpr_shape shape, rpr_hetero_volume heteroVolume)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprHeteroVolumeSetTransform(rpr_hetero_volume out_heteroVolume, rpr_bool transpose, rpr_float const * transform)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprMaterialNodeSetInputN_ext(rpr_material_node in_node, rpr_material_node_input in_input, rpr_material_node in_input_node)
{
    auto name_it  = kRPRInputStrings.find(in_input);
    return name_it != kRPRInputStrings.end() ?
        rprMaterialNodeSetInputN(in_node, name_it->second.c_str(), in_input_node)
        : RPR_ERROR_UNSUPPORTED;
}
rpr_int rprMaterialNodeSetInputF_ext(rpr_material_node in_node, rpr_material_node_input in_input, rpr_float in_value_x, rpr_float in_value_y, rpr_float in_value_z, rpr_float in_value_w)
{
    auto name_it = kRPRInputStrings.find(in_input);
    return name_it != kRPRInputStrings.end() ? 
        rprMaterialNodeSetInputF(in_node, name_it->second.c_str(), in_value_x, in_value_y, in_value_z, in_value_w)
        : RPR_ERROR_UNSUPPORTED;
}
rpr_int rprMaterialNodeSetInputU_ext(rpr_material_node in_node, rpr_material_node_input in_input, rpr_uint in_value)
{
    auto name_it = kRPRInputStrings.find(in_input);
    return name_it != kRPRInputStrings.end() ?
        rprMaterialNodeSetInputU(in_node, name_it->second.c_str(), in_value)
        : RPR_ERROR_UNSUPPORTED;
}
rpr_int rprMaterialNodeSetInputImageData_ext(rpr_material_node in_node, rpr_material_node_input in_input, rpr_image image)
{
    auto name_it = kRPRInputStrings.find(in_input);
    return name_it != kRPRInputStrings.end() ?
        rprMaterialNodeSetInputImageData(in_node, name_it->second.c_str(), image)
        : RPR_ERROR_UNSUPPORTED;
}
rpr_int rprMaterialNodeSetInputBufferData_ext(rpr_material_node in_node, rpr_material_node_input in_input, rpr_buffer buffer)
{
    auto name_it = kRPRInputStrings.find(in_input);
    return name_it != kRPRInputStrings.end() ?
        rprMaterialNodeSetInputBufferData(in_node, name_it->second.c_str(), buffer)
        : RPR_ERROR_UNSUPPORTED;
}

rpr_int rprShapeSetLayerMask(rpr_shape shape, rpr_uint layerMask)
{
    UNIMLEMENTED_FUNCTION
}

rpr_int rprContextCreateHeteroVolume(rpr_context context, rpr_hetero_volume * out_heteroVolume, size_t gridSizeX, size_t gridSizeY, size_t gridSizeZ, void const * indicesList, size_t numberOfIndices, rpr_hetero_volume_indices_topology indicesListTopology, void const * gridData, size_t gridDataSizeByte, rpr_uint gridDataTopology___unused)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprImageSetGamma(rpr_image image, rpr_float type)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprImageSetMipmapEnabled(rpr_image image, rpr_bool enabled)
{
    UNSUPPORTED_FUNCTION
}

rpr_int rprImageSetFilter(rpr_image image, rpr_image_filter_type type)
{
    UNSUPPORTED_FUNCTION
}

RPR_API_ENTRY rpr_int rprHeteroVolumeSetFilter(rpr_hetero_volume heteroVolume, rpr_hetero_volume_filter filter)
{
    UNSUPPORTED_FUNCTION
}

RPR_API_ENTRY rpr_int rprHeteroVolumeSetEmission(rpr_hetero_volume heteroVolume, rpr_float r, rpr_float g, rpr_float b)
{
    UNSUPPORTED_FUNCTION
}

RPR_API_ENTRY rpr_int rprHeteroVolumeSetAlbedo(rpr_hetero_volume heteroVolume, rpr_float r, rpr_float g, rpr_float b)
{
    UNSUPPORTED_FUNCTION
}

RPR_API_ENTRY rpr_int rprHeteroVolumeGetInfo(rpr_hetero_volume heteroVol, rpr_hetero_volume_parameter heteroVol_info, size_t size, void * data, size_t * size_ret)
{
    UNSUPPORTED_FUNCTION
}

RPR_API_ENTRY rpr_int rprBufferGetInfo(rpr_buffer buffer, rpr_buffer_info buffer_info, size_t size, void * data, size_t * size_ret)
{
    UNSUPPORTED_FUNCTION
}
