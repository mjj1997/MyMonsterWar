#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::system {

/**
 * @brief 战斗结算系统，用于处理战斗结算逻辑
 * 
 * 根据收到的事件（攻击、治疗），执行相应的结算操作。
 */
class CombatResolveSystem
{
public:
    CombatResolveSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~CombatResolveSystem();

private:
    /* --- 辅助函数 --- */
    /**
     * @brief 计算有效伤害（计算公式可修改）
     * 当前计算公式：攻击力 - 防御力，最小伤害为攻击力的 10%
     */
    float calculateEffectiveDamage(float attackerAtk, float targetDef);

    entt::registry& m_registry;
    entt::dispatcher& m_dispatcher;
};

} // namespace game::system
