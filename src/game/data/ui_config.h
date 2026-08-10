#pragma once

#include "../../engine/render/image.h"

#include <entt/entity/fwd.hpp>
#include <glm/vec2.hpp>

#include <unordered_map>

namespace game::data {

/**
 * @brief 管理 UI 配置数据。
 * 
 * 包含职业类型图标、角色肖像、角色肖像框、玩家单位面板的配置数据。
 */
class UiConfig
{
public:
    UiConfig() = default;
    ~UiConfig() = default;

    // --- Getters ---
    [[nodiscard]] engine::render::Image& icon(entt::id_type id);
    [[nodiscard]] engine::render::Image& portrait(entt::id_type id);
    [[nodiscard]] engine::render::Image& portraitFrame(int rarity);
    [[nodiscard]] float playerUnitPanelPadding() const { return m_playerUnitPanelPadding; }
    [[nodiscard]] glm::vec2 playerUnitPanelFrameSize() const { return m_playerUnitPanelFrameSize; }
    [[nodiscard]] int playerUnitPanelFontSize() const { return m_playerUnitPanelFontSize; }
    [[nodiscard]] std::string playerUnitPanelFontPath() const { return m_playerUnitPanelFontPath; }
    [[nodiscard]] glm::vec2 playerUnitPanelFontOffset() const
    {
        return m_playerUnitPanelFontOffset;
    }

private:
    /// @brief 储存职业类型的图标（职业类型ID:图标）
    std::unordered_map<entt::id_type, engine::render::Image> m_icons;
    /// @brief 储存角色肖像（角色名ID:肖像）
    std::unordered_map<entt::id_type, engine::render::Image> m_portraits;
    /// @brief 储存角色肖像框（稀有度:肖像框）
    std::unordered_map<int, engine::render::Image> m_portraitFrames;

    // --- 玩家单位面板的配置数据（从 JSON 配置文件中的 layout 项读取） ---
    float m_playerUnitPanelPadding{ 10.0F };                ///< @brief 玩家单位面板间隔
    glm::vec2 m_playerUnitPanelFrameSize{ 128.0F, 128.0F }; ///< @brief 玩家单位面板大小
    int m_playerUnitPanelFontSize{ 40 };                    ///< @brief 玩家单位面板字体大小
    std::string m_playerUnitPanelFontPath;                  ///< @brief 玩家单位面板字体路径
    glm::vec2 m_playerUnitPanelFontOffset{ 16.0F, 72.0F };  ///< @brief 玩家单位面板字体偏移
};

} // namespace game::data
