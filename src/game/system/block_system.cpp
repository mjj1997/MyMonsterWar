#include "block_system.h"
#include "../component/blocked_by_component.h"
#include "../component/blocker_component.h"
#include "../component/enemy_component.h"
#include "../defs/constants.h"
#include "../defs/tags.h"

#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"
#include "../../engine/utils/events.h"
#include "../../engine/utils/math.h"

#include <entt/entity/registry.hpp>
#include <entt/entity/view.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::system {

void BlockSystem::update(entt::registry& registry, entt::dispatcher& dispatcher)
{
    spdlog::trace("BlockSystem::update");

    /* --- 检查阻挡者是否依然有效 --- */
    auto blockedByView = registry.view<game::component::BlockedByComponent>();
    for (auto blockedByEntity : blockedByView) {
        auto& blockedByComponent = blockedByView.get<game::component::BlockedByComponent>(
            blockedByEntity);
        // 如果被阻挡组件指向的阻挡者实体无效（比如死亡），则移除被阻挡组件
        if (!registry.valid(blockedByComponent.m_entity)) {
            registry.remove<game::component::BlockedByComponent>(blockedByEntity);
            spdlog::info("阻挡者 ID：{} 无效，移除被阻挡组件",
                         entt::to_integral(blockedByComponent.m_entity));

            // 移除可能存在的动作锁定标签
            registry.remove<game::defs::ActionLockedTag>(blockedByEntity);
            // 播放动画“walk”
            dispatcher.enqueue<engine::utils::PlayAnimationEvent>(blockedByEntity, "walk"_hs, true);
        }
    }

    /* --- 判断敌人实体是否需要添加被阻挡组件 --- */
    // 获取所有阻挡者
    auto blockerView
        = registry.view<game::component::BlockerComponent, engine::component::TransformComponent>();
    // 获取所有敌人，使用 entt:exclude 排除“包含指定组件的实体”（已经存在阻挡者组件的敌人不需要再添加）
    auto enemyView = registry.view<game::component::EnemyComponent,
                                   engine::component::TransformComponent,
                                   engine::component::VelocityComponent>(
        entt::exclude<game::component::BlockedByComponent>);

    // 遍历所有敌人
    for (auto enemyEntity : enemyView) {
        const auto& enemyTransform = enemyView.get<engine::component::TransformComponent>(
            enemyEntity);
        auto& enemyVelocity = enemyView.get<engine::component::VelocityComponent>(enemyEntity);

        // 每个敌人都遍历所有阻挡者，检查是否被阻挡
        for (auto blockerEntity : blockerView) {
            const auto& blockerTransform = blockerView.get<engine::component::TransformComponent>(
                blockerEntity);
            auto& blockerComponent = blockerView.get<game::component::BlockerComponent>(
                blockerEntity);

            // 如果被阻挡（通过比较敌人和阻挡者之间的距离是否小于阻挡判断半径）
            if (engine::utils::distanceSquared(enemyTransform.m_position,
                                               blockerTransform.m_position)
                < game::defs::BLOCK_RADIUS * game::defs::BLOCK_RADIUS) {
                // 检查阻挡者是否还能阻挡
                if (blockerComponent.m_currentCount >= blockerComponent.m_maxCount) {
                    continue; // 如果不能阻挡，则跳过
                }
                ++blockerComponent.m_currentCount;          // 增加阻挡者当前阻挡数量
                enemyVelocity.m_velocity = glm::vec2(0.0F); // 设置敌人速度为 0，停止移动

                // 给敌人添加被阻挡组件
                registry.emplace<game::component::BlockedByComponent>(enemyEntity, blockerEntity);
                spdlog::info("敌人 ID：{} 被阻挡，阻挡者 ID：{}",
                             entt::to_integral(enemyEntity),
                             entt::to_integral(blockerEntity));
            }
        }
    }
}

} // namespace game::system
