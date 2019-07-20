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
#pragma once

#include "WrapObject.h"
#include "Output/clwoutput.h"
#include "Renderers/renderer.h"
#include "RadeonProRender_GL.h"

//this class represent rpr_context
class FramebufferObject
    : public WrapObject
{
public:
    FramebufferObject(Baikal::Output* out);
    FramebufferObject(CLWContext context, CLWKernel copy_cernel, rpr_GLenum target, rpr_GLint miplevel, rpr_GLuint texture);
    virtual ~FramebufferObject();

    //output
    void SetOutput(Baikal::Output* out)
    { 
        delete m_output;
        m_output = out;
    }

    std::size_t Width();
    std::size_t Height();
    void GetData(void* out_data);

    void Clear();
    void SaveToFile(const char* path);
    
    //if interop this will copy CL output data to GL texture
    void UpdateGlTex();
    Baikal::Output* GetOutput() { return m_output; }
private:
    Baikal::Output* m_output;
    std::size_t m_width;
    std::size_t m_height;
    CLWImage2D m_cl_interop_image;
    CLWContext m_context;
    CLWKernel m_copy_cernel;
};
