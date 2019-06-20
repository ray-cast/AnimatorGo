
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

#include <functional>
#include <queue>
#include "uber_node.h"
#include "SceneGraph/iterator.h"

class UberTreeIterator;

class UberTree
{
    friend class UberTreeIterator;

public:
    using Ptr = std::shared_ptr<UberTree>;
    using InputMap = UberNode::InputMap;

    static Ptr Create(InputMap::Ptr input_map);

    // 'id' is an id ofthe node to add subtree ('node' or 'tree' arg)
    // 'arg_number' is a number of the argument in
    // parent node (id of this node is argument "id") which will be set
    // this methods garanty adding items in Ubertree model,
    // and only if input value and tree itself are valid than update baikal material
    // Note: 'tree' arg will be nullptr if adding was successful
    bool AddTree(std::uint32_t id, std::uint32_t arg_number, UberTree::Ptr tree);

    bool IsValid() const;

    UberNode::Ptr Find(std::uint32_t id);

    // returns vector of the new trees (primary tree not included)
    std::vector<UberTree::Ptr> ExcludeNode(std::uint32_t id);

protected:

    UberTree(InputMap::Ptr input_map);
    UberTree(std::vector<UberNode::Ptr> nodes);

private:
    void BuildTree(InputMap::Ptr input_map);

    std::vector<UberNode::Ptr> m_nodes;
};

class UberTreeIterator
{
public:
    using Ptr = std::shared_ptr<UberTreeIterator>;

    static Ptr Create(UberTree::Ptr tree);

    // operations
    bool IsValid() const;
    void Next();
    UberNode::Ptr Item() const;
    void Reset();

    // accessor
    int GetLevel() const;

protected:
    UberTreeIterator(UberTree::Ptr tree);

private:
    UberTree::Ptr m_tree;
    std::queue<std::pair<int, UberNode::Ptr>> m_queue;
};