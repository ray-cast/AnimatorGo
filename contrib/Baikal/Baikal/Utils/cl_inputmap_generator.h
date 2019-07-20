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

#include <set>

#include "SceneGraph/scene1.h"
#include "SceneGraph/Collector/collector.h"
#include "SceneGraph/clwscene.h"

namespace Baikal
{
    class UberV2Material;
    class CLInputMapGenerator
    {
    public:
        /**
        * @brief Generates source code for input maps. 
        *
        * Code stored inside Generator object.
        * Makes lookups into leaf collectors to get parameters
        * Each input map will create single function that will output float4 value
        *
        * @param input_map_collector set of input maps for generation
        * @param input_map_leaf_collector list of leaf nodes that holds values
        */
        void Generate(const Collector& input_map_collector, const Collector& input_map_leaf_collector);

        // Returns generated source
        const std::string& GetGeneratedSource() const
        {
            return m_source_code;
        }

    private:
        // Proceed single input, writes function header and function call
        void GenerateSingleInput(std::shared_ptr<Baikal::InputMap> input, const Collector& input_map_leaf_collector);
        // Writes source code for single input map. Called recursively.
        void GenerateInputSource(std::shared_ptr<Baikal::InputMap> input, const Collector& input_map_leaf_collector);
        std::string m_source_code;
        std::string m_read_functions;
        std::string m_float4_selector;
        std::string m_float_selector;
        std::set<uint32_t> m_generated_inputs;
    };
}
