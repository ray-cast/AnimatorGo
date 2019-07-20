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

#include <OpenImageIO/imageio.h>
#ifdef __APPLE__
#include <OpenCL/OpenCL.h>
#define GLFW_INCLUDE_GLCOREARB
#define GLFW_NO_GLU
#include "GLFW/glfw3.h"
#elif WIN32
#define NOMINMAX
#include <Windows.h>
#include "GL/glew.h"
#include "GLFW/glfw3.h"
#else
#include <CL/cl.h>
#include <GL/glew.h>
#include <GL/glx.h>
#include "GLFW/glfw3.h"
#endif

#include "ImGUI/imgui.h"
#include "ImGUI/imgui_impl_glfw_gl3.h"

#include "XML/tinyxml2.h"

#include <memory>
#include <chrono>
#include <cassert>
#include <vector>
#include <stdexcept>
#include <sstream>
#include <iostream>
#include <cstdlib>
#include <algorithm>
#include <numeric>
#include <atomic>
#include <mutex>
#include <fstream>
#include <functional>
#include <queue>


#define _USE_MATH_DEFINES
#include <math.h>

#ifdef RR_EMBED_KERNELS
#include "./CL/cache/kernels.h"
#endif

#include "CLW.h"

#include "math/mathutils.h"
#include "Application/application.h"
#include "material_io.h"
#include "Application/material_explorer.h"

using namespace RadeonRays;

namespace Baikal
{
    static bool     g_is_left_pressed = false;
    static bool     g_is_right_pressed = false;
    static bool     g_is_fwd_pressed = false;
    static bool     g_is_back_pressed = false;
    static bool     g_is_climb_pressed = false;
    static bool     g_is_descent_pressed = false;
    static bool     g_is_mouse_tracking = false;
    static bool     g_is_double_click = false;
    static bool     g_is_f10_pressed = false;
    static bool     g_is_middle_pressed = false; // middle mouse button
    static bool     g_is_c_pressed = false;
    static bool     g_is_l_pressed = false;
    static float2   g_mouse_pos = float2(0, 0);
    static float2   g_mouse_delta = float2(0, 0);
    static float2   g_scroll_delta = float2(0, 0);

    const std::string kCameraLogFile("camera.xml");
    //ls - light set
    const std::string kLightLogFile("light.xml");

    auto start = std::chrono::high_resolution_clock::now();

    // helper functions (AppendCamera, AppendLight) to log camera position and light settings
    void AppendCamera(Baikal::PerspectiveCamera const* cam, const char* xml)
    {
        //camera values
        RadeonRays::float3 cam_pos = cam->GetPosition();
        RadeonRays::float3 cam_at = cam->GetForwardVector();
        RadeonRays::float3 cam_up = cam->GetUpVector();

        float aperture = cam->GetAperture();
        float focus_dist = cam->GetFocusDistance();
        float focal_length = cam->GetFocalLength();

        tinyxml2::XMLDocument doc;

        doc.LoadFile(xml);
        auto root = doc.FirstChildElement("cam_list");
        if (!root)
        {
            root = doc.NewElement("cam_list");
            doc.InsertFirstChild(root);
        }

        auto new_cam = doc.NewElement("camera");
        //position
        new_cam->SetAttribute("cpx", cam_pos.x);
        new_cam->SetAttribute("cpy", cam_pos.y);
        new_cam->SetAttribute("cpz", cam_pos.z);

        //target
        new_cam->SetAttribute("tpx", cam_at.x);
        new_cam->SetAttribute("tpy", cam_at.y);
        new_cam->SetAttribute("tpz", cam_at.z);

        //uo vector
        new_cam->SetAttribute("upx", cam_up.x);
        new_cam->SetAttribute("upy", cam_up.y);
        new_cam->SetAttribute("upz", cam_up.z);

        //other values
        new_cam->SetAttribute("aperture", aperture);
        new_cam->SetAttribute("focus_dist", focus_dist);
        new_cam->SetAttribute("focal_length", focal_length);

        root->InsertEndChild(new_cam);
        doc.SaveFile(xml);
    }

    void AppendLight(Baikal::Light::Ptr l, const char* xml)
    {
        //get light type
        Baikal::ImageBasedLight* ibl = dynamic_cast<Baikal::ImageBasedLight*>(l.get());
        Baikal::PointLight* pointl = dynamic_cast<Baikal::PointLight*>(l.get());
        Baikal::DirectionalLight* directl = dynamic_cast<Baikal::DirectionalLight*>(l.get());
        Baikal::SpotLight* spotl = dynamic_cast<Baikal::SpotLight*>(l.get());
        Baikal::AreaLight* areal = dynamic_cast<Baikal::AreaLight*>(l.get());

        tinyxml2::XMLDocument doc;

        doc.LoadFile(xml);
        auto root = doc.FirstChildElement("light_list");
        if (!root)
        {
            root = doc.NewElement("light_list");
            doc.InsertFirstChild(root);
        }

        if (areal)
        {
            //area lights are created when materials load, so ignore it;
            return;
        }

        tinyxml2::XMLElement* light_elem = nullptr;
        if (ibl)
        {
            light_elem = doc.NewElement("light");
            light_elem->SetAttribute("type", "ibl");
            light_elem->SetAttribute("tex", ibl->GetTexture()->GetName().c_str());
            light_elem->SetAttribute("mul", ibl->GetMultiplier());
        }
        else if (spotl)
        {
            light_elem = doc.NewElement("light");
            light_elem->SetAttribute("type", "spot");
            light_elem->SetAttribute("csx", spotl->GetConeShape().x);
            light_elem->SetAttribute("csy", spotl->GetConeShape().y);
        }
        else if (pointl)
        {
            light_elem = doc.NewElement("light");
            light_elem->SetAttribute("type", "point");
        }
        else if (directl)
        {
            light_elem = doc.NewElement("light");
            light_elem->SetAttribute("type", "direct");
        }

        float3 p = l->GetPosition();
        float3 d = l->GetDirection();
        float3 r = l->GetEmittedRadiance();

        light_elem->SetAttribute("posx", p.x);
        light_elem->SetAttribute("posy", p.y);
        light_elem->SetAttribute("posz", p.z);

        light_elem->SetAttribute("dirx", d.x);
        light_elem->SetAttribute("diry", d.y);
        light_elem->SetAttribute("dirz", d.z);

        light_elem->SetAttribute("radx", r.x);
        light_elem->SetAttribute("rady", r.y);
        light_elem->SetAttribute("radz", r.z);

        root->InsertEndChild(light_elem);
        doc.SaveFile(xml);
    }

    bool Application::InputSettings::HasMultiplier() const
    { return m_multiplier.first; }

    float Application::InputSettings::GetMultiplier() const
    { return m_multiplier.second; }

    void Application::InputSettings::SetMultiplier(float multiplier)
    {
        m_multiplier.first = true;
        m_multiplier.second = multiplier;
    }

    RadeonRays::float3  Application::InputSettings::GetColor() const
    { return m_color.second; }

    void Application::InputSettings::SetColor(RadeonRays::float3  color)
    {
        m_color.first = true;
        m_color.second = color;
    }

    std::uint32_t Application::InputSettings::GetInteger() const
    { return m_integer_input.second; }

    void Application::InputSettings::SetInteger(std::uint32_t integer)
    {
        m_integer_input.first= true;
        m_integer_input.second = integer;
    }

    std::string Application::InputSettings::GetTexturePath() const
    { return m_texture_path.second; }

    void Application::InputSettings::SetTexturePath(std::string texture_path)
    {
        m_texture_path.first = true;
        m_texture_path.second = texture_path;
    }


    void Application::OnMouseMove(GLFWwindow* window, double x, double y)
    {
        if (g_is_mouse_tracking)
        {
            g_mouse_delta = float2((float)x, (float)y) - g_mouse_pos;
            g_mouse_pos = float2((float)x, (float)y);
        }
    }

    void Application::OnMouseScroll(GLFWwindow* window, double x, double y)
    {
        g_scroll_delta = float2((float)x, (float)y);
    }

    void Application::OnMouseButton(GLFWwindow* window, int button, int action, int mods)
    {
        if ((button == GLFW_MOUSE_BUTTON_LEFT) ||
            (button == GLFW_MOUSE_BUTTON_RIGHT) ||
            (button == GLFW_MOUSE_BUTTON_MIDDLE))
        {
            if (action == GLFW_PRESS)
            {

                double x, y;
                glfwGetCursorPos(window, &x, &y);
                g_mouse_pos = float2((float)x, (float)y);
                g_mouse_delta = float2(0, 0);
            }
            else if (action == GLFW_RELEASE && g_is_mouse_tracking)
            {
                g_is_mouse_tracking = false;
                g_is_middle_pressed = false;
                g_mouse_delta = float2(0, 0);
            }
        }

        if ((button == GLFW_MOUSE_BUTTON_RIGHT) &&  (action == GLFW_PRESS))
        {
            g_is_mouse_tracking = true;
        }

        if (button == GLFW_MOUSE_BUTTON_LEFT)
        {
            if (action == GLFW_PRESS)
            {
                double x, y;
                glfwGetCursorPos(window, &x, &y);
                g_mouse_pos = float2((float)x, (float)y);

                auto duration = std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::high_resolution_clock::now() - start);
                g_is_double_click = (duration.count() < 200) ? (true) : (false);
                start = std::chrono::high_resolution_clock::now();
            }

            else if (action == GLFW_RELEASE  && g_is_mouse_tracking)
            {
                g_is_double_click = false;
            }
        }

        if (button == GLFW_MOUSE_BUTTON_MIDDLE)
        {
            if (action == GLFW_PRESS)
            {
                g_is_middle_pressed = true;
                g_is_mouse_tracking = true;
            }
        }
    }

    void Application::OnKey(GLFWwindow* window, int key, int scancode, int action, int mods)
    {
        ImGuiIO& io = ImGui::GetIO();
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        
        const bool press_or_repeat = action == GLFW_PRESS || action == GLFW_REPEAT;

        if (action == GLFW_PRESS)
            io.KeysDown[key] = true;
        if (action == GLFW_RELEASE)
            io.KeysDown[key] = false;

        (void)mods; // Modifiers are not reliable across systems
        io.KeyCtrl = io.KeysDown[GLFW_KEY_LEFT_CONTROL] || io.KeysDown[GLFW_KEY_RIGHT_CONTROL];
        io.KeyShift = io.KeysDown[GLFW_KEY_LEFT_SHIFT] || io.KeysDown[GLFW_KEY_RIGHT_SHIFT];
        io.KeyAlt = io.KeysDown[GLFW_KEY_LEFT_ALT] || io.KeysDown[GLFW_KEY_RIGHT_ALT];
        io.KeySuper = io.KeysDown[GLFW_KEY_LEFT_SUPER] || io.KeysDown[GLFW_KEY_RIGHT_SUPER];

        switch (key)
        {
        case GLFW_KEY_W:
            g_is_fwd_pressed = press_or_repeat;
            break;
        case GLFW_KEY_S:
            g_is_back_pressed = press_or_repeat;
            break;
        case GLFW_KEY_A:
            g_is_left_pressed = press_or_repeat;
            break;
        case GLFW_KEY_D:
            g_is_right_pressed = press_or_repeat;
            break;
        case GLFW_KEY_Q:
            g_is_climb_pressed = press_or_repeat;
            break;
        case GLFW_KEY_E:
            g_is_descent_pressed = press_or_repeat;
            break;
        case GLFW_KEY_F1:
            app->m_settings.gui_visible = action == GLFW_PRESS ? !app->m_settings.gui_visible : app->m_settings.gui_visible;
            break;
        case GLFW_KEY_F3:
            app->m_settings.benchmark = action == GLFW_PRESS ? true : app->m_settings.benchmark;
            break;
        case GLFW_KEY_F10:
            g_is_f10_pressed = action == GLFW_PRESS;
            break;
        case GLFW_KEY_C:
            g_is_c_pressed = action == GLFW_RELEASE ? true : false;
            break;
        case GLFW_KEY_L:
            g_is_l_pressed = action == GLFW_RELEASE ? true : false;
        default:
            break;
        }
    }

    void Application::OnWindowResize(GLFWwindow* window, int /* width */, int /* height */)
    {
        Application* app = static_cast<Application*>(glfwGetWindowUserPointer(window));
        glfwSetWindowSize(window, app->m_settings.width, app->m_settings.height);
    }

    void Application::Update(bool update_required)
    {
        static auto prevtime = std::chrono::high_resolution_clock::now();
        auto time = std::chrono::high_resolution_clock::now();
        auto dt = std::chrono::duration_cast<std::chrono::duration<double>>(time - prevtime);
        prevtime = time;

        bool update = update_required;
        float camrotx = 0.f;
        float camroty = 0.f;

        const float kMouseSensitivity = 0.001125f;
        const float kScrollSensitivity = 0.05f;
        auto camera = m_cl->GetCamera();
        if (!m_settings.benchmark && !m_settings.time_benchmark)
        {
            float2 delta = g_mouse_delta * float2(kMouseSensitivity, kMouseSensitivity);
            float2 scroll_delta = g_scroll_delta * float2(kScrollSensitivity, kScrollSensitivity);
            camrotx = -delta.x;
            camroty = -delta.y;


            if (!g_is_middle_pressed)
            {

                if (std::abs(camroty) > 0.001f)
                {
                    camera->Tilt(camroty);
                    update = true;
                }

                if (std::abs(camrotx) > 0.001f)
                {

                    camera->Rotate(camrotx);
                    update = true;
                }
            }

            const float kMovementSpeed = m_settings.cspeed;
            if (std::abs(scroll_delta.y) > 0.001f)
            {
                camera->Zoom(scroll_delta.y * kMovementSpeed);
                g_scroll_delta = float2();
                update = true;
            }
            if (g_is_fwd_pressed)
            {
                camera->MoveForward((float)dt.count() * kMovementSpeed);
                update = true;
            }

            if (g_is_back_pressed)
            {
                camera->MoveForward(-(float)dt.count() * kMovementSpeed);
                update = true;
            }

            if (g_is_right_pressed)
            {
                camera->MoveRight((float)dt.count() * kMovementSpeed);
                update = true;
            }

            if (g_is_left_pressed)
            {
                camera->MoveRight(-(float)dt.count() * kMovementSpeed);
                update = true;
            }

            if (g_is_climb_pressed)
            {
                camera->MoveUp((float)dt.count() * kMovementSpeed);
                update = true;
            }

            if (g_is_descent_pressed)
            {
                camera->MoveUp(-(float)dt.count() * kMovementSpeed);
                update = true;
            }

            if (g_is_f10_pressed)
            {
                g_is_f10_pressed = false; //one time execution
                SaveToFile(time);
            }
#ifdef GENERATE_DATASET
            //log camera props
            if (g_is_c_pressed)
            {
                PerspectiveCamera* pcam = dynamic_cast<PerspectiveCamera*>(camera.get());
                AppendCamera(pcam, kCameraLogFile.c_str());

                g_is_c_pressed = false;
            }
            //log scene lights
            if (g_is_l_pressed)
            {
                auto scene = m_cl->GetScene();
                auto it = scene->CreateLightIterator();
                for (; it->IsValid(); it->Next())
                {
                    Light::Ptr l = it->ItemAs<Light>();
                    AppendLight(l, kLightLogFile.c_str());
                }

                g_is_l_pressed = false;
            }
#endif
            if (g_is_middle_pressed)
            {
                float distance = (float)dt.count() * kMovementSpeed / 2.f;
                float right_shift, up_shift;
                right_shift = (delta.x) ? distance * delta.x / std::abs(delta.x) : 0;
                up_shift = (delta.y) ? distance * delta.y / std::abs(delta.y) : 0;
                camera->MoveRight(-right_shift);
                camera->MoveUp(up_shift);
                update = true;
                g_mouse_delta = RadeonRays::float2(0, 0);
            }
        }

        if (update)
        {
            //if (g_num_samples > -1)
            {
                m_settings.samplecount = 0;
            }

            m_cl->UpdateScene();
        }

        if (m_settings.num_samples == -1 || m_settings.samplecount <  m_settings.num_samples)
        {
            m_cl->Render(m_settings.samplecount);

        }
        else if (m_settings.samplecount == m_settings.num_samples)
        {
            m_cl->SaveFrameBuffer(m_settings);
            std::cout << "Target sample count reached\n";
        }

        m_cl->Update(m_settings);
    }

    void Application::SaveToFile(std::chrono::high_resolution_clock::time_point time) const
    {
        using namespace OIIO;
        int w, h;
        glfwGetFramebufferSize(m_window.get(), &w, &h);
        assert(glGetError() == 0);

        const auto channels = 3;
        auto *data = new GLubyte[channels * w * h];
        glReadPixels(0, 0, w, h, GL_RGB, GL_UNSIGNED_BYTE, data);

        //opengl coordinates to oiio coordinates
        for (auto i = 0; i <= h / 2; ++i)
        {
            std::swap_ranges(data + channels * w * i, data + channels * w * (i + 1) - 1, data + channels * w * (h - (i + 1)));
        }
        
        const auto filename = m_settings.path + "/" + m_settings.base_image_file_name + "-" + std::to_string(time.time_since_epoch().count()) + "." + m_settings.image_file_format;
        
        auto out = ImageOutput::create(filename);
        if (out)
        {
            ImageSpec spec{ w, h, channels, TypeDesc::UINT8 };
            out->open(filename, spec);
            out->write_image(TypeDesc::UINT8, data);
            out->close();
            delete out; // ImageOutput::destroy not found
        }
        else
        {
            std::cout << "Wrong file format\n";
        }
        
        delete[] data;
    }


    void OnError(int error, const char* description)
    {
        std::cout << description << "\n";
    }

    bool GradeTimeBenchmarkResults(std::string const& scene, int time_in_sec, std::string& rating, ImVec4& color)
    {
        if (scene == "classroom.obj")
        {
            if (time_in_sec < 70)
            {
                rating = "Excellent";
                color = ImVec4(0.1f, 0.7f, 0.1f, 1.f);
            }
            else if (time_in_sec < 100)
            {
                rating = "Good";
                color = ImVec4(0.1f, 0.7f, 0.1f, 1.f);
            }
            else if (time_in_sec < 120)
            {
                rating = "Average";
                color = ImVec4(0.7f, 0.7f, 0.1f, 1.f);
            }
            else
            {
                rating = "Poor";
                color = ImVec4(0.7f, 0.1f, 0.1f, 1.f);
            }

            return true;
        }

        return false;
    }

    Application::Application(int argc, char * argv[])
        : m_window(nullptr, glfwDestroyWindow) // Add custom deleter to shared_ptr
        , m_num_triangles(0)
        , m_num_instances(0)
    {
        // Command line parsing
        AppCliParser cli(argc, argv);
        m_settings = cli.Parse();

        if (!m_settings.cmd_line_mode)
        {
            // Initialize GLFW
            try
            {
                int err = glfwInit();

                if (err != GL_TRUE)
                {
                    throw std::runtime_error("Error code: " + std::to_string(err));
                }

            }
            catch (std::runtime_error&)
            {
                std::cerr << "GLFW initialization failed!\n";
                throw;
            }

            // Setup window
            glfwSetErrorCallback(OnError);
            glfwWindowHint(GLFW_RESIZABLE, GL_FALSE);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
            glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
            glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    #if __APPLE__
            glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
    #endif

            // GLUT Window Initialization:
            m_window.reset(glfwCreateWindow(m_settings.width, m_settings.height, "Baikal standalone demo", nullptr, nullptr));
            glfwMakeContextCurrent(m_window.get());

    #ifndef __APPLE__
            try
            {
                glewExperimental = GL_TRUE;
                GLenum err = glewInit();

                if (err != GLEW_OK)
                {
                    throw std::runtime_error((const char*)glewGetErrorString(err));
                }

            }
            catch (std::runtime_error&)
            {
                std::cerr << "GLEW initialization failed!\n";
                throw;
            }
    #endif

            ImGui_ImplGlfwGL3_Init(m_window.get(), true);

            // Set callbacks
            glfwSetWindowUserPointer(m_window.get(), this);
            glfwSetMouseButtonCallback(m_window.get(), Application::OnMouseButton);
            glfwSetCursorPosCallback(m_window.get(), Application::OnMouseMove);
            glfwSetKeyCallback(m_window.get(), Application::OnKey);
            glfwSetScrollCallback(m_window.get(), Application::OnMouseScroll);
            glfwSetWindowSizeCallback(m_window.get(), Application::OnWindowResize);

            // Initialize AppGlRender and AppClRender
            try
            {
                m_gl.reset(new AppGlRender(m_settings));
                m_cl.reset(new AppClRender(m_settings, m_gl->GetTexture()));
            }
            catch (std::runtime_error&)
            {
                std::cerr << "Error when inializing AppGlRender or AppClRender!\n";
                throw;
            }

        }
        else
        {
            m_settings.interop = false;
            m_cl.reset(new AppClRender(m_settings, -1));
        }
    }

    void Application::Run()
    {
        CollectSceneStats();

        if (!m_settings.cmd_line_mode)
        {
            try
            {
                m_cl->StartRenderThreads();
                static bool update = true;
                while (!glfwWindowShouldClose(m_window.get()))
                {
                    ImGui_ImplGlfwGL3_NewFrame();
                    Update(update);
                    m_gl->Render(m_window.get());
                    update = UpdateGui();
                    glfwSwapBuffers(m_window.get());
                    glfwPollEvents();
                }

                m_cl->StopRenderThreads();

            }
            catch (std::runtime_error&)
            {
                std::cout << "Caught exception in Application::Run()\n";
                throw;
            }

        }
        else
        {
            m_cl.reset(new AppClRender(m_settings, -1));

            std::cout << "Number of triangles: " << m_num_triangles << "\n";
            std::cout << "Number of instances: " << m_num_instances << "\n";

            //compile scene
            m_cl->UpdateScene();
            m_cl->RunBenchmark(m_settings);

            auto minutes = (int)(m_settings.time_benchmark_time / 60.f);
            auto seconds = (int)(m_settings.time_benchmark_time - minutes * 60);

            std::cout << "General benchmark results:\n";
            std::cout << "\tRendering time: " << minutes << "min:" << seconds << "s\n";
            std::string rating;
            ImVec4 color;
            if (GradeTimeBenchmarkResults(m_settings.modelname, minutes * 60 + seconds, rating, color))
            {
                std::cout << "\tRating: " << rating.c_str() << "\n";
            }
            else
            {
                std::cout << "\tRating: N/A\n";
            }

            std::cout << "RT benchmark results:\n";
            std::cout << "\tPrimary: " << m_settings.stats.primary_throughput * 1e-6f << " Mrays/s\n";
            std::cout << "\tSecondary: " << m_settings.stats.secondary_throughput * 1e-6f << " Mrays/s\n";
            std::cout << "\tShadow: " << m_settings.stats.shadow_throughput * 1e-6f << " Mrays/s\n";
        }
    }



    bool Application::UpdateGui()
    {
        static const ImVec2 win_size(380, 580);
        static float aperture = 0.0f;
        static float focal_length = 35.f;
        static float focus_distance = 1.f;
        static int num_bounces = 5;
        static const std::vector<std::pair<Baikal::Renderer::OutputType, char const*>> kBaikalOutputs =
        {
            { Renderer::OutputType::kColor, "Color" },
            { Renderer::OutputType::kOpacity, "Opacity" },
            { Renderer::OutputType::kVisibility, "Visibility" },
            { Renderer::OutputType::kWorldPosition, "World Position" },
            { Renderer::OutputType::kWorldShadingNormal, "Shading Normal" },
            { Renderer::OutputType::kWorldGeometricNormal, "Geometric Normal" },
            { Renderer::OutputType::kUv, "Texture Coordinates" },
            { Renderer::OutputType::kWireframe, "Wireframe" },
            { Renderer::OutputType::kAlbedo, "Albedo" },
            { Renderer::OutputType::kWorldTangent, "Tangent" },
            { Renderer::OutputType::kWorldBitangent, "Bitangent" },
            { Renderer::OutputType::kGloss, "Glossiness" },
            { Renderer::OutputType::kMeshID, "Object ID" },
            { Renderer::OutputType::kGroupID, "Object Group ID" },
            { Renderer::OutputType::kBackground, "Background" },
            { Renderer::OutputType::kDepth, "Depth" }
        };

        static int output = 0;
        bool update = false;
        if (m_settings.gui_visible)
        {
            ImGui::SetNextWindowSizeConstraints(win_size, win_size);
            ImGui::Begin("Baikal settings");
            ImGui::Text("Use wsad keys to move");
            ImGui::Text("Q/E to climb/descent");
            ImGui::Text("Mouse+RMB to look around");
            ImGui::Text("F1 to hide/show GUI");
            ImGui::Separator();

            auto const& configs = m_cl->GetConfigs();
            for (auto const& cfg : configs)
            {
                ImGui::Text("Device vendor: %s", cfg.context.GetDevice(0).GetVendor().c_str());
                ImGui::Text("Device name: %s", cfg.context.GetDevice(0).GetName().c_str());
                ImGui::Text("OpenCL: %s", cfg.context.GetDevice(0).GetVersion().c_str());
                ImGui::Separator();
            }

            ImGui::Text("Resolution: %dx%d ", m_settings.width, m_settings.height);
            ImGui::Text("Scene: %s", m_settings.modelname.c_str());
            ImGui::Text("Unique triangles: %d", m_num_triangles);
            ImGui::Text("Number of instances: %d", m_num_instances);
            ImGui::Separator();
            ImGui::SliderInt("GI bounces", &num_bounces, 1, 10);

            auto camera = m_cl->GetCamera();

            if (m_settings.camera_type == CameraType::kPerspective)
            {
                auto perspective_camera = std::dynamic_pointer_cast<PerspectiveCamera>(camera);

                if (!perspective_camera)
                {
                    throw std::runtime_error("Application::UpdateGui(...): can not cast to perspective camera");
                }

                if (aperture != m_settings.camera_aperture * 1000.f)
                {
                    m_settings.camera_aperture = aperture / 1000.f;
                    perspective_camera->SetAperture(m_settings.camera_aperture);
                    update = true;
                }

                if (focus_distance != m_settings.camera_focus_distance)
                {
                    m_settings.camera_focus_distance = focus_distance;
                    perspective_camera->SetFocusDistance(m_settings.camera_focus_distance);
                    update = true;
                }

                if (focal_length != m_settings.camera_focal_length * 1000.f)
                {
                    m_settings.camera_focal_length = focal_length / 1000.f;
                    perspective_camera->SetFocalLength(m_settings.camera_focal_length);
                    update = true;
                }

                ImGui::SliderFloat("Aperture(mm)", &aperture, 0.0f, 100.0f);
                ImGui::SliderFloat("Focal length(mm)", &focal_length, 5.f, 200.0f);
                ImGui::SliderFloat("Focus distance(m)", &focus_distance, 0.05f, 20.f);
            }

            if (num_bounces != m_settings.num_bounces)
            {
                m_settings.num_bounces = num_bounces;
                m_cl->SetNumBounces(num_bounces);
                update = true;
            }

            auto gui_out_type = kBaikalOutputs[output].first;

            if (gui_out_type != m_cl->GetOutputType())
            {
                m_cl->SetOutputType(gui_out_type);
                update = true;
            }

            RadeonRays::float3 eye, at;
            eye = camera->GetPosition();
            at = eye + camera->GetForwardVector();

            ImGui::Combo("Output", &output,
                [](void* , int idx, const char** out_text)
                {
                    if (out_text)
                        *out_text = kBaikalOutputs[idx].second;
                    return true;
                },
                nullptr, (int)kBaikalOutputs.size()
            );
            ImGui::Text(" ");
            ImGui::Text("Number of samples: %d", m_settings.samplecount);
            ImGui::Text("Frame time %.3f ms/frame (%.1f FPS)", 1000.0f / ImGui::GetIO().Framerate, ImGui::GetIO().Framerate);
            ImGui::Text("Renderer performance %.3f Msamples/s", (ImGui::GetIO().Framerate *m_settings.width * m_settings.height) / 1000000.f);
            ImGui::Text("Eye: x = %.3f y = %.3f z = %.3f", eye.x, eye.y, eye.z);
            ImGui::Text("At: x = %.3f y = %.3f z = %.3f", at.x, at.y, at.z);
            ImGui::Separator();

            if (m_settings.time_benchmark)
            {
                ImGui::ProgressBar(m_settings.samplecount / 512.f);
            }

            static decltype(std::chrono::high_resolution_clock::now()) time_bench_start_time;
            if (!m_settings.time_benchmark && !m_settings.benchmark)
            {
                if (ImGui::Button("Start benchmark") && m_settings.num_samples == -1)
                {
                    time_bench_start_time = std::chrono::high_resolution_clock::now();
                    m_settings.time_benchmark = true;
                    update = true;
                }

                if (!m_settings.time_benchmark && ImGui::Button("Start RT benchmark"))
                {
                    m_settings.benchmark = true;
                }
            }

            if (m_settings.time_benchmark && m_settings.samplecount > 511)
            {
                m_settings.time_benchmark = false;
                auto delta = std::chrono::duration_cast<std::chrono::milliseconds>
                    (std::chrono::high_resolution_clock::now() - time_bench_start_time).count();
                m_settings.time_benchmark_time = delta / 1000.f;
                m_settings.time_benchmarked = true;
            }

            if (m_settings.time_benchmarked)
            {
                auto minutes = (int)(m_settings.time_benchmark_time / 60.f);
                auto seconds = (int)(m_settings.time_benchmark_time - minutes * 60);
                ImGui::Separator();

                ImVec4 color;
                std::string rating;
                ImGui::Text("Rendering time: %2dmin:%ds", minutes, seconds);
                if (GradeTimeBenchmarkResults(m_settings.modelname, minutes * 60 + seconds, rating, color))
                {
                    ImGui::TextColored(color, "Rating: %s", rating.c_str());
                }
                else
                {
                    ImGui::Text("Rating: N/A");
                }
            }

            if (m_settings.rt_benchmarked)
            {
                auto& stats = m_settings.stats;

                ImGui::Separator();
                ImGui::Text("Primary rays: %f Mrays/s", stats.primary_throughput * 1e-6f);
                ImGui::Text("Secondary rays: %f Mrays/s", stats.secondary_throughput * 1e-6f);
                ImGui::Text("Shadow rays: %f Mrays/s", stats.shadow_throughput * 1e-6f);
            }

#ifdef ENABLE_DENOISER
            ImGui::Separator();

            static float sigmaPosition = m_cl->GetDenoiserFloatParam("position_sensitivity").x;
            static float sigmaNormal = m_cl->GetDenoiserFloatParam("normal_sensitivity").x;
            static float sigmaColor = m_cl->GetDenoiserFloatParam("color_sensitivity").x;

            ImGui::Text("Denoiser settings");
            ImGui::SliderFloat("Position sigma", &sigmaPosition, 0.f, 0.3f);
            ImGui::SliderFloat("Normal sigma", &sigmaNormal, 0.f, 5.f);
            ImGui::SliderFloat("Color sigma", &sigmaColor, 0.f, 5.f);       

            if (m_cl->GetDenoiserFloatParam("position_sensitivity").x != sigmaPosition ||
                m_cl->GetDenoiserFloatParam("normal_sensitivity").x != sigmaNormal ||
                m_cl->GetDenoiserFloatParam("color_sensitivity").x != sigmaColor)
            {
                m_cl->SetDenoiserFloatParam("position_sensitivity", sigmaPosition);
                m_cl->SetDenoiserFloatParam("normal_sensitivity", sigmaNormal);
                m_cl->SetDenoiserFloatParam("color_sensitivity", sigmaColor);
            }
#endif
            ImGui::End();

            // Get shape/material info from renderer
            if (m_shape_id_future.valid())
            {
                m_current_shape_id = m_shape_id_future.get();
                auto shape = m_cl->GetShapeById(m_current_shape_id);

                if (shape)
                {
                    // set basic material settings
                    m_material_id = m_current_shape_id;

                    // can be nullptr
                    auto material =
                        std::dynamic_pointer_cast<UberV2Material>(
                            shape->GetMaterial());

                    if (!material)
                    {
                        throw std::runtime_error(
                            "Application::UpdateGui(...): dynamic cast failure");
                    }

                    m_material_explorer = MaterialExplorer::Create(material);
                    m_object_name = shape->GetName();
                }
            }

            // Process double click event if it occured
            if (g_is_double_click)
            {
                m_shape_id_future = m_cl->GetShapeId((std::uint32_t)g_mouse_pos.x, (std::uint32_t)g_mouse_pos.y);
                g_is_double_click = false;
            }


            // draw material
            if (m_material_explorer)
            {
                ImVec2 explorer_win_size(win_size.x, win_size.y);
                bool status = m_material_explorer->DrawExplorer(explorer_win_size);
                update = update ? true : status;
            }
        }

        ImGui::Render();

        return update;
    }

    void Application::CollectSceneStats()
    {
        // Collect some scene statistics
        m_num_triangles = 0U;
        m_num_instances = 0U;
        {
            auto scene = m_cl->GetScene();
            auto shape_iter = scene->CreateShapeIterator();

            for (; shape_iter->IsValid(); shape_iter->Next())
            {
                auto shape = shape_iter->ItemAs<Baikal::Shape>();
                auto mesh = std::dynamic_pointer_cast<Baikal::Mesh>(shape);

                if (mesh)
                {
                    m_num_triangles += (int)(mesh->GetNumIndices() / 3);
                }
                else
                {
                    ++m_num_instances;
                }
            }
        }
    }

}
