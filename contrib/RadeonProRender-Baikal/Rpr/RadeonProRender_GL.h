/*****************************************************************************\
*
*  Module Name    FireRender_GL.h
*  Project        FireRender Engine OpenGL Interop API
*
*  Description    Fire Render Engine OpenGL Interop header
*
*  Copyright 2011 - 2013 Advanced Micro Devices, Inc.
*
*  All rights reserved.  This notice is intended as a precaution against
*  inadvertent publication and does not imply publication or any waiver
*  of confidentiality.  The year included in the foregoing notice is the
*  year of creation of the work.
*  @author Dmitry Kozlov (dmitry.kozlov@amd.com)
*  @author Takahiro Harada (takahiro.harada@amd.com)
*  @bug No known bugs.
*
\*****************************************************************************/
#ifndef __RADEONPRORENDER_GL_H
#define __RADEONPRORENDER_GL_H

#ifdef __cplusplus
extern "C" {
#endif

#include "RadeonProRender.h"

/* Error codes  */
#define RPR_INVALID_GL_SHAREGROUP_REFERENCE_KHR -1000 

/* rpr_context_properties  */
#define RPR_GL_CONTEXT_KHR 0x2001 
#define RPR_EGL_DISPLAY_KHR 0x2002 
#define RPR_GLX_DISPLAY_KHR 0x2003 
#define RPR_WGL_HDC_KHR 0x2004 
#define RPR_CGL_SHAREGROUP_KHR 0x2005 

typedef unsigned int rpr_GLuint;
typedef int rpr_GLint;
typedef unsigned int rpr_GLenum;
typedef rpr_uint rpr_gl_object_type;
typedef rpr_uint rpr_gl_texture_info;
typedef rpr_uint rpr_gl_platform_info;

extern RPR_API_ENTRY rpr_int rprContextCreateFramebufferFromGLTexture2D(rpr_context context, rpr_GLenum target, rpr_GLint miplevel, rpr_GLuint texture, rpr_framebuffer * out_fb);
/***************compatibility part***************/
#define FR_INVALID_GL_SHAREGROUP_REFERENCE_KHR -1000 
#define FR_GL_CONTEXT_KHR 0x2001 
#define FR_EGL_DISPLAY_KHR 0x2002 
#define FR_GLX_DISPLAY_KHR 0x2003 
#define FR_WGL_HDC_KHR 0x2004 
#define FR_CGL_SHAREGROUP_KHR 0x2005 
typedef rpr_GLuint fr_GLuint;
typedef rpr_GLint fr_GLint;
typedef rpr_GLenum fr_GLenum;
typedef rpr_gl_object_type fr_gl_object_type;
typedef rpr_gl_texture_info fr_gl_texture_info;
typedef rpr_gl_platform_info fr_gl_platform_info;
extern RPR_API_ENTRY fr_int frContextCreateFramebufferFromGLTexture2D(fr_context context, fr_GLenum target, fr_GLint miplevel, fr_GLuint texture, fr_framebuffer * out_fb);

#ifdef __cplusplus
}
#endif

#endif  /*__RADEONPRORENDER_GL_H  */
