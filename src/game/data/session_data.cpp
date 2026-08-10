#include "session_data.h"

#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

namespace game::data {

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
