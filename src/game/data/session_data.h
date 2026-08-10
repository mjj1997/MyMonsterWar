#pragma once

#include <entt/entity/entity.hpp>

#include <string>
#include <string_view>
#include <unordered_map>

namespace game::data {

/**
 * @brief 玩家角色数据
 * 
 * 包含玩家角色名称、职业、等级、稀有度。
 */
struct PlayerUnitData
{
    entt::id_type m_nameId{ entt::null };
    entt::id_type m_classId{ entt::null };
    std::string m_name;
    std::string m_className;
    int m_lv{ 1 };
    int m_rarity{ 1 };
};

/**
 * @brief 场景间（例如通关时）传递的跨关卡数据
 * 
 * 包含玩家角色列表、积分、是否通关等跨关卡数据。
 */
class SessionData
{
public:
    SessionData() = default;
    ~SessionData() = default;

    ///< @brief 加载默认数据
    bool loadDefaultData(std::string_view path = "assets/data/default_session_data.json");
    ///< @brief 清空所有数据
    void clear();

    // --- getters & setters ---

    [[nodiscard]] std::unordered_map<entt::id_type, PlayerUnitData>& playerUnits()
    {
        return m_playerUnits;
    }
    ///< @brief 添加玩家角色
    void addPlayerUnit(std::string_view name, std::string_view className, int lv, int rarity);
    ///< @brief 删除玩家角色
    void removePlayerUnit(entt::id_type nameId);
    ///< @brief 增加玩家角色等级
    void increasePlayerUnitLv(entt::id_type nameId, int lv = 1);
    ///< @brief 增加玩家角色稀有度
    void increasePlayerUnitRarity(entt::id_type nameId, int rarity = 1);
    ///< @brief 清空玩家角色列表
    void clearPlayerUnits();

    [[nodiscard]] int level() const { return m_level; }
    ///< @brief 增加关卡号(进入下一关)
    int increaseLevel() { return ++m_level; }

    [[nodiscard]] int score() const { return m_score; }
    ///< @brief 增加积分
    void increaseScore(int score) { m_score += score; }

    [[nodiscard]] bool isLevelClear() const { return m_isLevelClear; }
    ///< @brief 设置是否通关
    void setLevelClear(bool clear) { m_isLevelClear = clear; }

private:
    /// @brief 储存玩家拥有的角色 (角色名ID:玩家角色数据)
    std::unordered_map<entt::id_type, PlayerUnitData> m_playerUnits;

    int m_level{ 1 };             ///< @brief 当前关卡号
    int m_score{ 0 };             ///< @brief 积分
    bool m_isLevelClear{ false }; ///< @brief 是否通关
};

} // namespace game::data
