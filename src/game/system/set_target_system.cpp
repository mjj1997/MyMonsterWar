#include "set_target_system.h"
#include "../component/enemy_component.h"
#include "../component/player_component.h"
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

void SetTargetSystem::updatePlayerWithoutTarget(entt::registry& registry)
{
    // 筛选条件：无目标的玩家攻击型角色（排除治疗角色）
    auto playerWithoutTargetView = registry.view<game::component::PlayerComponent,
                                                 engine::component::TransformComponent,
                                                 game::component::StatsComponent>(
        entt::exclude<game::component::TargetComponent, game::defs::HealerTag>);

    // 获取所有敌方角色用于检测
    auto enemyView
        = registry.view<game::component::EnemyComponent, engine::component::TransformComponent>();

    // 遍历每一个无目标的玩家攻击型角色
    for (auto playerEntity : playerWithoutTargetView) {
        const auto& playerTransform = playerWithoutTargetView
                                          .get<engine::component::TransformComponent>(playerEntity);
        const auto& playerStats = playerWithoutTargetView.get<game::component::StatsComponent>(
            playerEntity);
        auto atkCheckingRadius = playerStats.m_range + game::defs::UNIT_RADIUS;

        // 检查是否有敌人在玩家的攻击范围内
        for (auto enemyEntity : enemyView) {
            const auto& enemyTransform = enemyView.get<engine::component::TransformComponent>(
                enemyEntity);
            if (engine::utils::distanceSquared(playerTransform.m_position, enemyTransform.m_position)
                <= atkCheckingRadius * atkCheckingRadius) {
                // 如果敌人在玩家的攻击范围内，则设置玩家的目标为该敌人
                registry.emplace<game::component::TargetComponent>(playerEntity, enemyEntity);
                spdlog::info("玩家 ID: {}, 设置目标 ID: {}",
                             entt::to_integral(playerEntity),
                             entt::to_integral(enemyEntity));
                break; // 设置一个目标敌人角色就停止检查
            }
        }
    }
}

void SetTargetSystem::updateEnemyWithoutTarget(entt::registry& registry)
{
    // 筛选条件：无目标的敌方角色（只考虑远程型，近战型敌人的目标就是阻挡者）
    auto enemyWithoutTargetView = registry.view<game::component::EnemyComponent,
                                                game::defs::RangedUnitTag,
                                                engine::component::TransformComponent,
                                                game::component::StatsComponent>(
        entt::exclude<game::component::TargetComponent>);

    // 获取所有玩家角色用于检测
    auto playerView
        = registry.view<game::component::PlayerComponent, engine::component::TransformComponent>();

    // 遍历每一个无目标的敌方角色
    for (auto enemyEntity : enemyWithoutTargetView) {
        const auto& enemyTransform = enemyWithoutTargetView
                                         .get<engine::component::TransformComponent>(enemyEntity);
        const auto& enemyStats = enemyWithoutTargetView.get<game::component::StatsComponent>(
            enemyEntity);
        auto atkCheckingRadius = enemyStats.m_range + game::defs::UNIT_RADIUS;

        // 检查是否有玩家在敌人的攻击范围内
        for (auto playerEntity : playerView) {
            const auto& playerTransform = playerView.get<engine::component::TransformComponent>(
                playerEntity);
            if (engine::utils::distanceSquared(enemyTransform.m_position, playerTransform.m_position)
                <= atkCheckingRadius * atkCheckingRadius) {
                // 如果玩家在敌人的攻击范围内，则设置敌人的目标为该玩家
                registry.emplace<game::component::TargetComponent>(enemyEntity, playerEntity);
                spdlog::info("敌人 ID: {}, 设置目标 ID: {}",
                             entt::to_integral(enemyEntity),
                             entt::to_integral(playerEntity));
                break; // 设置一个目标玩家角色就停止检查
            }
        }
    }
}

void SetTargetSystem::updateHealer(entt::registry& registry)
{
    /* --- 检查治疗角色（玩家）的目标，选择血量百分比最低的受伤玩家角色作为目标 --- */
    // 筛选条件：玩家治疗者角色
    auto healerView = registry.view<game::component::PlayerComponent,
                                    game::defs::HealerTag,
                                    engine::component::TransformComponent,
                                    game::component::StatsComponent>();

    // 获取所有受伤玩家角色用于检测
    auto injuredPlayerView = registry.view<game::component::PlayerComponent,
                                           game::defs::InjuredTag,
                                           engine::component::TransformComponent,
                                           game::component::StatsComponent>();

    // 遍历每一治疗角色
    for (auto healerEntity : healerView) {
        const auto& healerTransform = healerView.get<engine::component::TransformComponent>(
            healerEntity);
        const auto& healerStats = healerView.get<game::component::StatsComponent>(healerEntity);
        auto healCheckingRadius = healerStats.m_range + game::defs::UNIT_RADIUS;

        // --- 获取血量百分比最低的玩家角色 ---
        float lowestHpPercent{ 1.0F };                   // 保存最低血量百分比（初始为100%）
        entt::entity lowestHpPlayerEntity{ entt::null }; // 保存最低血量玩家角色（初始为null）

        // 遍历每一个受伤玩家角色
        for (auto injuredPlayerEntity : injuredPlayerView) {
            const auto& injuredPlayerTransform
                = injuredPlayerView.get<engine::component::TransformComponent>(injuredPlayerEntity);

            // 检查是否有受伤玩家在治疗角色的治疗范围内
            if (engine::utils::distanceSquared(healerTransform.m_position,
                                               injuredPlayerTransform.m_position)
                <= healCheckingRadius * healCheckingRadius) {
                // 计算血量百分比，更新最低血量百分比、最低血量玩家角色
                const auto& injuredPlayerStats
                    = injuredPlayerView.get<game::component::StatsComponent>(injuredPlayerEntity);
                auto hpPercent = static_cast<float>(injuredPlayerStats.m_hp
                                                    / injuredPlayerStats.m_maxHp);
                if (hpPercent < lowestHpPercent) {
                    lowestHpPercent = hpPercent;
                    lowestHpPlayerEntity = injuredPlayerEntity;
                }
            }
        }

        // 如果找到了最低血量玩家角色，则设置治疗角色的目标为该玩家角色
        if (lowestHpPlayerEntity != entt::null) {
            // 设置（更新）目标
            registry.emplace<game::component::TargetComponent>(healerEntity, lowestHpPlayerEntity);
        } else {
            // 否则移除目标（即使没有组件，也可以安全调用 remove）
            registry.remove<game::component::TargetComponent>(healerEntity);
        }
    }
}

} // namespace game::system
