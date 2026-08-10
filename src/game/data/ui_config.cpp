#include "ui_config.h"

#include <spdlog/spdlog.h>

namespace game::data {

engine::render::Image& UiConfig::icon(entt::id_type id)
{
    if (auto it = m_icons.find(id); it != m_icons.end()) {
        return it->second;
    } else {
        spdlog::error("未找到职业类型 ID: {} 的图标", id);
        return m_icons.begin()->second;
    }
}

engine::render::Image& UiConfig::portrait(entt::id_type id)
{
    if (auto it = m_portraits.find(id); it != m_portraits.end()) {
        return it->second;
    } else {
        spdlog::error("未找到角色名 ID: {} 的肖像", id);
        return m_portraits.begin()->second;
    }
}

engine::render::Image& UiConfig::portraitFrame(int rarity)
{
    if (auto it = m_portraitFrames.find(rarity); it != m_portraitFrames.end()) {
        return it->second;
    } else {
        spdlog::error("未找到稀有度: {} 的肖像框", rarity);
        return m_portraitFrames.begin()->second;
    }
}

} // namespace game::data
