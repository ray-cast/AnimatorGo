/**********************************************************************
Copyright (c) 2018 Advanced Micro Devices, Inc. All rights reserved.

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

#include "cmd_line_parser.h"

#include "utils.h"

#include <iostream>

namespace
{
    constexpr char const* kHelpMessage =
        "USAGE:\n\n"
        "Show this help message:\n"
        "    BaikalDataGeneratorApp [-help]\n\n"
        "List available devices:\n"
        "    BaikalDataGeneratorApp [-list_devices]\n\n"
        "Run data generation:\n"
        "    BaikalDataGeneratorApp -width OUTPUT_IMAGES_WIDTH -height OUTPUT_IMAGES_HEIGHT\n"
        "                           -scene_file SCENE_FILE_PATH -spp_file SPP_CONFIG_PATH\n"
        "                           -light_file LIGHT_CONFIG_PATH -camera_file CAMERA_CONFIG_PATH\n"
        "                           -output_dir OUTPUT_DIRECTORY [-device DEVICE_INDEX] [-gamma]\n"
        "                           [-split_num CAMERA_SUBSET_NUMBER [-split_idx USE_CAMERA_SUBSET]]\n"
        "                           [-start_output_idx CAMERA_START_OUTPUT_INDEX] [-nb BOUNCES_NUMBER]\n\n";
}

CmdLineParser::CmdLineParser(int argc, char* argv[])
    : m_cmd_parser(argc, argv)
{   }

AppConfig CmdLineParser::Parse() const
{
    AppConfig config;

    config.device_idx = m_cmd_parser.GetOption("-device", config.device_idx);

    config.light_file = m_cmd_parser.GetOption("-light_file");

    config.camera_file = m_cmd_parser.GetOption("-camera_file");

    config.output_dir = m_cmd_parser.GetOption("-output_dir");

    config.scene_file = m_cmd_parser.GetOption("-scene_file");

    config.spp_file = m_cmd_parser.GetOption("-spp_file");

    config.width = m_cmd_parser.GetOption<unsigned>("-width");

    config.height = m_cmd_parser.GetOption<unsigned>("-height");

    config.split_num = m_cmd_parser.GetOption("-split_num", config.split_num);

    config.split_idx = m_cmd_parser.GetOption("-split_idx", config.split_idx);

    config.start_output_idx = m_cmd_parser.GetOption("-start_output_idx", config.start_output_idx);

    config.gamma_correction = m_cmd_parser.OptionExists("-gamma");

    config.num_bounces = m_cmd_parser.GetOption("-nb", config.num_bounces);

    return config;
}

bool CmdLineParser::HasListDevicesOption() const
{
    return m_cmd_parser.OptionExists("-list_devices");
}

void CmdLineParser::ShowHelp() const
{
    std::cout << kHelpMessage;
}

bool CmdLineParser::HasHelpOption() const
{
    return m_cmd_parser.OptionExists("-help");
}