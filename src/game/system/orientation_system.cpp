#include "orientation_system.h"
#include "../component/blocked_by_component.h"
#include "../component/enemy_component.h"
#include "../component/target_component.h"
#include "../defs/tags.h"

#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"

#include <entt/entity/registry.hpp>

namespace game::system {

void OrientationSystem::update(entt::registry& registry)
{
    updateEntityWithTarget(registry);
    updateEnemyBlocked(registry);
    updateEnemyMoving(registry);
}

void OrientationSystem::updateEntityWithTarget(entt::registry& registry)
{
    /* 面朝目标 */
    // 筛选依据：有目标的角色（包括敌人和玩家）
    auto view = registry.view<game::component::TargetComponent,
                              engine::component::TransformComponent,
                              engine::component::SpriteComponent>();
    for (auto entity : view) {
        const auto& entityTarget = view.get<game::component::TargetComponent>(entity);
        const auto& targetTransform = view.get<engine::component::TransformComponent>(
            entityTarget.m_entity);
        const auto& entityTransform = view.get<engine::component::TransformComponent>(entity);
        auto& entitySprite = view.get<engine::component::SpriteComponent>(entity);
        // 如果目标位置.x > 自身位置.x，说明目标位于自身的右侧
        bool isFacedRight{ targetTransform.m_position.x > entityTransform.m_position.x };
        // 使用 all_of 检查实体是否拥有指定标签（FacedLeftTag）
        if (registry.all_of<game::defs::FacedLeftTag>(entity)) {
            entitySprite.m_sprite.m_isFlipped = isFacedRight;
        } else {
            entitySprite.m_sprite.m_isFlipped = !isFacedRight;
        }
    }
}

void OrientationSystem::updateEnemyBlocked(entt::registry& registry)
{
    /* 面朝阻挡者 */
    // 筛选依据：被阻挡的敌方角色
    auto view = registry.view<game::component::BlockedByComponent,
                              engine::component::TransformComponent,
                              engine::component::SpriteComponent>();
    for (auto entity : view) {
        const auto& entityBlockedBy = view.get<game::component::BlockedByComponent>(entity);
        const auto& blockedByTransform = view.get<engine::component::TransformComponent>(
            entityBlockedBy.m_entity);
        const auto& entityTransform = view.get<engine::component::TransformComponent>(entity);
        auto& entitySprite = view.get<engine::component::SpriteComponent>(entity);
        // 如果阻挡者位置.x > 自身位置.x，说明阻挡者位于自身的右侧
        bool isFacedRight{ blockedByTransform.m_position.x > entityTransform.m_position.x };
        // 使用 all_of 检查实体是否拥有指定标签（FacedLeftTag）
        if (registry.all_of<game::defs::FacedLeftTag>(entity)) {
            entitySprite.m_sprite.m_isFlipped = isFacedRight;
        } else {
            entitySprite.m_sprite.m_isFlipped = !isFacedRight;
        }
    }
}

void OrientationSystem::updateEnemyMoving(entt::registry& registry)
{
    /* 面朝移动方向 */
    // 筛选依据：移动中的敌方角色（排除被阻挡、正在进行动作的角色）
    auto view = registry.view<game::component::EnemyComponent,
                              engine::component::VelocityComponent,
                              engine::component::SpriteComponent>(
        entt::exclude<game::component::BlockedByComponent, game::defs::ActionLockedTag>);
    for (auto entity : view) {
        const auto& entityVelocity = view.get<engine::component::VelocityComponent>(entity);
        auto& entitySprite = view.get<engine::component::SpriteComponent>(entity);
        // 如果移动速度.x > 0，说明移动方向为右侧
        bool isFacedRight{ entityVelocity.m_velocity.x > 0.0F };
        // 使用 all_of 检查实体是否拥有指定标签（FacedLeftTag）
        if (registry.all_of<game::defs::FacedLeftTag>(entity)) {
            entitySprite.m_sprite.m_isFlipped = isFacedRight;
        } else {
            entitySprite.m_sprite.m_isFlipped = !isFacedRight;
        }
    }
}

} // namespace game::system
