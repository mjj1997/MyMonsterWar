#include "session_data.h"

#include <entt/core/hashed_string.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

namespace game::data {

bool SessionData::loadDefaultData(std::string_view path)
{
    if (!std::filesystem::exists(path)) {
        spdlog::error("Session data 文件未找到: {}", path);
        return false;
    }

    // 先清空所有数据
    clear();

    std::filesystem::path filePath{ path };
    std::ifstream file{ filePath };
    if (!file.is_open()) {
        spdlog::error("无法打开 Session data 文件: {}", path);
        return false;
    }

    nlohmann::json json;
    file >> json;

    try {
        // 关卡基本信息：当前关卡、积分、是否通关
        m_level = json.at("level").get<int>();
        m_score = json.at("score").get<int>();
        m_isLevelClear = json.at("is_level_clear").get<bool>();
        // 玩家角色数据：玩家角色名Id、职业Id、角色名、职业名、等级、稀有度
        for (const auto& [name, data] : json.at("player_unit").items()) {
            entt::id_type nameId{ entt::hashed_string(name.c_str()) };
            std::string className{ data.at("class").get<std::string>() };
            entt::id_type classId{ entt::hashed_string(className.c_str()) };
            int lv{ data.at("lv").get<int>() };
            int rarity{ data.at("rarity").get<int>() };
            m_playerUnits.emplace(nameId,
                                  PlayerUnitData{ .m_nameId = nameId,
                                                  .m_classId = classId,
                                                  .m_name = name,
                                                  .m_className = className,
                                                  .m_lv = lv,
                                                  .m_rarity = rarity });
        }
    } catch (const std::exception& e) {
        spdlog::error("加载 Session data 失败: {}", e.what());
        return false;
    }

    return true;
}

void SessionData::clear()
{
    m_playerUnits.clear();
    m_level = 1;
    m_score = 0;
    m_isLevelClear = false;
}

void SessionData::addPlayerUnit(std::string_view name,
                                std::string_view className,
                                int lv,
                                int rarity)
{
    entt::id_type nameId{ entt::hashed_string(name.data()) };
    entt::id_type classId{ entt::hashed_string(className.data()) };
    // 创建玩家角色数据，并插入到玩家角色映射中
    m_playerUnits.emplace(nameId,
                          PlayerUnitData{ .m_nameId = nameId,
                                          .m_classId = classId,
                                          .m_name = std::string(name),
                                          .m_className = std::string{ className },
                                          .m_lv = lv,
                                          .m_rarity = rarity });
}

void SessionData::removePlayerUnit(entt::id_type nameId)
{
    if (auto it = m_playerUnits.find(nameId); it != m_playerUnits.end()) {
        m_playerUnits.erase(it);
    } else {
        spdlog::error("未找到该玩家角色名 ID: {}", nameId);
    }
}

void SessionData::increasePlayerUnitLv(entt::id_type nameId, int lv)
{
    if (auto it = m_playerUnits.find(nameId); it != m_playerUnits.end()) {
        it->second.m_lv += lv;
    } else {
        spdlog::error("未找到该玩家角色名 ID: {}", nameId);
    }
}

void SessionData::increasePlayerUnitRarity(entt::id_type nameId, int rarity)
{
    if (auto it = m_playerUnits.find(nameId); it != m_playerUnits.end()) {
        it->second.m_rarity += rarity;
    } else {
        spdlog::error("未找到该玩家角色名 ID: {}", nameId);
    }
}

void SessionData::clearPlayerUnits()
{
    m_playerUnits.clear();
}

} // namespace game::data
