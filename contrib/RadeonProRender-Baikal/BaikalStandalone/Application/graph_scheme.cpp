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

#include "graph_scheme.h"

////////////////////////////////////////////////
//// GraphScheme implemenataion
////////////////////////////////////////////////

using namespace Baikal;

namespace {
    struct GraphSchemeConcrete : public GraphScheme
    {
        GraphSchemeConcrete(UberTree::Ptr tree, RadeonRays::int2 root_pos, RadeonRays::int2 node_size)
        : GraphScheme(tree, root_pos, node_size)
        { }
    };
}

GraphScheme::GraphScheme(UberTree::Ptr tree, RadeonRays::int2 root_pos, RadeonRays::int2 node_size)
{
    if (!tree)
    {
        throw std::logic_error("GraphScheme::GraphScheme(...): 'tree' is nullptr");
    }

    m_trees.push_back(tree);
    RecomputeCoordinates(root_pos, node_size);
}

GraphScheme::Ptr GraphScheme::Create(UberTree::Ptr tree, RadeonRays::int2 root_pos, RadeonRays::int2 node_size)
{
    return std::make_shared<GraphScheme>(GraphSchemeConcrete(
                                         tree, root_pos, node_size));
}

void GraphScheme::RecomputeCoordinates(RadeonRays::int2 root_pos, RadeonRays::int2 node_size)
{
    const int x_offset = 180;
    const int y_offset = 120;

    // primary tree
    auto iter = UberTreeIterator::Create(m_trees[0]);

    int level = 0;
    int level_item_counter = 0;
    while (iter->IsValid())
    {
        level_item_counter = (iter->GetLevel() == level) ?
            (level_item_counter + 1) : (0);
        level = iter->GetLevel();

        RadeonRays::int2 position(
            root_pos.x + level * x_offset,
            root_pos.y + level_item_counter * y_offset);

        m_nodes.push_back(Node(iter->Item(), position, node_size));

        if (iter->Item()->GetParentId() >= 0)
        {
            m_links.push_back(
                Link(iter->Item()->GetParentId(),
                     iter->Item()->GetId()));
        }

        iter->Next();
    }
}

void GraphScheme::RemoveLink(std::uint32_t src_id, std::uint32_t dst_id)
{
    for (auto link = m_links.begin(); link != m_links.end(); ++link)
    {
        if (link->src_id == src_id &&
            link->dst_id == dst_id)
        {
            m_links.erase(link);
            return;
        }
    }
}

void GraphScheme::AddTree(UberTree::Ptr tree)
{
    throw std::runtime_error("Not implemented");
}

void GraphScheme::MergeTrees(UberTree::Ptr tree_1, UberTree::Ptr tree_2)
{
    throw std::runtime_error("Not implemented");
}

void GraphScheme::RemoveNode(std::uint32_t id)
{
    UberTree::Ptr tree = nullptr;
    UberNode::Ptr node = nullptr;
    // find tree
    for (auto item : m_trees)
    {
        // nullptr if 'id' wasn't found
        node = item->Find(id);
        if (node)
        {
            tree = item;
            break;
        }
    }
    // thre is no such id
    if (!tree)
        return;

    auto trees_vector = tree->ExcludeNode(id);

    RemoveLink(node->GetParentId(), id);

    std::remove_if(m_links.begin(), m_links.end(),
        [id](const Link &link)
        {
            return link.src_id == id;
        });

    std::remove_if(m_nodes.begin(), m_nodes.end(),
        [id](const Node &node)
        {
            return node.id == id;
        });

    m_trees.insert(m_trees.end(), trees_vector.begin(), trees_vector.end());
}

void GraphScheme::UpdateNodePos(std::uint32_t id, RadeonRays::int2 pos)
{
    for (auto& node : m_nodes)
    {
        if (node.id == id)
        {
            node.pos.x = pos.x;
            node.pos.y = pos.y;
        }
    }
}

void GraphScheme::UpdateNodeSize(std::uint32_t id, RadeonRays::int2 size)
{
    for (auto& node : m_nodes)
    {
        if (node.id == id)
        {
            node.size.x = size.x;
            node.size.y = size.y;
        }
    }
}

////////////////////////////////////////////////
//// Node implemenataion
////////////////////////////////////////////////

GraphScheme::Node::Node(
    UberNode::Ptr node_,
    RadeonRays::int2 pos_,
    RadeonRays::int2 size_)
    // initialize list
    : id(node_->GetId()), pos(pos_), size(size_),
      name (node_->GetDataTypeText()), node(node_)
{
    switch (node->GetDataType())
    {
    case InputMap::InputMapType::kConstantFloat:
        type = NodeType::kFloat;
        return;
    case InputMap::InputMapType::kConstantFloat3:
        type = NodeType::kFloat3;
        return;
    case InputMap::InputMapType::kSampler:
        type = NodeType::kTexture;
        return;
    default:
        type = NodeType::kIntermidiate;
    }
}

float GraphScheme::Node::GetFloat() const
{
    if (node->GetDataType() != InputMap::InputMapType::kConstantFloat)
    {
        return 0.f;
    }

    auto float_node = std::dynamic_pointer_cast<UberNode_Float>(node);

    if (!float_node)
    {
        throw std::runtime_error("Node::GetFloat(...): dynamic_pointer_cast failure");
    }

    return float_node->GetValue();
}

void GraphScheme::Node::SetFloat(float value)
{
    if (node->GetDataType() != InputMap::InputMapType::kConstantFloat)
    {
        return;
    }

    auto float_node = std::dynamic_pointer_cast<UberNode_Float>(node);

    if (!float_node)
    {
        throw std::runtime_error(
            "Node::SetFloat(...): dynamic_pointer_cast failure");
    }

    float_node->SetValue(value);
}

RadeonRays::float3 GraphScheme::Node::GetFloat3() const
{
    if (node->GetDataType() != InputMap::InputMapType::kConstantFloat3)
    {
        return RadeonRays::float3(0);
    }

    auto float_node = std::dynamic_pointer_cast<UberNode_Float3>(node);

    if (!float_node)
    {
        throw std::runtime_error(
            "Node::GetFloat3(...): dynamic_pointer_cast failure");
    }

    return float_node->GetValue();
}

void GraphScheme::Node::SetFloat3(RadeonRays::float3 value)
{
    if (node->GetDataType() != InputMap::InputMapType::kConstantFloat3)
    {
        return;
    }

    auto float_node = std::dynamic_pointer_cast<UberNode_Float3>(node);

    if (!float_node)
    {
        throw std::runtime_error("Node::SetFloat3(...): dynamic_pointer_cast failure");
    }

    float_node->SetValue(value);
}

void GraphScheme::Node::SetTexture(Texture::Ptr texture)
{
    if (node->GetDataType() != InputMap::InputMapType::kSampler)
    {
        return;
    }

    auto texture_node = std::dynamic_pointer_cast<UberNode_Sampler>(node);

    if (!texture_node)
    {
        throw std::runtime_error("Node::SetTexture(...): dynamic_pointer_cast failure");
    }

    texture_node->SetValue(texture);
}