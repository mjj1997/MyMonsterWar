#pragma once

#include <entt/entity/entity.hpp>

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

} // namespace game::defs
