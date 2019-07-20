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

#include "app_config.h"

#include <Baikal/Utils/cmd_parser.h>

#include <string>


class CmdLineParser
{
public:

    // 'argc' - number of the command line arguments
    // 'argv' - command line arguments string
    CmdLineParser(int argc, char* argv[]);

    // Generate AppConfig from internal string
    AppConfig Parse() const;

    // check whether device list is requested
    bool HasListDevicesOption() const;

    // check if there is '-help' option
    // returns true if there's a help option, false otherwise
    bool HasHelpOption() const;

    // print help option
    void ShowHelp() const;

private:
    Baikal::CmdParser m_cmd_parser;
};