#pragma once

#include <entt/entity/fwd.hpp>
#include <glm/vec2.hpp>

#include <unordered_map>

namespace game::data {
struct SpriteBlueprint;
struct AnimationBlueprint;
struct SoundsBlueprint;
struct StatsBlueprint;
struct EnemyBlueprint;
struct PlayerBlueprint;
} // namespace game::data

namespace game::factory {

class BlueprintManager;

/**
 * @brief 实体工厂，用于创建不同类型的实体
 * 
 * 实体工厂通过蓝图管理器获取蓝图数据，并创建不同类型的实体。
 */
class EntityFactory
{
public:
    /// @brief 实体工厂构造函数, 需要传入注册表和蓝图管理器。通过蓝图数据创建不同实体
    EntityFactory(entt::registry& registry, BlueprintManager& blueprintManager);

    entt::entity createEnemyUnit(entt::id_type classId,
                                 glm::vec2 position,
                                 int targetPathNodeId,
                                 int level = 1,
                                 int rarity = 1);
    entt::entity createPlayerUnit(entt::id_type classId,
                                  glm::vec2 position,
                                  int level = 1,
                                  int rarity = 1);
    // TODO: 未来添加其他实体的创建函数

private:
    // --- 组件创建函数 ---
    void addTransformComponent(entt::entity entity,
                               glm::vec2 position,
                               glm::vec2 scale = glm::vec2(1.0F),
                               float rotation = 0.0F);
    void addSpriteComponent(entt::entity entity,
                            const data::SpriteBlueprint& sprite,
                            bool isFlipped = false);
    void addAnimationComponent(
        entt::entity entity,
        const std::unordered_map<entt::id_type, data::AnimationBlueprint>& animationBlueprints,
        const data::SpriteBlueprint& spriteBlueprint,
        entt::id_type defaultAnimationId);
    void addAudioComponent(entt::entity entity, const data::SoundsBlueprint& sounds);
    void addStatsComponent(entt::entity entity,
                           const data::StatsBlueprint& stats,
                           int level = 1,
                           int rarity = 1);
    void addEnemyComponent(entt::entity entity,
                           const data::EnemyBlueprint& enemy,
                           int targetPathNodeId);
    void addPlayerComponent(entt::entity entity, const data::PlayerBlueprint& player, int rarity);
    // TODO: 未来添加其他组件创建函数

    entt::registry& m_registry;
    BlueprintManager& m_blueprintManager;
};

} // namespace game::factory
