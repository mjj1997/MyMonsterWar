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

    /**
     * @brief 创建敌人单位
     * @param classId 敌人类型 ID
     * @param position 位置
     * @param targetPathNodeId 目标路径点 ID
     * @param level 等级
     * @param rarity 稀有度
     * @return 敌人单位实体
     */
    entt::entity createEnemyUnit(entt::id_type classId,
                                 glm::vec2 position,
                                 int targetPathNodeId,
                                 int level = 1,
                                 int rarity = 1);

    /**
     * @brief 创建玩家单位
     * @param classId 玩家类型 ID
     * @param position 位置
     * @param level 等级
     * @param rarity 稀有度
     * @return 玩家单位实体
     */
    entt::entity createPlayerUnit(entt::id_type classId,
                                  glm::vec2 position,
                                  int level = 1,
                                  int rarity = 1);

    /**
     * @brief 创建投射物
     * @param id 投射物 ID
     * @param startPosition 起始位置
     * @param targetPosition 目标位置
     * @param target 目标实体
     * @param damage 伤害
     * @return 投射物实体
     */
    entt::entity createProjectile(entt::id_type id,
                                  glm::vec2 startPosition,
                                  glm::vec2 targetPosition,
                                  entt::entity target,
                                  float damage);

    /**
     * @brief 创建敌人死亡特效
     * @note 敌人死亡特效直接从敌人蓝图中获取，对应的动画名称必须为“damage”。
     * @param classId 敌人类型 ID
     * @param position 位置
     * @param isFlipped 是否翻转
     * @return 敌人死亡特效实体
     */
    entt::entity createEnemyDeadEffect(entt::id_type classId,
                                       glm::vec2 position,
                                       const bool isFlipped = false);

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

    ///< @brief 添加动画组件（包含多个动画）
    void addAnimationComponent(
        entt::entity entity,
        const std::unordered_map<entt::id_type, data::AnimationBlueprint>& animationBlueprints,
        const data::SpriteBlueprint& spriteBlueprint,
        entt::id_type defaultAnimationId);
    ///< @brief 添加动画组件（只包含一个动画），用于创建特效
    void addAnimationComponent(entt::entity entity,
                               const data::AnimationBlueprint& animationBlueprint,
                               const data::SpriteBlueprint& spriteBlueprint,
                               entt::id_type animationId,
                               bool isLoop = false);

    void addAudioComponent(entt::entity entity, const data::SoundsBlueprint& sounds);
    void addStatsComponent(entt::entity entity,
                           const data::StatsBlueprint& stats,
                           int level = 1,
                           int rarity = 1);
    void addEnemyComponent(entt::entity entity,
                           const data::EnemyBlueprint& enemy,
                           int targetPathNodeId);
    void addPlayerComponent(entt::entity entity, const data::PlayerBlueprint& player, int rarity);
    void addProjectileIdComponent(entt::entity entity, entt::id_type id);
    // TODO: 未来添加其他组件创建函数

    entt::registry& m_registry;
    BlueprintManager& m_blueprintManager;
};

} // namespace game::factory
