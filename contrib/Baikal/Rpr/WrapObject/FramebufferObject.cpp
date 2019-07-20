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

#include "GL/glew.h"
#include "WrapObject/FramebufferObject.h"
#include "WrapObject/Exception.h"
#include "Output/clwoutput.h"
#include "RadeonProRender.h"
#include "RadeonProRender_GL.h"
#include "Renderers/monte_carlo_renderer.h"

#ifdef BAIKAL_ENABLE_IO
#include "OpenImageIO/imageio.h"
#endif

FramebufferObject::FramebufferObject(Baikal::Output* out)
    : m_output(out)
    , m_width(out->width())
    , m_height(out->height())
{

}

FramebufferObject::FramebufferObject(CLWContext context, CLWKernel copy_kernel, rpr_GLenum target, rpr_GLint miplevel, rpr_GLuint texture)
    : m_output(nullptr)
    , m_width(0)
    , m_height(0)
    , m_context(context)
    , m_copy_cernel(copy_kernel)
{
    if (target != GL_TEXTURE_2D)
    {
        throw Exception(RPR_ERROR_INTERNAL_ERROR, "Unsupported GL texture target.");
    }

    //need to get width and height of GL texture
    GLint backup_tex = 0;
    glGetIntegerv(GL_TEXTURE_BINDING_2D, &backup_tex);
    glBindTexture(target, texture);
    glGetTexLevelParameteriv(target, miplevel, GL_TEXTURE_WIDTH, (GLint*)&m_width);
    glGetTexLevelParameteriv(target, miplevel, GL_TEXTURE_HEIGHT, (GLint*)&m_height);

    //create interop image
    m_cl_interop_image = context.CreateImage2DFromGLTexture(texture);
    glBindTexture(target, backup_tex);
}

FramebufferObject::~FramebufferObject()
{
    delete m_output;
    m_output = nullptr;
}

std::size_t FramebufferObject::Width()
{
    return m_width;
}

std::size_t FramebufferObject::Height()
{
    return m_height;
}

void FramebufferObject::GetData(void* out_data)
{
    m_output->GetData(static_cast<RadeonRays::float3*>(out_data));
}

void FramebufferObject::Clear()
{
    Baikal::ClwOutput* output = dynamic_cast<Baikal::ClwOutput*>(m_output);
    output->Clear(RadeonRays::float3(0.f, 0.f, 0.f, 0.f));
}

void FramebufferObject::UpdateGlTex()
{
    //only if FramebufferObject was created from GL texture
    if (m_cl_interop_image)
    {
        std::vector<cl_mem> objects;
        objects.push_back(m_cl_interop_image);
        m_context.AcquireGLObjects(0, objects);
        
        int argc = 0;
        m_copy_cernel.SetArg(argc++, static_cast<Baikal::ClwOutput*>(GetOutput())->data());
        m_copy_cernel.SetArg(argc++, static_cast<cl_int>(Width()));
        m_copy_cernel.SetArg(argc++, static_cast<cl_int>(Height()));
        m_copy_cernel.SetArg(argc++, 2.2f);
        m_copy_cernel.SetArg(argc++, m_cl_interop_image);

        std::size_t globalsize = Width() * Height();
        m_context.Launch1D(0, ((globalsize + 63) / 64) * 64, 64, m_copy_cernel);

        m_context.ReleaseGLObjects(0, objects);
        m_context.Finish(0);
    }
}


void FramebufferObject::SaveToFile(const char* path)
{
#ifdef BAIKAL_ENABLE_IO
    OIIO_NAMESPACE_USING;

    std::size_t width = Width();
    size_t height = Height();
    std::vector<RadeonRays::float3> tempbuf(width * height);
    GetData(tempbuf.data());
    std::vector<RadeonRays::float3> data(tempbuf);

    //convert pixels
    for (std::size_t y = 0; y < height; ++y)
    {
        for (std::size_t x = 0; x < width; ++x)
        {

            RadeonRays::float3 val = data[y * width + x];
            tempbuf[y * width + x] = (1.f / val.w) * val;

            tempbuf[y * width + x].x = std::pow(tempbuf[y * width + x].x, 1.f / 2.2f);
            tempbuf[y * width + x].y = std::pow(tempbuf[y * width + x].y, 1.f / 2.2f);
            tempbuf[y * width + x].z = std::pow(tempbuf[y * width + x].z, 1.f / 2.2f);
        }
    }

    //save results to file
    ImageOutput* out = ImageOutput::create(path);
    if (!out)
    {
        throw Exception(RPR_ERROR_IO_ERROR, "FramebufferObject: failed to create file.");
    }

    ImageSpec spec(static_cast<int>(width), static_cast<int>(height), 3, TypeDesc::FLOAT);
    out->open(path, spec);
    out->write_image(TypeDesc::FLOAT, &tempbuf[0], sizeof(RadeonRays::float3));
    out->close();
    delete out;
#else
	throw Exception(RPR_ERROR_IO_ERROR, "FramebufferObject: failed to create file.");
#endif
}
