#include "set_target_system.h"
#include "../component/stats_component.h"
#include "../component/target_component.h"
#include "../defs/constants.h"
#include "../defs/tags.h"

#include "../../engine/component/transform_component.h"
#include "../../engine/utils/math.h"

#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

namespace game::system {

void SetTargetSystem::update(entt::registry& registry) {}

void SetTargetSystem::updateEntityWithTarget(entt::registry& registry)
{
    // 筛选条件：有目标的敌我双方所有攻击型角色（排除治疗角色，治疗角色在 updateHealer 中处理）
    auto entityWithTargetView = registry.view<game::component::TargetComponent,
                                              game::component::StatsComponent,
                                              engine::component::TransformComponent>(
        entt::exclude<game::defs::HealerTag>);

    // 遍历每一个有目标组件的角色
    for (auto entity : entityWithTargetView) {
        const auto& entityTarget = entityWithTargetView.get<game::component::TargetComponent>(
            entity);
        // 检查目标组件指向的实体是否有效
        if (!registry.valid(entityTarget.m_entity)) {
            // 如果目标组件指向的实体无效，清除目标组件
            registry.remove<game::component::TargetComponent>(entity);
            spdlog::info("ID: {}，目标 ID: {} 无效，清除目标",
                         entt::to_integral(entity),
                         entt::to_integral(entityTarget.m_entity));
            continue;
        }

        // 检查目标是否还在攻击范围内（检测半径 = 角色攻击范围 + 角色自身半径）
        const auto& entityStats = registry.get<game::component::StatsComponent>(entity);
        auto atkCheckingRadius = entityStats.m_range + game::defs::UNIT_RADIUS;
        const auto& entityTransform = registry.get<engine::component::TransformComponent>(entity);
        const auto& targetTransform = registry.get<engine::component::TransformComponent>(
            entityTarget.m_entity);
        if (engine::utils::distanceSquared(entityTransform.m_position, targetTransform.m_position)
            > atkCheckingRadius * atkCheckingRadius) {
            // 如果目标在攻击范围外，则清除目标
            registry.remove<game::component::TargetComponent>(entity);
            spdlog::info("ID: {}, 目标 ID: {}, 不在攻击范围之内, 清除目标",
                         entt::to_integral(entity),
                         entt::to_integral(entityTarget.m_entity));
            continue;
        }
    }
}

} // namespace game::system
