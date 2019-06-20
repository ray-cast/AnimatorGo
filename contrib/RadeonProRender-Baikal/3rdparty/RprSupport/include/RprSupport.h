/*****************************************************************************\
*
*  Module Name    RprSupport.h
*  Project        RRP support library
*
*  Description    RRP support library interface header
*
*  Copyright 2017 Advanced Micro Devices, Inc.
*
*  All rights reserved.  This notice is intended as a precaution against
*  inadvertent publication and does not imply publication or any waiver
*  of confidentiality.  The year included in the foregoing notice is the
*  year of creation of the work.
\*****************************************************************************/
#ifndef __RPRSUPPORT_H
#define __RPRSUPPORT_H

#include "RadeonProRender.h"

#ifdef __cplusplus
extern "C" {
#endif

#define RPRX_SUPPORT_API_VERSION 0x0000002 
#define RPRX_FLAGS_ENABLE_LOGGING 0x1 

/*rprx_material_type*/
#define RPRX_MATERIAL_UBER 0x1 
#define RPRX_MATERIAL_ARCHITECTURAL 0x2 
#define RPRX_MATERIAL_SKIN 0x3 
#define RPRX_MATERIAL_CARPAINT 0x4 

typedef rpr_uint rprx_material_type;
typedef rpr_uint rprx_parameter;
typedef rpr_uint rprx_parameter_type;

struct _rprxcontext
{
    rpr_uint padding;
};
struct _rprxmaterial
{
    rpr_uint padding;
};

typedef _rprxcontext *rprx_context;
typedef _rprxmaterial *rprx_material;

/*rprx_material_type*/
#define RPRX_UBER_MATERIAL_DIFFUSE_COLOR 0x100U 
#define RPRX_UBER_MATERIAL_DIFFUSE_WEIGHT 0x101U 
#define RPRX_UBER_MATERIAL_DIFFUSE_ROUGHNESS 0x102U 
#define RPRX_UBER_MATERIAL_REFLECTION_COLOR 0x103U 
#define RPRX_UBER_MATERIAL_REFLECTION_WEIGHT 0x104U 
#define RPRX_UBER_MATERIAL_REFLECTION_ROUGHNESS 0x105U 
#define RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY 0x106U 
#define RPRX_UBER_MATERIAL_REFLECTION_ANISOTROPY_ROTATION 0x107U 
#define RPRX_UBER_MATERIAL_REFLECTION_MODE 0x108U 
#define RPRX_UBER_MATERIAL_REFLECTION_IOR 0x109U 
#define RPRX_UBER_MATERIAL_REFLECTION_METALNESS 0x109U 
#define RPRX_UBER_MATERIAL_REFRACTION_COLOR 0x10AU 
#define RPRX_UBER_MATERIAL_REFRACTION_WEIGHT 0x10BU 
#define RPRX_UBER_MATERIAL_REFRACTION_ROUGHNESS 0x10CU 
#define RPRX_UBER_MATERIAL_REFRACTION_IOR 0x10DU 
#define RPRX_UBER_MATERIAL_REFRACTION_IOR_MODE 0x10EU 
#define RPRX_UBER_MATERIAL_REFRACTION_THIN_SURFACE 0x10FU 
#define RPRX_UBER_MATERIAL_COATING_COLOR 0x110U 
#define RPRX_UBER_MATERIAL_COATING_WEIGHT 0x111U 
#define RPRX_UBER_MATERIAL_COATING_ROUGHNESS 0x112U 
#define RPRX_UBER_MATERIAL_COATING_MODE 0x113U 
#define RPRX_UBER_MATERIAL_COATING_IOR 0x114U 
#define RPRX_UBER_MATERIAL_COATING_METALNESS 0x114U 
#define RPRX_UBER_MATERIAL_EMISSION_COLOR 0x115U 
#define RPRX_UBER_MATERIAL_EMISSION_WEIGHT 0x116U 
#define RPRX_UBER_MATERIAL_EMISSION_MODE 0x117U 
#define RPRX_UBER_MATERIAL_TRANSPARENCY 0x118U 
#define RPRX_UBER_MATERIAL_NORMAL 0x119U 
#define RPRX_UBER_MATERIAL_BUMP 0x11AU 
#define RPRX_UBER_MATERIAL_DISPLACEMENT 0x11BU 
#define RPRX_UBER_MATERIAL_SSS_ABSORPTION_COLOR 0x11CU 
#define RPRX_UBER_MATERIAL_SSS_SCATTER_COLOR 0x11DU 
#define RPRX_UBER_MATERIAL_SSS_ABSORPTION_DISTANCE 0x11EU 
#define RPRX_UBER_MATERIAL_SSS_SCATTER_DISTANCE 0x11FU 
#define RPRX_UBER_MATERIAL_SSS_SCATTER_DIRECTION 0x120U 
#define RPRX_UBER_MATERIAL_SSS_WEIGHT 0x121U 
#define RPRX_UBER_MATERIAL_SSS_SUBSURFACE_COLOR 0x122U 
#define RPRX_UBER_MATERIAL_SSS_MULTISCATTER 0x123U 

#define RPRX_UBER_MATERIAL_REFLECTION_MODE_PBR 0x1U 
#define RPRX_UBER_MATERIAL_REFLECTION_MODE_METALNESS 0x2U 
#define RPRX_UBER_MATERIAL_REFRACTION_MODE_SEPARATE 0x1U 
#define RPRX_UBER_MATERIAL_REFRACTION_MODE_LINKED 0x2U 
#define RPRX_UBER_MATERIAL_COATING_MODE_PBR 0x1U 
#define RPRX_UBER_MATERIAL_COATING_MODE_METALNESS 0x2U 
#define RPRX_UBER_MATERIAL_EMISSION_MODE_SINGLESIDED 0x1U 
#define RPRX_UBER_MATERIAL_EMISSION_MODE_DOUBLESIDED 0x2U 

/*rprx_parameter_type*/
#define RPRX_PARAMETER_TYPE_FLOAT4 0x1U 
#define RPRX_PARAMETER_TYPE_UINT 0x2U 
#define RPRX_PARAMETER_TYPE_NODE 0x3U 

/** @brief Create RPRX context.
    *
    *   RPR contexts allow to track several RPR contexts and material systems at the same time.
    *
    *  @param material_system   Material system to use
    *  @param flags             Additional options (currently only RPRX_FLAGS_ENABLE_LOGGING supported)
    *  @param out_context       Context
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxCreateContext(rpr_material_system material_system, rpr_uint flags, rprx_context* out_context);

/** @brief Create RPRX material
    *
    *   The function creates a material of a specified type. Supported types:
    *       RPRX_MATERIAL_UBER
    *
    *  @param context           RPRX context
    *  @param type              Material type
    *  @param out_material      Pointer to newly created material
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxCreateMaterial(rprx_context context, rprx_material_type type, rprx_material* out_material);

/** @brief Delete RPRX material
    *
    *   The function deletes the material passed in. The function also remove material from 
    *   the set of tracked materials.
	*
	*   Warning : this function will manipulate all the rpr_shape objects attached to this material (with rprxShapeAttachMaterial)
	*             So do not call rprObjectDelete(shape) before calling rprxMaterialDelete.
	*             Or call rprxShapeDetachMaterial on all the shapes using material before calling rprxMaterialDelete.
	*
    *  @param context           RPRX context
    *  @param material          Material to delete
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxMaterialDelete(rprx_context context, rprx_material material);

/** @brief Set the parameter for a material
    *
    *   The function sets specified parameter for a given material.
    *
    *  @param context           RPRX context
    *  @param material          Material
    *  @param parameter         parameter Parameter to set
    *  @param node              Input node
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxMaterialSetParameterN(rprx_context context, rprx_material material, rprx_parameter parameter, rpr_material_node node);
extern RPR_API_ENTRY rpr_int rprxMaterialSetParameterU(rprx_context context, rprx_material material, rprx_parameter parameter, rpr_uint value);
extern RPR_API_ENTRY rpr_int rprxMaterialSetParameterF(rprx_context context, rprx_material material, rprx_parameter parameter, rpr_float x, rpr_float y, rpr_float z, rpr_float w);
extern RPR_API_ENTRY rpr_int rprxMaterialGetParameterType(rprx_context context, rprx_material material, rprx_parameter parameter, rprx_parameter_type * out_type);
extern RPR_API_ENTRY rpr_int rprxMaterialGetParameterValue(rprx_context context, rprx_material material, rprx_parameter parameter, void * out_value);

/** @brief Commit changes for a specified material
    *
    *   initializes internal data structures and established necessary conditions
    *
    *  @param context           RPRX context
    *  @param material          Material to commit changes for
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxMaterialCommit(rprx_context context, rprx_material material);

/** @brief Attach RPRX material for a shape
    *
    *   RPRX library internally optimizes material by recompiling internal material graph structure.
    *   Change-commit approach allows to minimize the number of costly unnecessary recompilations and only
    *   recompile once before the rendering has started. As a result of this recompilation the output node
    *   of the material graph might change and the library might need to reapply the material to the shape.
    *   To implement this, the library needs to "track" the objects material is assigned to.
    *
    *  @param context           RPRX context
    *  @param shape             Shape
    *  @param material          Material
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxShapeAttachMaterial(rprx_context context, rpr_shape shape, rprx_material material);

/** @brief Detach RPRX material from a shape
    *
    *   RPRX library internally optimizes material by recompiling internal material graph structure.
    *   Change-commit approach allows to minimize the number of costly unnecessary recompilations and only
    *   recompile once before the rendering has started. As a result of this recompilation the output node
    *   of the material graph might change and the library might need to reapply the material to the shape.
    *   To implement this, the library needs to "track" the objects material is assigned to.
    *   rprxShapeDetachMaterial function resets shape material back to nullptr and removes the object
    *   from the set of "tracked" objects.
    *
    *  @param context           RPRX context
    *  @param shape             Shape
    *  @param material          Material
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxShapeDetachMaterial(rprx_context context, rpr_shape shape, rprx_material material);

/** @brief Attach RPRX material for a shape
    *
    *   RPRX library internally optimizes material by recompiling internal material graph structure.
    *   Change-commit approach allows to minimize the number of costly unnecessary recompilations and only
    *   recompile once before the rendering has started. As a result of this recompilation the output node
    *   of the material graph might change. If materials is acting in compounds with basic RPR materials
    *   (for example in blends), this compound materials should be updated with a new output node, so 
    *   the library needs to reapply their inputs. To implement this, the library needs to "track" 
    *   the objects material is bound to.
    *
    *  @param context           RPRX context
    *  @param node              Material node to bind to
    *  @param parameter         Parameter to bind
    *  @param material          Material
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxMaterialAttachMaterial(rprx_context context, rpr_material_node node, rpr_char const * parameter, rprx_material material);

/** @brief Detach RPRX material from a shape
    *
    *   RPRX library internally optimizes material by recompiling internal material graph structure.
    *   Change-commit approach allows to minimize the number of costly unnecessary recompilations and only
    *   recompile once before the rendering has started. As a result of this recompilation the output node
    *   of the material graph might change. If materials is acting in compounds with basic RPR materials
    *   (for example in blends), this compound materials should be updated with a new output node, so
    *   the library needs to reapply their inputs. To implement this, the library needs to "track"
    *   the objects material is bound to. rprxMaterialDetachMaterial resets specified parameter back 
    *   to nullptr and removes the node from the set of "tracked" nodes.
    *
    *  @param context           RPRX context
    *  @param node              Material node to bind to
    *  @param parameter         Parameter to bind
    *  @param material          Material
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxMaterialDetachMaterial(rprx_context context, rpr_material_node node, rpr_char const * parameter, rprx_material material);

/** @brief Delete RPRX context
    *
    *   The function deletes the context passed in.
    *
    *  @param context           RPRX context
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxDeleteContext(rprx_context context);

/** @brief Check whether specified rpr_material_node belongs to rprx material
    *
    *  @param context           RPRX context
    *  @param node              RPR material node
    *  @param material          RPRX material 
    *  @param result            out_result result
    *  @return                  RPR_SUCCESS in case of success, error code otherwise
    */
extern RPR_API_ENTRY rpr_int rprxIsMaterialRprx(rprx_context context, rpr_material_node node, rprx_material * out_material, rpr_bool* out_result);
extern RPR_API_ENTRY rpr_int rprxGetLog(rprx_context context, rpr_char* log, size_t* size);
extern RPR_API_ENTRY rpr_int rprxShapeGetMaterial(rprx_context context, rpr_shape shape, rprx_material* material);
#ifdef __cplusplus
}
#endif

#endif  /*__RPRSUPPORT_H  */
