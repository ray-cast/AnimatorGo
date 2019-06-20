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
#include "Application/app_utils.h"

#include <iostream>

namespace
{
    char const* kHelpMessage =
        "Baikal [-p path_to_models][-f model_name][-b][-r][-ns number_of_shadow_rays][-ao ao_radius][-w window_width][-h window_height][-nb number_of_indirect_bounces]";
}

namespace Baikal
{
    AppCliParser::AppCliParser(int argc, char * argv[])
    : m_cmd_parser(argc, argv)
    {   }

    AppSettings AppCliParser::Parse()
    {
        AppSettings s;

        s.path = m_cmd_parser.GetOption("-p", s.path);

        s.modelname = m_cmd_parser.GetOption("-f", s.modelname);

        s.envmapname = m_cmd_parser.GetOption("-e", s.envmapname);

        s.width = m_cmd_parser.GetOption("-w", s.width);

        s.height = m_cmd_parser.GetOption("-h", s.height);

        if (m_cmd_parser.OptionExists("-ao"))
        {
            s.ao_radius = m_cmd_parser.GetOption<float>("-ao");
            s.num_ao_rays = (int)(s.ao_radius);
            s.ao_enabled = true;
        }

        s.num_bounces = m_cmd_parser.GetOption("-nb", s.num_bounces);

        s.camera_pos.x = m_cmd_parser.GetOption("-cpx", s.camera_pos.x);

        s.camera_pos.y = m_cmd_parser.GetOption("-cpy", s.camera_pos.y);

        s.camera_pos.z = m_cmd_parser.GetOption("-cpz", s.camera_pos.z);

        s.camera_at.x = m_cmd_parser.GetOption("-tpx", s.camera_at.x);

        s.camera_at.y = m_cmd_parser.GetOption("-tpy", s.camera_at.y);

        s.camera_at.z = m_cmd_parser.GetOption("-tpz", s.camera_at.z);

        s.envmapmul = m_cmd_parser.GetOption("-em", s.envmapmul);

        s.num_samples = m_cmd_parser.GetOption("-ns", s.num_samples);

        s.camera_aperture = m_cmd_parser.GetOption("-a", s.camera_aperture);

        s.camera_focus_distance = m_cmd_parser.GetOption("-fd", s.camera_focus_distance);

        s.camera_focal_length = m_cmd_parser.GetOption("-fl", s.camera_focal_length);

        s.camera_out_folder = m_cmd_parser.GetOption("-output_cam", s.camera_out_folder);

        s.camera_sensor_size.x = m_cmd_parser.GetOption("-ssx", s.camera_sensor_size.x);

        s.camera_sensor_size.y = m_cmd_parser.GetOption("-ssy", s.camera_sensor_size.y);

        s.base_image_file_name = m_cmd_parser.GetOption("-ifn", s.base_image_file_name);

        s.image_file_format = m_cmd_parser.GetOption("-iff", s.image_file_format);

        s.light_file = m_cmd_parser.GetOption("-lights", s.light_file);

        if (m_cmd_parser.OptionExists("-ct"))
        {
            auto camera_type = m_cmd_parser.GetOption("-ct");

            if (camera_type == "perspective")
                s.camera_type = CameraType::kPerspective;
            else if (camera_type == "orthographic")
                s.camera_type = CameraType::kOrthographic;
            else
                throw std::runtime_error("Unsupported camera type");
        }

        s.interop = m_cmd_parser.GetOption("-interop", s.interop);

        s.cspeed = m_cmd_parser.GetOption("-cs", s.cspeed);

        if (m_cmd_parser.OptionExists("-config"))
        {
            auto cfg = m_cmd_parser.GetOption("-config");

            if (cfg == "cpu")
                s.mode = ConfigManager::Mode::kUseSingleCpu;
            else if (cfg == "gpu")
                s.mode = ConfigManager::Mode::kUseSingleGpu;
            else if (cfg == "mcpu")
                s.mode = ConfigManager::Mode::kUseCpus;
            else if (cfg == "mgpu")
                s.mode = ConfigManager::Mode::kUseGpus;
            else if (cfg == "all")
                s.mode = ConfigManager::Mode::kUseAll;
        }

        s.platform_index = m_cmd_parser.GetOption("-platform", s.platform_index);

        s.device_index = m_cmd_parser.GetOption("-device", s.device_index);

        if ((s.device_index >= 0) && (s.platform_index < 0))
        {
            std::cout <<
                "Can not set device index, because platform index was not specified" << std::endl;
        }

        if (m_cmd_parser.OptionExists("-r"))
        {
            s.progressive = true;
        }

        if (m_cmd_parser.OptionExists("-nowindow"))
        {
            s.cmd_line_mode = true;
        }

        return s;
    }

    void AppCliParser::ShowHelp()
    {
        std::cout << kHelpMessage << "\n";
    }

    AppSettings::AppSettings()
        : path("../Resources/CornellBox")
        , modelname("orig.objm")
        , envmapname("../Resources/Textures/studio015.hdr")
        //render
        , width(512)
        , height(512)
        , num_bounces(5)
        , num_samples(-1)
        , interop(true)
        , cspeed(10.25f)
        , mode(ConfigManager::Mode::kUseSingleGpu)
        //ao
        , ao_radius(1.f)
        , num_ao_rays(1)
        , ao_enabled(false)

        //camera
        , camera_pos(0.f, 1.f, 3.f)
        , camera_at(0.f, 1.f, 0.f)
        , camera_up(0.f, 1.f, 0.f)
        , camera_sensor_size(0.036f, 0.024f)  // default full frame sensor 36x24 mm
        , camera_zcap(0.0f, 100000.f)
        , camera_aperture(0.f)
        , camera_focus_distance(1.f)
        , camera_focal_length(0.035f) // 35mm lens
        , camera_type (CameraType::kPerspective)
        , camera_out_folder(".")

        //app
        , progressive(false)
        , cmd_line_mode(false)
        , recording_enabled(false)
        , benchmark(false)
        , gui_visible(true)
        , time_benchmarked(false)
        , rt_benchmarked(false)
        , time_benchmark(false)
        , time_benchmark_time(0.f)

        //imagefile
        , base_image_file_name("out")
        , image_file_format("png")

        //unused
        , num_shadow_rays(1)
        , samplecount(0)
        , envmapmul(1.f)
        , platform_index(-1)
        , device_index(-1)
    {
    }
}