#include "entity_factory.h"
#include "../component/blocker_component.h"
#include "../component/class_name_component.h"
#include "../component/enemy_component.h"
#include "../component/player_component.h"
#include "../component/projectile_component.h"
#include "../component/stats_component.h"
#include "../data/entity_blueprint.h"
#include "../defs/tags.h"
#include "blueprint_manager.h"

#include "../../engine/component/animation_component.h"
#include "../../engine/component/audio_component.h"
#include "../../engine/component/render_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"

#include <entt/entity/registry.hpp>

#include <cmath>

using namespace entt::literals;

namespace game::factory {

EntityFactory::EntityFactory(entt::registry& registry, BlueprintManager& blueprintManager)
    : m_registry{ registry }
    , m_blueprintManager{ blueprintManager }
{}

entt::entity EntityFactory::createEnemyUnit(
    entt::id_type classId, glm::vec2 position, int targetPathNodeId, int level, int rarity)
{
    auto entity = m_registry.create();
    const auto& blueprint = m_blueprintManager.getEnemyClassBlueprint(classId);

    /* --- 添加组件 --- */
    // 添加变换组件
    addTransformComponent(entity, position);
    // 添加精灵组件
    addSpriteComponent(entity, blueprint.m_sprite);
    // 添加动画组件
    addAnimationComponent(entity, blueprint.m_animations, blueprint.m_sprite, "walk"_hs);
    // 添加音频组件
    addAudioComponent(entity, blueprint.m_sounds);
    // 添加属性组件
    addStatsComponent(entity, blueprint.m_stats, level, rarity);
    // 添加敌人组件
    addEnemyComponent(entity, blueprint.m_enemy, targetPathNodeId);
    // 补充其它必要组件
    m_registry.emplace<game::component::ClassNameComponent>(entity,
                                                            classId,
                                                            blueprint.m_displayInfo.m_name);
    m_registry.emplace<engine::component::RenderComponent>(entity); // 默认添加到主图层

    // TODO: 未来可添加其它组件

    return entity;
}

entt::entity EntityFactory::createPlayerUnit(entt::id_type classId,
                                             glm::vec2 position,
                                             int level,
                                             int rarity)
{
    auto entity = m_registry.create();
    const auto& blueprint = m_blueprintManager.getPlayerClassBlueprint(classId);

    /* --- 添加组件 --- */
    // 添加变换组件
    addTransformComponent(entity, position);
    // 添加精灵组件
    addSpriteComponent(entity, blueprint.m_sprite);
    // 添加动画组件
    addAnimationComponent(entity, blueprint.m_animations, blueprint.m_sprite, "walk"_hs);
    // 添加音频组件
    addAudioComponent(entity, blueprint.m_sounds);
    // 添加属性组件
    addStatsComponent(entity, blueprint.m_stats, level, rarity);
    // 添加玩家组件
    addPlayerComponent(entity, blueprint.m_player, rarity);
    // 补充其它必要组件
    m_registry.emplace<game::component::ClassNameComponent>(entity,
                                                            classId,
                                                            blueprint.m_displayInfo.m_name);
    m_registry.emplace<engine::component::RenderComponent>(entity); // 默认添加到主图层

    // TODO: 未来可添加其它组件

    return entity;
}

entt::entity EntityFactory::createProjectile(entt::id_type id,
                                             glm::vec2 startPosition,
                                             glm::vec2 targetPosition,
                                             entt::entity target,
                                             float damage)
{
    auto entity = m_registry.create();
    const auto& blueprint = m_blueprintManager.getProjectileBlueprint(id);

    /* --- 添加组件 --- */
    // 添加变换组件
    addTransformComponent(entity, startPosition);
    // 添加精灵组件
    addSpriteComponent(entity, blueprint.m_sprite);
    // 添加音频组件
    addAudioComponent(entity, blueprint.m_sounds);
    // 添加投射物组件
    m_registry.emplace<game::component::ProjectileComponent>(entity,
                                                             target,
                                                             damage,
                                                             startPosition,
                                                             targetPosition,
                                                             startPosition,
                                                             blueprint.m_arcHeight,
                                                             blueprint.m_totalFlightTime,
                                                             0.0F);
    // 添加渲染组件(让投射物位于主图层+1，即可以遮住角色)
    m_registry.emplace<engine::component::RenderComponent>(
        entity, engine::component::RenderComponent::MAIN_LAYER_INDEX + 1);

    // TODO: 未来可添加其它组件

    return entity;
}

void EntityFactory::addTransformComponent(entt::entity entity,
                                          glm::vec2 position,
                                          glm::vec2 scale,
                                          float rotation)
{
    m_registry.emplace<engine::component::TransformComponent>(entity, position, scale, rotation);
}

void EntityFactory::addSpriteComponent(entt::entity entity,
                                       const data::SpriteBlueprint& sprite,
                                       bool isFlipped)
{
    m_registry.emplace<engine::component::SpriteComponent>(entity,
                                                           engine::component::Sprite{
                                                               sprite.m_texturePath,
                                                               sprite.m_sourceRect,
                                                               isFlipped },
                                                           sprite.m_size,
                                                           sprite.m_offset);
    // 如果图片不是面向右侧, 添加朝左标签
    if (!sprite.m_isFacedRight) {
        m_registry.emplace<game::defs::FacedLeftTag>(entity);
    }
}

void EntityFactory::addAnimationComponent(
    entt::entity entity,
    const std::unordered_map<entt::id_type, data::AnimationBlueprint>& animationBlueprints,
    const data::SpriteBlueprint& spriteBlueprint,
    entt::id_type defaultAnimationId)
{
    // 先创建动画 map 容器
    std::unordered_map<entt::id_type, engine::component::Animation> animations;
    // 依次读取动画蓝图数据
    for (const auto& [animationId, animationBlueprint] : animationBlueprints) {
        // 创建动画帧容器
        std::vector<engine::component::AnimationFrame> frames;
        // 依次读取蓝图中的每一个帧索引
        for (const auto& frameIndex : animationBlueprint.m_frameIndices) {
            engine::utils::Rect sourceRect{ spriteBlueprint.m_sourceRect };
            // 通过索引计算每一帧的源矩形位置
            sourceRect.position.x += frameIndex * sourceRect.size.x;
            sourceRect.position.y += animationBlueprint.m_row * sourceRect.size.y;
            // 创建动画帧并填充到容器中
            frames.emplace_back(sourceRect, animationBlueprint.m_durationPerFrame);
        }

        // 使用填充好的动画帧容器创建动画, 并填充到动画 map 容器中（直接使用蓝图中的关键帧事件信息）
        engine::component::Animation animation{ std::move(frames), animationBlueprint.m_events };
        animations.emplace(animationId, animation);
    }

    // 使用动画 map 容器创建动画组件
    m_registry.emplace<engine::component::AnimationComponent>(entity,
                                                              std::move(animations),
                                                              defaultAnimationId);
}

void EntityFactory::addAudioComponent(entt::entity entity, const data::SoundsBlueprint& sounds)
{
    if (sounds.m_sounds.empty()) {
        return;
    }

    // 将 m_sounds 中的键值对保存到临时容器 audioMap 中
    std::unordered_map<entt::id_type, entt::id_type> audioMap;
    for (const auto& [soundNameId, soundPathId] : sounds.m_sounds) {
        audioMap.emplace(soundNameId, soundPathId);
    }

    m_registry.emplace<engine::component::AudioComponent>(entity, std::move(audioMap));
}

void EntityFactory::addStatsComponent(entt::entity entity,
                                      const data::StatsBlueprint& stats,
                                      int level,
                                      int rarity)
{
    auto hp = engine::utils::statModify(stats.m_hp, level, rarity);
    auto atk = engine::utils::statModify(stats.m_atk, level, rarity);
    auto def = engine::utils::statModify(stats.m_def, level, rarity);

    m_registry.emplace_or_replace<game::component::StatsComponent>(
        entity, hp, hp, atk, def, stats.m_range, stats.m_atkInterval, 0.0F, level, rarity);
}

void EntityFactory::addEnemyComponent(entt::entity entity,
                                      const data::EnemyBlueprint& enemy,
                                      int targetPathNodeId)
{
    m_registry.emplace<game::component::EnemyComponent>(entity, targetPathNodeId, enemy.m_speed);
    // 顺便添加速度组件
    m_registry.emplace<engine::component::VelocityComponent>(entity, glm::vec2(0.0F));

    // 添加远程或近战标签备用
    if (enemy.m_isRanged) {
        m_registry.emplace<game::defs::RangedUnitTag>(entity);
    } else {
        m_registry.emplace<game::defs::MeleeUnitTag>(entity);
    }
}

void EntityFactory::addPlayerComponent(entt::entity entity,
                                       const data::PlayerBlueprint& player,
                                       int rarity)
{
    // 根据稀有度调整玩家成本
    auto cost = static_cast<int>(std::round(player.m_cost * (0.9F + 0.1F * rarity)));
    m_registry.emplace<game::component::PlayerComponent>(entity, cost);

    // 添加玩家职业类型标签（近战、远程、治疗）
    if (player.m_type == game::defs::PlayerType::Melee) {
        m_registry.emplace<game::defs::MeleeUnitTag>(entity); // 近战单位标签
        // 近战类型添加阻挡者组件
        m_registry.emplace<game::component::BlockerComponent>(entity, player.m_block);
    } else if (player.m_type == game::defs::PlayerType::Ranged) {
        m_registry.emplace<game::defs::RangedUnitTag>(entity); // 远程单位标签
        if (player.m_isHealer) {
            m_registry.emplace<game::defs::HealerTag>(entity); // 治疗单位标签
        }
    }

    // TODO: 未来可以处理玩家职业类型为 PlayerType::Mixed 的情况
}

void EntityFactory::addProjectileIdComponent(entt::entity entity, entt::id_type id)
{
    if (id == entt::null) {
        return;
    }

    m_registry.emplace<game::component::ProjectileIdComponent>(entity, id);
}

} // namespace game::factory
