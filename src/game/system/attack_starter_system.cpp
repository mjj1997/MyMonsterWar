#include "attack_starter_system.h"
#include "../component/blocked_by_component.h"
#include "../component/enemy_component.h"
#include "../component/target_component.h"
#include "../defs/tags.h"

#include "../../engine/component/velocity_component.h"
#include "../../engine/utils/events.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

using namespace entt::literals;

namespace game::system {

void AttackStarterSystem::update(entt::registry& registry, entt::dispatcher& dispatcher) {}

void AttackStarterSystem::updateEnemyBlocked(entt::registry& registry, entt::dispatcher& dispatcher)
{
    // 筛选条件：被阻挡的敌人角色，且攻击就绪
    auto blockedEnenmyView = registry.view<game::component::EnemyComponent,
                                           game::component::BlockedByComponent,
                                           game::defs::AttackReadyTag>();

    for (auto enemyEntity : blockedEnenmyView) {
        // 添加动作锁定标签（确保攻击动画执行完毕再进行其它动作）
        registry.emplace_or_replace<game::defs::ActionLockedTag>(enemyEntity);
        // 每次攻击后，移除攻击就绪标签，攻击冷却重新计时
        registry.remove<game::defs::AttackReadyTag>(enemyEntity);
        dispatcher.enqueue(engine::utils::PlayAnimationEvent{ enemyEntity, "attack"_hs, false });
    }
}

void AttackStarterSystem::updateEnemyRanged(entt::registry& registry, entt::dispatcher& dispatcher)
{
    // 筛选条件：有目标的远程敌人角色，未被阻挡，且攻击就绪
    auto rangedEnenmyView = registry.view<game::component::EnemyComponent,
                                          game::component::TargetComponent,
                                          game::defs::AttackReadyTag>(
        entt::exclude<game::component::BlockedByComponent>);

    for (auto enemyEntity : rangedEnenmyView) {
        registry.emplace_or_replace<game::defs::ActionLockedTag>(enemyEntity);
        // 清空速度, 防止出现边打边移动的情况（对于体积很小的组件，可以直接构造替换，不必“获取 + 修改”）
        registry.emplace_or_replace<engine::component::VelocityComponent>(enemyEntity,
                                                                          glm::vec2(0.0F));
        registry.remove<game::defs::AttackReadyTag>(enemyEntity);
        dispatcher.enqueue(
            engine::utils::PlayAnimationEvent{ enemyEntity, "ranged_attack"_hs, false });
    }
}

} // namespace game::system
