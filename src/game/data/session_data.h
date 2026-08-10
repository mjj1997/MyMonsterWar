#pragma once

namespace game::data {

/**
 * @brief 场景间（例如通关时）传递的跨关卡数据
 * 
 * 包含玩家角色列表、积分、是否通关等跨关卡数据。
 */
class SessionData
{
public:
    // --- getters & setters ---

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
    int m_level{ 1 };             ///< @brief 当前关卡号
    int m_score{ 0 };             ///< @brief 积分
    bool m_isLevelClear{ false }; ///< @brief 是否通关
};

} // namespace game::data
