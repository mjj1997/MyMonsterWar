#include "ui_config.h"

#include <entt/core/hashed_string.hpp>
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
        // 加载图标
        loadIcon(json.at("icon"));
        // 加载肖像
        loadPortrait(json.at("portrait"));
        // 加载肖像框
        loadPortraitFrame(json.at("portrait_frame"));
        // 加载布局
        loadLayout(json.at("layout"));
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

void UiConfig::loadIcon(const nlohmann::json& json)
{
    for (const auto& [key, value] : json.items()) {
        entt::id_type id{ entt::hashed_string(key.c_str()) };

        std::string texturePath{ value.at("sprite_sheet").get<std::string>() };
        engine::utils::Rect srcRect{ value.at("x").get<float>(),
                                     value.at("y").get<float>(),
                                     value.at("width").get<float>(),
                                     value.at("height").get<float>() };
        engine::render::Image icon{ texturePath, srcRect, false };

        m_icons.emplace(id, icon);
    }
}

void UiConfig::loadPortrait(const nlohmann::json& json)
{
    for (const auto& [key, value] : json.items()) {
        entt::id_type id{ entt::hashed_string(key.c_str()) };

        std::string texturePath{ value.at("sprite_sheet").get<std::string>() };
        engine::utils::Rect srcRect{ value.at("x").get<float>(),
                                     value.at("y").get<float>(),
                                     value.at("width").get<float>(),
                                     value.at("height").get<float>() };
        engine::render::Image portrait{ texturePath, srcRect, false };

        m_portraits.emplace(id, portrait);
    }
}

void UiConfig::loadPortraitFrame(const nlohmann::json& json)
{
    for (const auto& [key, value] : json.items()) {
        int rank{ value.at("rank").get<int>() };

        std::string texturePath{ value.at("sprite_sheet").get<std::string>() };
        engine::utils::Rect srcRect{ value.at("x").get<float>(),
                                     value.at("y").get<float>(),
                                     value.at("width").get<float>(),
                                     value.at("height").get<float>() };
        engine::render::Image portraitFrame{ texturePath, srcRect, false };

        m_portraitFrames.emplace(rank, portraitFrame);
    }
}

void UiConfig::loadLayout(const nlohmann::json& json)
{
    auto playerUnitPanelData = json.at("player_unit_panel");

    m_playerUnitPanelPadding = playerUnitPanelData.at("padding").get<float>();
    m_playerUnitPanelFrameSize = glm::vec2{
        playerUnitPanelData.at("frame_size").at("width").get<float>(),
        playerUnitPanelData.at("frame_size").at("height").get<float>()
    };
    m_playerUnitPanelFontSize = playerUnitPanelData.at("font_size").get<int>();
    m_playerUnitPanelFontPath = playerUnitPanelData.at("font_path").get<std::string>();
    m_playerUnitPanelFontOffset = glm::vec2{
        playerUnitPanelData.at("font_offset").at("x").get<float>(),
        playerUnitPanelData.at("font_offset").at("y").get<float>()
    };
}

} // namespace game::data
