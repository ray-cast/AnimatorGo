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
#include "Application/gl_render.h" 
#include <cassert>
#include <iostream>

#define CHECK_GL_ERROR assert(glGetError() == 0)

namespace Baikal
{
    void AppGlRender::Render(GLFWwindow* window)
    {
        try
        {
            {
                int w, h;
                glfwGetFramebufferSize(window, &w, &h); CHECK_GL_ERROR;
                glDisable(GL_DEPTH_TEST); CHECK_GL_ERROR;
                glViewport(0, 0, w, h); CHECK_GL_ERROR;


                glClear(GL_COLOR_BUFFER_BIT); CHECK_GL_ERROR;
                glBindVertexArray(m_vao); CHECK_GL_ERROR;

                GLuint program = m_shader_manager->GetProgram("../BaikalStandalone/Kernels/GLSL/simple");
                glUseProgram(program); CHECK_GL_ERROR;

                GLuint texloc = glGetUniformLocation(program, "g_Texture");
                assert(texloc >= 0);

                glUniform1i(texloc, 0); CHECK_GL_ERROR;

                glActiveTexture(GL_TEXTURE0); CHECK_GL_ERROR;
                glBindTexture(GL_TEXTURE_2D, m_texture); CHECK_GL_ERROR;


                glEnableVertexAttribArray(0); CHECK_GL_ERROR;
                glEnableVertexAttribArray(1); CHECK_GL_ERROR;

                glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer); CHECK_GL_ERROR;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer); CHECK_GL_ERROR;

                glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(float) * 5, 0); CHECK_GL_ERROR;
                glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(float) * 5, (void*)(sizeof(float) * 3)); CHECK_GL_ERROR;
                glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_SHORT, nullptr);

                glDisableVertexAttribArray(0); CHECK_GL_ERROR;
                glDisableVertexAttribArray(1); CHECK_GL_ERROR;
                glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERROR;
                glBindBuffer(GL_ARRAY_BUFFER, 0); CHECK_GL_ERROR;
                glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); CHECK_GL_ERROR;
                glUseProgram(0); CHECK_GL_ERROR;
                glBindVertexArray(0);

                glFinish(); CHECK_GL_ERROR;
            }
        }
        catch (std::runtime_error& e)
        {
            std::cout << e.what();
            exit(-1);
        }
    }

    AppGlRender::AppGlRender(const AppSettings& settings)
        : m_window_width(settings.width)
        , m_window_height(settings.height)
    {
        m_shader_manager.reset(new ShaderManager());

        glGetError();
        glClearColor(0.0, 0.5, 0.0, 0.0); CHECK_GL_ERROR;
        glDisable(GL_DEPTH_TEST); CHECK_GL_ERROR;

        glGenBuffers(1, &m_vertex_buffer); CHECK_GL_ERROR;
        glGenBuffers(1, &m_index_buffer); CHECK_GL_ERROR;

        glGenVertexArrays(1, &m_vao); CHECK_GL_ERROR;

        // create Vertex buffer
        glBindBuffer(GL_ARRAY_BUFFER, m_vertex_buffer); CHECK_GL_ERROR;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, m_index_buffer); CHECK_GL_ERROR;

        float quad_vdata[] =
        {
            -1, -1, 0.5, 0, 0,
            1, -1, 0.5, 1, 0,
            1, 1, 0.5, 1, 1,
            -1, 1, 0.5, 0, 1
        };

        GLshort quad_idata[] =
        {
            0, 1, 3,
            3, 1, 2
        };

        // fill data
        glBufferData(GL_ARRAY_BUFFER, sizeof(quad_vdata), quad_vdata, GL_STATIC_DRAW); CHECK_GL_ERROR;
        glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(quad_idata), quad_idata, GL_STATIC_DRAW); CHECK_GL_ERROR;

        glBindBuffer(GL_ARRAY_BUFFER, 0); CHECK_GL_ERROR;
        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0); CHECK_GL_ERROR;


        glGenTextures(1, &m_texture); CHECK_GL_ERROR;
        glBindTexture(GL_TEXTURE_2D, m_texture); CHECK_GL_ERROR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST); CHECK_GL_ERROR;
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST); CHECK_GL_ERROR;

        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, m_window_width, m_window_height, 0, GL_RGBA, GL_FLOAT, nullptr); CHECK_GL_ERROR;

        glBindTexture(GL_TEXTURE_2D, 0); CHECK_GL_ERROR;
    }
}
