#pragma once

#include <entt/entity/entity.hpp>
#include <glm/vec2.hpp>

namespace game::component {

/**
 *  @brief 投射物组件, 附加在投射物实体上
 */
struct ProjectileComponent
{
    entt::entity m_target{ entt::null }; ///< @brief 目标实体
    float m_damage{};                    ///< @brief 伤害
    glm::vec2 m_startPosition{};         ///< @brief 起始位置
    glm::vec2 m_targetPosition{};        ///< @brief 目标位置
    glm::vec2 m_previousPosition{};      ///< @brief 上一帧位置（用于计算朝向）
    float m_arcArcHeight{};              ///< @brief 弧度高度(即正弦函数振幅)
    float m_totalFlightTime{};           ///< @brief 总飞行时间
    float m_currentFlightTime{};         ///< @brief 当前飞行时间
};

} // namespace game::component
