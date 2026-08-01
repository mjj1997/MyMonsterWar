#include "blueprint_manager.h"
#include "../data/entity_blueprint.h"

#include "../../engine/resource/resource_manager.h"

#include <entt/core/hashed_string.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

namespace game::factory {

BlueprintManager::BlueprintManager(engine::resource::ResourceManager& resourceManager)
    : m_resourceManager{ resourceManager }
{}

bool BlueprintManager::loadEnemyClassBlueprints(std::string_view enemyJsonPath)
{
    auto path = std::filesystem::path{ enemyJsonPath };
    std::ifstream file{ path };
    nlohmann::json json;
    file >> json;

    // --- 解析蓝图 ---
    try {
        for (const auto& [enemyClassName, enemyClassDataJson] : json.items()) {
            const entt::id_type classId{ entt::hashed_string(enemyClassName.c_str()) };
            // 解析 Stats
            auto stats = BlueprintManager::parseStats(enemyClassDataJson);
            // 解析 Enemy
            auto enemy = BlueprintManager::parseEnemy(enemyClassDataJson);
            // 解析 Sounds
            auto sounds = parseSounds(enemyClassDataJson);
            // 解析 Sprite
            auto sprite = BlueprintManager::parseSprite(enemyClassDataJson);
            // 解析 DisplayInfo
            auto displayInfo = BlueprintManager::parseDisplayInfo(enemyClassDataJson);
            // 解析 Animation
            auto animations = BlueprintManager::parseAnimationsMap(enemyClassDataJson);
            // 解析完毕，组合蓝图并插入容器
            m_enemyClassBlueprints.emplace(classId,
                                           data::EnemyClassBlueprint{
                                               .m_classId = classId,
                                               .m_className = enemyClassName,
                                               .m_stats = stats,
                                               .m_enemy = enemy,
                                               .m_sounds = std::move(sounds),
                                               .m_sprite = std::move(sprite),
                                               .m_displayInfo = std::move(displayInfo),
                                               .m_animations = std::move(animations) });
        }
    } catch (const std::exception& e) {
        spdlog::error("加载敌人类型蓝图数据时出错: {}", e.what());
        return false;
    }
    return true;
}

bool BlueprintManager::loadPlayerClassBlueprints(std::string_view playerJsonPath)
{
    auto path = std::filesystem::path{ playerJsonPath };
    std::ifstream file{ path };
    nlohmann::json json;
    file >> json;

    // --- 解析蓝图 ---
    try {
        for (const auto& [playerClassName, playerClassDataJson] : json.items()) {
            const entt::id_type classId{ entt::hashed_string(playerClassName.c_str()) };
            // 解析 Stats
            auto stats = BlueprintManager::parseStats(playerClassDataJson);
            // 解析 Player
            auto player = BlueprintManager::parsePlayer(playerClassDataJson);
            // 解析 Sounds
            auto sounds = parseSounds(playerClassDataJson);
            // 解析 Sprite
            auto sprite = BlueprintManager::parseSprite(playerClassDataJson);
            // 解析 DisplayInfo
            auto displayInfo = BlueprintManager::parseDisplayInfo(playerClassDataJson);
            // 解析 Animation
            auto animations = BlueprintManager::parseAnimationsMap(playerClassDataJson);
            // 解析完毕，组合蓝图并插入容器
            m_playerClassBlueprints.emplace(classId,
                                            data::PlayerClassBlueprint{
                                                .m_classId = classId,
                                                .m_className = playerClassName,
                                                .m_stats = stats,
                                                .m_player = player,
                                                .m_sounds = std::move(sounds),
                                                .m_sprite = std::move(sprite),
                                                .m_displayInfo = std::move(displayInfo),
                                                .m_animations = std::move(animations) });
        }
    } catch (const std::exception& e) {
        spdlog::error("加载玩家类型蓝图数据时出错: {}", e.what());
        return false;
    }
    return true;
}

bool BlueprintManager::loadProjectileBlueprints(std::string_view projectileJsonPath)
{
    auto path = std::filesystem::path{ projectileJsonPath };
    std::ifstream file{ path };
    nlohmann::json json;
    file >> json;

    // --- 解析蓝图 ---
    try {
        for (const auto& [projectileName, projectileDataJson] : json.items()) {
            const entt::id_type id{ entt::hashed_string(projectileName.c_str()) };
            float arcHeight{ projectileDataJson.at("arc_height").get<float>() };
            float totalFlightTime{ projectileDataJson.at("total_flight_time").get<float>() };
            // 解析 Sounds
            auto sounds = parseSounds(projectileDataJson);
            // 解析 Sprite
            auto sprite = BlueprintManager::parseSprite(projectileDataJson);
            // 解析完毕，组合蓝图并插入容器
            m_projectileBlueprints.emplace(id,
                                           data::ProjectileBlueprint{
                                               .m_id = id,
                                               .m_name = projectileName,
                                               .m_arcHeight = arcHeight,
                                               .m_totalFlightTime = totalFlightTime,
                                               .m_sounds = std::move(sounds),
                                               .m_sprite = std::move(sprite) });
        }
    } catch (const std::exception& e) {
        spdlog::error("加载玩家类型蓝图数据时出错: {}", e.what());
        return false;
    }
    return true;
}

const data::EnemyClassBlueprint& BlueprintManager::getEnemyClassBlueprint(entt::id_type id) const
{
    if (m_enemyClassBlueprints.contains(id)) {
        return m_enemyClassBlueprints.at(id);
    }

    spdlog::error("未找到对应 ID 的敌人类型蓝图: {}", id);
    return m_enemyClassBlueprints.begin()->second;
}

const data::PlayerClassBlueprint& BlueprintManager::getPlayerClassBlueprint(entt::id_type id) const
{
    if (m_playerClassBlueprints.contains(id)) {
        return m_playerClassBlueprints.at(id);
    }

    spdlog::error("未找到对应 ID 的玩家类型蓝图: {}", id);
    return m_playerClassBlueprints.begin()->second;
}

const data::ProjectileBlueprint& BlueprintManager::getProjectileBlueprint(entt::id_type id) const
{
    if (m_projectileBlueprints.contains(id)) {
        return m_projectileBlueprints.at(id);
    }

    spdlog::error("未找到对应 ID 的投射物蓝图: {}", id);
    return m_projectileBlueprints.begin()->second;
}

data::StatsBlueprint BlueprintManager::parseStats(const nlohmann::json& json)
{
    data::StatsBlueprint stats{ .m_hp = json.at("hp").get<float>(),
                                .m_atk = json.at("atk").get<float>(),
                                .m_def = json.at("def").get<float>(),
                                .m_range = json.at("range").get<float>(),
                                .m_atkInterval = json.at("atk_interval").get<float>() };
    return stats;
}

data::EnemyBlueprint BlueprintManager::parseEnemy(const nlohmann::json& json)
{
    data::EnemyBlueprint enemy{ .m_isRanged = json.at("is_ranged").get<bool>(),
                                .m_speed = json.at("speed").get<float>() };
    return enemy;
}

data::SoundsBlueprint BlueprintManager::parseSounds(const nlohmann::json& json)
{
    data::SoundsBlueprint sounds;
    if (json.contains("sounds")) {
        for (const auto& [soundKey, soundValue] : json.at("sounds").items()) {
            // 先把 soundValue 看成是音效路径并通过资源管理器加载
            auto soundPath = soundValue.get<std::string>();
            entt::id_type soundId{ entt::hashed_string(soundPath.c_str()) };
            m_resourceManager.loadSound(soundId, soundPath);
            // 将音效键值对转换为音效 ID 并插入到声音蓝图中
            sounds.m_sounds.emplace(entt::hashed_string(soundKey.c_str()), soundId);
        }
    }
    return sounds;
}

data::SpriteBlueprint BlueprintManager::parseSprite(const nlohmann::json& json)
{
    auto width = json.at("width").get<float>();
    auto height = json.at("height").get<float>();
    auto texturePath = json.at("sprite_sheet").get<std::string>();
    entt::id_type textureId{ entt::hashed_string(texturePath.c_str()) };

    /**
     * 可选部分: 源矩形的起点默认值为 (0, 0); 渲染目标大小默认值为 (width, height);
     * 如果指定, 则起点为 (x, y); 渲染目标大小为 (size_x, size_y);
     */
    data::SpriteBlueprint sprite{
        .m_textureId = textureId,
        .m_texturePath = texturePath,
        .m_sourceRect = engine::utils::Rect{ glm::vec2{ json.value("x", 0), json.value("y", 0) },
                                             glm::vec2{ width, height } },
        .m_size = glm::vec2{ json.value("size_x", width), json.value("size_y", height) },
        .m_offset = glm::vec2{ json.value("offset_x", 0), json.value("offset_y", 0) },
        .m_isFacedRight = json.value("is_faced_right", true)
    };
    return sprite;
}

data::DisplayInfoBlueprint BlueprintManager::parseDisplayInfo(const nlohmann::json& json)
{
    data::DisplayInfoBlueprint displayInfo{ .m_name = json.value("name", ""),
                                            .m_description = json.value("description", "") };
    return displayInfo;
}

std::unordered_map<entt::id_type, data::AnimationBlueprint> BlueprintManager::parseAnimationsMap(
    const nlohmann::json& json)
{
    std::unordered_map<entt::id_type, data::AnimationBlueprint> animations;
    for (const auto& [animeName, animeData] : json.at("animation").items()) {
        entt::id_type animeNameId{ entt::hashed_string(animeName.c_str()) };
        // 直接获取帧索引列表
        std::vector<int> frameIndices{ animeData.at("frames").get<std::vector<int>>() };
        // 处理可能存在的关键帧事件信息
        std::unordered_map<int, entt::id_type> events;
        if (animeData.contains("events")) {
            for (const auto& [eventName, keyframeIndex] : animeData.at("events").items()) {
                events.emplace(keyframeIndex.get<int>(), entt::hashed_string(eventName.c_str()));
            }
        }
        // 创建单个动画蓝图
        data::AnimationBlueprint animation{ .m_durationPerFrame = animeData.value("duration",
                                                                                  100.0F),
                                            .m_row = animeData.value("row", 0),
                                            .m_frameIndices = std::move(frameIndices),
                                            .m_events = std::move(events) };
        // 插入动画蓝图到映射中
        animations.emplace(animeNameId, animation);
    }
    return animations;
}

data::PlayerBlueprint BlueprintManager::parsePlayer(const nlohmann::json& json)
{
    // 解析玩家类型
    auto typeStr = json.at("type").get<std::string>();
    auto type = typeStr == "melee"    ? game::defs::PlayerType::Melee
                : typeStr == "ranged" ? game::defs::PlayerType::Ranged
                : typeStr == "mixed"  ? game::defs::PlayerType::Mixed
                                      : game::defs::PlayerType::Unknown;

    // 解析技能
    entt::id_type skillId{ entt::null };
    if (json.contains("skill")) {
        skillId = entt::hashed_string(json.at("skill").get<std::string>().c_str());
    }

    // 解析其它数据并返回
    data::PlayerBlueprint player{ .m_type = type,
                                  .m_skillId = skillId,
                                  .m_isHealer = json.at("is_healer").get<bool>(),
                                  .m_block = json.at("block").get<int>(),
                                  .m_cost = json.at("cost").get<int>() };

    return player;
}

} // namespace game::factory
