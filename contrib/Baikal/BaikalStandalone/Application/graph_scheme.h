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

#include "uber_tree.h"

class GraphScheme
{
public:
    using Ptr = std::shared_ptr<GraphScheme>;

    struct Link
    {
        std::uint32_t src_id;
        std::uint32_t dst_id;

        Link(std::uint32_t src_id_, std::uint32_t dst_id_)
        : src_id(src_id_), dst_id(dst_id_)
        { }
    };

    enum class NodeType
    {
        kIntermidiate = 0,
        kFloat,
        kFloat3,
        kTexture
    };

    class Node
    {
    public:
        Node(UberNode::Ptr node,
             RadeonRays::int2 pos,
             RadeonRays::int2 size);

        float GetFloat() const;
        void SetFloat(float value);
        RadeonRays::float3 GetFloat3() const;
        void SetFloat3(RadeonRays::float3 value);
        void SetTexture(Baikal::Texture::Ptr texture);

        // fields
        std::uint32_t id;
        NodeType type;
        RadeonRays::int2 pos; // position of the top left corner
        RadeonRays::int2 size;
        std::string name;

    private:
        UberNode::Ptr node;
    };

    static Ptr Create(UberTree::Ptr tree,
                      RadeonRays::int2 root_pos = RadeonRays::int2(0, 0),
                      RadeonRays::int2 node_size = RadeonRays::int2(120, 60));

    void RemoveNode(std::uint32_t id);
    void UpdateNodePos(std::uint32_t id, RadeonRays::int2 pos);
    void UpdateNodeSize(std::uint32_t id, RadeonRays::int2 size);
    void AddTree(UberTree::Ptr tree);
    void MergeTrees(UberTree::Ptr tree_1, UberTree::Ptr tree_2);

    const std::vector<Node>& GetNodes() const
    { return m_nodes; }

    const std::vector<Link>& GetLinks() const
    { return m_links; }

protected:
    GraphScheme(UberTree::Ptr tree,
                RadeonRays::int2 root_pos,
                RadeonRays::int2 node_size);

private:
    void RemoveLink(std::uint32_t src_id, std::uint32_t dst_id);
    void RecomputeCoordinates(RadeonRays::int2 root_pos,
                              RadeonRays::int2 node_size);

    std::vector<UberTree::Ptr> m_trees;
    std::vector<Node> m_nodes;
    std::vector<Link> m_links;
};