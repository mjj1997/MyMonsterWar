#pragma once

namespace game::component {

/**
 * @brief 敌人组件, 包含目标路径节点ID和自身移动速度
 */
struct EnemyComponent
{
    int m_targetPathNodeId;
    float m_speed;
};

} // namespace game::component
