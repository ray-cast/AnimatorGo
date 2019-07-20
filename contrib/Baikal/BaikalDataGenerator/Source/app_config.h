#pragma once

#include "filesystem.h"

#include <vector>

const int kDefaultStartOutputIndex = -1;

struct AppConfig
{
    std::filesystem::path scene_file;
    std::filesystem::path light_file;
    std::filesystem::path camera_file;
    std::filesystem::path spp_file;
    std::filesystem::path output_dir;
    unsigned width = 0;
    unsigned height = 0;
    unsigned split_num = 1;
    unsigned split_idx = 0;
    int start_output_idx = kDefaultStartOutputIndex;
    unsigned num_bounces = 5;
    unsigned device_idx = 0;
    bool gamma_correction = false;
};
