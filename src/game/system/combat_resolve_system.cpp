#include "combat_resolve_system.h"
#include "../component/blocked_by_component.h"
#include "../component/blocker_component.h"
#include "../component/enemy_component.h"
#include "../component/player_component.h"
#include "../component/stats_component.h"
#include "../defs/tags.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

#include <utility>

namespace game::system {

CombatResolveSystem::CombatResolveSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{
    m_dispatcher.sink<game::defs::AttackEvent>().connect<&CombatResolveSystem::handleAttackEvent>(
        this);
    m_dispatcher.sink<game::defs::HealEvent>().connect<&CombatResolveSystem::handleHealEvent>(this);
}

CombatResolveSystem::~CombatResolveSystem()
{
    m_dispatcher.disconnect(this);
}

void CombatResolveSystem::handleAttackEvent(const game::defs::AttackEvent& event)
{
    // 如果目标无效或目标已经被标记为死亡，直接返回
    if (!m_registry.valid(event.m_target)
        || m_registry.all_of<game::defs::DeadTag>(event.m_target)) {
        return;
    }

    // 根据伤害公式，让目标扣血
    auto& targetStats = m_registry.get<game::component::StatsComponent>(event.m_target);
    float damage{ calculateEffectiveDamage(event.m_damage, targetStats.m_def) };
    targetStats.m_hp -= damage;

    /* 如果目标是玩家 */
    if (m_registry.all_of<game::component::PlayerComponent>(event.m_target)) {
        spdlog::info("玩家 ID: {} 受到 ID: {} 的伤害, 剩余生命值: {}",
                     entt::to_integral(event.m_target),
                     entt::to_integral(event.m_attacker),
                     targetStats.m_hp);

        if (targetStats.m_hp <= 0) { // 玩家死亡情况
            targetStats.m_hp = 0;
            m_registry.emplace_or_replace<game::defs::DeadTag>(event.m_target);
            spdlog::info("玩家 ID: {} 死亡", entt::to_integral(event.m_target));
            // NOTE: 可添加死亡特效, 更新统计信息等
        } else if (targetStats.m_hp < targetStats.m_maxHp) { // 玩家受伤情况
            m_registry.emplace_or_replace<game::defs::InjuredTag>(event.m_target);
        }

        return;
    }

    /* 如果目标是敌人 */
    if (m_registry.all_of<game::component::EnemyComponent>(event.m_target)) {
        spdlog::info("敌人 ID: {} 受到 ID: {} 的伤害, 剩余生命值: {}",
                     entt::to_integral(event.m_target),
                     entt::to_integral(event.m_attacker),
                     targetStats.m_hp);

        if (targetStats.m_hp <= 0) { // 敌人死亡情况
            targetStats.m_hp = 0;
            m_registry.emplace_or_replace<game::defs::DeadTag>(event.m_target);
            spdlog::info("敌人 ID: {} 死亡", entt::to_integral(event.m_target));
            // TODO: 添加死亡特效
            // TODO: 更新统计信息

            // 如果敌人死亡时处于被阻挡状态，减少阻挡者的阻挡计数
            if (auto* blockedByComponent = m_registry.try_get<game::component::BlockedByComponent>(
                    event.m_target);
                blockedByComponent != nullptr) {
                auto blockerEntity = blockedByComponent->m_entity;
                if (m_registry.valid(blockerEntity)) {
                    auto& blockerComponent = m_registry.get<game::component::BlockerComponent>(
                        blockerEntity);
                    blockerComponent.m_currentCount = std::max(0,
                                                               blockerComponent.m_currentCount - 1);
                }
            }
        } else if (targetStats.m_hp < targetStats.m_maxHp) { // 敌人受伤情况
            m_registry.emplace_or_replace<game::defs::InjuredTag>(event.m_target);
        }

        return;
    }
}

void CombatResolveSystem::handleHealEvent(const game::defs::HealEvent& event)
{
    // 如果目标无效，直接返回
    if (!m_registry.valid(event.m_target)) {
        return;
    }

    // 如果目标不是玩家，直接返回
    if (!m_registry.all_of<game::component::PlayerComponent>(event.m_target)) {
        return;
    }

    // 根据治疗量，让目标回血
    auto& targetStats = m_registry.get<game::component::StatsComponent>(event.m_target);
    targetStats.m_hp += event.m_amount;
    spdlog::info("治疗者 ID: {}, 治疗目标 ID: {}, 治疗量: {}",
                 entt::to_integral(event.m_healer),
                 entt::to_integral(event.m_target),
                 event.m_amount);

    // 如果治疗后目标满血，移除受伤标签
    if (targetStats.m_hp >= targetStats.m_maxHp) {
        targetStats.m_hp = targetStats.m_maxHp;
        m_registry.remove<game::defs::InjuredTag>(event.m_target);
    }

    // TODO: 添加治疗特效
}

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
