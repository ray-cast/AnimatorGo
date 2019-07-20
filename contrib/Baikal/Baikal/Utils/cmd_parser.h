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

#pragma once

#include <utility>
#include <sstream>
#include <string>
#include <vector>


namespace Baikal
{

    class CmdParser
    {
    public:
        // 'argc' - number of the command line arguments
        // 'argv' - command line arguments string
        CmdParser(int argc, char* argv[]);

        // 'option' - name of the option in command line
        // returns true in case there's exist
        bool OptionExists(const std::string& option) const;

        // 'option' - name of the option in command line
        // return the value of the option (the format of the strign should be 'option' 'value')
        template <class T = std::string>
        T GetOption(const std::string& option) const
        {
            auto value = GetOptionValue(option);

            if (!value)
            {
                std::stringstream ss;

                ss << __func__ << ": "
                    << option << " :option or its value is missed";

                throw std::logic_error(ss.str());
            }

            return LexicalCast<std::remove_cv_t<std::remove_reference_t<T>>>(*value);
        }

        // 'option' - name of the option in command line
        // 'default_value' - default value if option wasn't specified
        // return the value of the option (the format of the strign should be 'option' 'value') or 'default_value'
        template <class T = std::string>
        std::decay_t<T> GetOption(const std::string& option, T default_value) const
        {
            auto value = GetOptionValue(option);

            if (!value)
            {
                return std::forward<T>(default_value);
            }
            else
            {
                return LexicalCast<std::decay_t<T>>(*value);
            }
        }

    private:
        const std::string* GetOptionValue(const std::string& option) const;

        template <typename T>
        static T LexicalCast(const std::string& str)
        {
            T var;

            std::istringstream iss(str);

            if (iss.fail())
            {
                throw std::logic_error(std::string(__func__) + "cmd stream failed");
            }

            iss >> var;
            return var;
        }

        std::vector<std::string> m_cmd_line;
    };

}