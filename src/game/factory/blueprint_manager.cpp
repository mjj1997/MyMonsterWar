#include "blueprint_manager.h"
#include "../data/entity_blueprint.h"

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
            // TODO: 解析 Sprite
            // TODO: 解析 Animation
            // TODO: 解析 Sounds
            // TODO: 解析 Enemy
            // TODO: 解析 DisplayInfo
            // TODO: 解析完毕，组合蓝图并插入容器
            m_enemyClassBlueprints.emplace(classId,
                                           data::EnemyClassBlueprint{ .m_classId = classId,
                                                                      .m_className = enemyClassName,
                                                                      .m_stats = stats });
        }
    } catch (const std::exception& e) {
        spdlog::error("加载敌人类型蓝图数据时出错: {}", e.what());
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

data::StatsBlueprint BlueprintManager::parseStats(const nlohmann::json& json)
{
    data::StatsBlueprint stats{ .m_hp = json.at("hp").get<float>(),
                                .m_atk = json.at("atk").get<float>(),
                                .m_def = json.at("def").get<float>(),
                                .m_range = json.at("range").get<float>(),
                                .m_atkInterval = json.at("atk_interval").get<float>() };
    return stats;
}

} // namespace game::factory
