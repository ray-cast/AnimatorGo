
/**********************************************************************
 Copyright (c) 2017 Advanced Micro Devices, Inc. All rights reserved.

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

#include "Application/app_utils.h"
#include "Application/cl_render.h"
#include "Application/gl_render.h"
#include "Application/material_explorer.h"
#include "image_io.h"

#include <future>
#include <memory>
#include <chrono>

namespace Baikal
{

    class Application
    {
    public:
        Application(int argc, char * argv[]);
        void Run();
    private:
        void Update(bool update_required);

        // Update app state according to gui
        // Return: true if scene update required
        bool UpdateGui();
        void CollectSceneStats();

        void SaveToFile(std::chrono::high_resolution_clock::time_point time) const;

        // Input callbacks
        // Note: use glfwGetWindowUserPointer(window) to get app instance
        static void OnKey(GLFWwindow* window, int key, int scancode, int action, int mods);
        static void OnMouseMove(GLFWwindow* window, double x, double y);
        static void OnMouseButton(GLFWwindow* window, int button, int action, int mods);
        static void OnMouseScroll(GLFWwindow* window, double x, double y);
        static void OnWindowResize(GLFWwindow* window, int /* width */, int /* height */);

        AppSettings m_settings;

        // Note: the following three members should be declared in strict order
        std::unique_ptr<GLFWwindow, void(*)(GLFWwindow*)> m_window;
        std::unique_ptr<AppGlRender> m_gl;
        std::unique_ptr<AppClRender> m_cl;

        // Scene stats stuff
        int m_num_triangles;
        int m_num_instances;

        int m_current_shape_id;
        std::string m_object_name;
        std::future<int> m_shape_id_future;

        class InputSettings
        {
        public:
            bool HasMultiplier() const;
            float GetMultiplier() const;
            void SetMultiplier(float multiplier);

            RadeonRays::float3 GetColor() const;
            void SetColor(RadeonRays::float3 color);

            std::uint32_t GetInteger() const;
            void SetInteger(std::uint32_t integer);

            std::string GetTexturePath() const;
            void SetTexturePath(std::string texture_path);

        private:
            std::pair<bool, float> m_multiplier;
            std::pair<bool, RadeonRays::float3> m_color;
            std::pair<bool, std::uint32_t> m_integer_input;
            std::pair<bool, std::string> m_texture_path;
        };

        int m_material_id;
        MaterialExplorer::Ptr m_material_explorer;
    };
}
