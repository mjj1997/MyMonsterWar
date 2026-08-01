#pragma once

#include <entt/entity/fwd.hpp>
#include <nlohmann/json_fwd.hpp>

#include <string_view>
#include <unordered_map>

namespace engine::resource {
class ResourceManager;
}

namespace game::data {
struct EnemyClassBlueprint;
struct StatsBlueprint;
struct EnemyBlueprint;
struct SoundsBlueprint;
struct SpriteBlueprint;
struct DisplayInfoBlueprint;
struct AnimationBlueprint;
struct PlayerClassBlueprint;
struct PlayerBlueprint;
} // namespace game::data

namespace game::factory {

/**
 * @brief 蓝图管理器，用于存储、管理所有蓝图
 * 
 * 它具有从json数据中加载蓝图并保存到容器，和获取指定ID的蓝图的功能。蓝图信息将由实体工厂使用。
 */
class BlueprintManager
{
public:
    BlueprintManager(engine::resource::ResourceManager& resourceManager);

    ///< @brief 加载敌人类型蓝图, 返回是否成功
    [[nodiscard]] bool loadEnemyClassBlueprints(std::string_view enemyJsonPath);
    ///< @brief 加载玩家类型蓝图, 返回是否成功
    [[nodiscard]] bool loadPlayerClassBlueprints(std::string_view playerJsonPath);
    ///< @brief 加载投射物蓝图, 返回是否成功
    [[nodiscard]] bool loadProjectileBlueprints(std::string_view projectileJsonPath);
    // TODO: 未来添加其他蓝图加载函数

    ///< @brief 获取指定ID的敌人类型蓝图
    const data::EnemyClassBlueprint& getEnemyClassBlueprint(entt::id_type id) const;
    ///< @brief 获取指定ID的玩家类型蓝图
    const data::PlayerClassBlueprint& getPlayerClassBlueprint(entt::id_type id) const;
    ///< @brief 获取指定ID的投射物蓝图
    const data::ProjectileBlueprint& getProjectileBlueprint(entt::id_type id) const;
    // TODO: 未来添加其他蓝图获取函数

private:
    // --- 分别针对各个子蓝图进行json解析，并创建(返回)对应的蓝图结构体 ---
    static data::StatsBlueprint parseStats(const nlohmann::json& json);
    static data::EnemyBlueprint parseEnemy(const nlohmann::json& json);
    data::SoundsBlueprint parseSounds(const nlohmann::json& json);
    static data::SpriteBlueprint parseSprite(const nlohmann::json& json);
    static data::DisplayInfoBlueprint parseDisplayInfo(const nlohmann::json& json);
    static std::unordered_map<entt::id_type, data::AnimationBlueprint> parseAnimationsMap(
        const nlohmann::json& json);
    static data::PlayerBlueprint parsePlayer(const nlohmann::json& json);
    static entt::id_type parseProjectileId(const nlohmann::json& json);

    engine::resource::ResourceManager& m_resourceManager;
    ///< @brief 敌人类型蓝图
    std::unordered_map<entt::id_type, data::EnemyClassBlueprint> m_enemyClassBlueprints;
    ///< @brief 玩家类型蓝图
    std::unordered_map<entt::id_type, data::PlayerClassBlueprint> m_playerClassBlueprints;
    ///< @brief 投射物蓝图
    std::unordered_map<entt::id_type, data::ProjectileBlueprint> m_projectileBlueprints;
    // TODO: 未来添加其他蓝图容器
};

} // namespace game::factory
