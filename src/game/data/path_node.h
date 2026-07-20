#pragma once

#include <glm/vec2.hpp>

#include <vector>

namespace game::data {

/**
 * @brief 路径节点数据结构。
 * @note 包含节点ID、坐标和指向下一个节点的ID列表。
 */
struct PathNode
{
    int m_nodeId;
    glm::vec2 m_position;
    std::vector<int> m_nextNodeIds; // 指向下一个节点的ID列表
};

} // namespace game::data
