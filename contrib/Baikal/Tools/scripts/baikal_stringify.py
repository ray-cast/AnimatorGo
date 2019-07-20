#!/usr/bin/env python
import sys
import os
import re

def make_header_list_recursive(file, files_to_headers_map):
    header_list = []
    file_includes = [incs for f, incs in files_to_headers_map if f == file]
    for i in file_includes[0]:
        header_list += make_header_list_recursive(i, files_to_headers_map)
        header_list += [i]
    # remove duplicates and save order
    return sorted(set(header_list), key=lambda x: header_list.index(x))

# generate variable name based on file and its type
def filevarname(file, typest):
    return 'g_'+ file.replace(dir, '').replace(ext, '').replace('/', '_') + '_' + typest

def print_file(filename, dir):
    fh = open(dir + filename)
    # include only new files
    header_list = []
    for line in fh.readlines():
        a = line.strip('\r\n')
        match = re.search('#include\s*<(.*/.+)>', a)
        if match:
            header_name = match.group(1)
            if header_name not in header_list:
                header_list.append(header_name)
        print('"' + a.replace('\\','\\\\').replace('"', '\\"') + ' \\n"\\')
    return header_list

def stringify(filename, dir, typest):
    print('static const char ' + filevarname(filename, typest) +'[] = \\')
    header_list = print_file(filename, dir)
    print(';\n')
    return header_list

argvs = sys.argv

if len(argvs) == 4:
    dir = argvs[1]
    ext = argvs[2]
    typest = argvs[3]
else:
	sys.error('Wrong argument count!')

files = []
for root, directories, filenames in os.walk(dir):
    for filename in filenames:
        name = os.path.join(os.path.relpath(root, dir), filename)
        name = name.replace('\\', '/')
        name = name.replace('./', '')
        files.append(name)


print('/* This is an auto-generated file. Do not edit manually! */\n')
print('#pragma once\n')
print('#include <unordered_map>\n')

# this will contain tuple(filename, include files)
files_to_headers_map = []
for file in files:
    if file.find(ext) == -1:
        continue
    header_list = stringify(file, dir, typest)
    files_to_headers_map.append((dir + file, header_list))

for file, headers in files_to_headers_map:
    if not headers:
        continue
    print('static const std::unordered_map<char const*, char const*> ' + filevarname(file, typest) + '_headers =\n{')
    for i in make_header_list_recursive(file, files_to_headers_map):
        print('    {"' + i + '", ' + filevarname(i, typest) + '},')
    print('};\n')
