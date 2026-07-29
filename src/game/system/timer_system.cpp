#include "timer_system.h"
#include "../component/stats_component.h"
#include "../defs/tags.h"

#include <entt/entity/registry.hpp>

namespace game::system {

void TimerSystem::update(entt::registry& registry, float deltaTime)
{
    updateAttackTimer(registry, deltaTime);
}

void TimerSystem::updateAttackTimer(entt::registry& registry, float deltaTime)
{
    // 筛选条件：有属性组件，但没有攻击就绪标签（即攻击正在冷却）
    auto view = registry.view<game::component::StatsComponent>(
        entt::exclude<game::defs::AttackReadyTag>);
    for (auto entity : view) {
        auto& stats = registry.get<game::component::StatsComponent>(entity);
        stats.m_atkTimer += deltaTime; // 推进攻击计时器
        // 如果攻击计时器超过攻击间隔，添加攻击就绪标签并重置攻击计时器
        if (stats.m_atkTimer >= stats.m_atkInterval) {
            registry.emplace_or_replace<game::defs::AttackReadyTag>(entity);
            stats.m_atkTimer = 0.0f; // 重置攻击计时器
        }
    }
}

} // namespace game::system
