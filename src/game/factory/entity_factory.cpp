#include "entity_factory.h"
#include "../component/class_name_component.h"
#include "../component/enemy_component.h"
#include "../component/stats_component.h"
#include "../data/entity_blueprint.h"
#include "blueprint_manager.h"

#include "../../engine/component/animation_component.h"
#include "../../engine/component/audio_component.h"
#include "../../engine/component/render_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"

#include <entt/entity/registry.hpp>

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
    // TODO: 如果图片不是面向右侧, 添加 FaceLeftTag
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

        // 使用填充好的动画帧容器创建动画, 并填充到动画 map 容器中
        engine::component::Animation animation{ std::move(frames) };
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
    // TODO: 添加远程或近战标签
}

} // namespace game::factory
