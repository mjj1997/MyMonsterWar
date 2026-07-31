#include "combat_resolve_system.h"

namespace game::system {

CombatResolveSystem::CombatResolveSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{}

CombatResolveSystem::~CombatResolveSystem() {}

/* --- 辅助函数 --- */
float CombatResolveSystem::calculateEffectiveDamage(float attackerAtk, float targetDef)
{
    // 有效伤害 = 攻击力 - 防御力
    float damage{ attackerAtk - targetDef };
    // 最小伤害为攻击力的 10%
    damage = std::max(damage, 0.1F * attackerAtk);
    return damage;
}

} // namespace game::system
