#include "ui_config.h"

#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <filesystem>
#include <fstream>

namespace game::data {

bool UiConfig::loadFromFile(std::string_view path)
{
    std::filesystem::path filePath{ path };
    std::ifstream file{ filePath };
    if (!file.is_open()) {
        spdlog::error("无法打开 UI 配置文件: {}", path);
        return false;
    }

    nlohmann::json json;
    file >> json;

    try {
        // TODO： 加载图标
        // TODO： 加载肖像
        // TODO： 加载肖像框
        // TODO： 加载布局
    } catch (const std::exception& e) {
        spdlog::error("载入 UI 配置文件失败: {}", e.what());
        return false;
    }

    return true;
}

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
