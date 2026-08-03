#pragma once

#include <entt/entity/entity.hpp>
#include <glm/vec2.hpp>

namespace game::defs {

struct EnemyArriveBaseEvent // 敌人到达基地事件
{};

/// @brief 攻击（命中）事件
struct AttackEvent
{
    entt::entity m_attacker{ entt::null }; ///< @brief 攻击者
    entt::entity m_target{ entt::null };   ///< @brief 目标
    float m_damage{};                      ///< @brief 原始伤害
};

/// @brief 治疗（命中）事件
struct HealEvent
{
    entt::entity m_healer{ entt::null }; ///< @brief 治疗者
    entt::entity m_target{ entt::null }; ///< @brief 目标
    float m_amount{};                    ///< @brief 治疗量
};

/// @brief 发射投射物事件
struct EmitProjectileEvent
{
    entt::id_type m_id{ entt::null };    ///< @brief 投射物 ID
    entt::entity m_target{ entt::null }; ///< @brief 目标
    glm::vec2 m_startPosition{};         ///< @brief 起始位置
    glm::vec2 m_targetPosition{};        ///< @brief 目标位置
    float m_damage{};                    ///< @brief 伤害
};

} // namespace game::defs
