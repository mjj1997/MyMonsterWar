#include "font_manager.h"

#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

#include <stdexcept>

namespace engine::resource {

FontManager::FontManager()
{
    if (TTF_WasInit() == 0 && !TTF_Init()) {
        throw std::runtime_error{ "FontManager 错误：TTF_Init 失败："
                                  + std::string(SDL_GetError()) };
    }
    spdlog::trace("FontManager 构造成功。");
}

FontManager::~FontManager()
{
    if (!m_fonts.empty()) {
        spdlog::debug("FontManager 不为空，调用 clearFonts() 处理清理逻辑。");
        clearFonts();
    }

    TTF_Quit();
    spdlog::trace("FontManager 析构成功。");
}

TTF_Font* FontManager::loadFont(entt::id_type id, int pointSize, std::string_view filePath)
{
    // 检查点大小是否有效
    if (pointSize <= 0) {
        spdlog::error("无法加载字体 '{}': 无效的点大小 {}。", filePath, pointSize);
        return nullptr;
    }

    // 创建映射表的键
    FontKey key{ id, pointSize };

    // 首先检查缓存
    auto iter = m_fonts.find(key);
    if (iter != m_fonts.end()) {
        return iter->second.get();
    }

    // 缓存中不存在，则加载字体
    spdlog::debug("正在加载字体: {} ({}pt)", filePath, pointSize);
    TTF_Font* font{ TTF_OpenFont(filePath.data(), pointSize) };
    if (font == nullptr) {
        spdlog::error("加载字体 '{}' ({}pt) 失败: {}", filePath, pointSize, SDL_GetError());
        return nullptr;
    }

    // 使用 unique_ptr 存储到缓存中
    m_fonts.emplace(key, std::unique_ptr<TTF_Font, SDLFontDeletor>{ font });
    spdlog::debug("成功加载并缓存字体: {} ({}pt)", filePath, pointSize);
    return font;
}

TTF_Font* FontManager::loadFont(entt::hashed_string hs, int pointSize)
{
    return loadFont(hs.value(), pointSize, hs.data());
}

TTF_Font* FontManager::getFont(entt::id_type id, int pointSize, std::string_view filePath)
{
    // 查找现有字体
    const FontKey key{ id, pointSize };
    auto iter = m_fonts.find(key);
    if (iter != m_fonts.end()) {
        return iter->second.get();
    }

    // 如果未找到字体，尝试加载字体
    if (filePath.empty()) {
        spdlog::error("字体 '{}' 未找到缓存，且未提供文件路径，返回 nullptr。", id);
        return nullptr;
    }
    spdlog::info("字体 '{}' ({}pt) 不在缓存中，尝试加载。", filePath, pointSize);
    return loadFont(id, pointSize, filePath);
}

TTF_Font* FontManager::getFont(entt::hashed_string hs, int pointSize)
{
    return getFont(hs.value(), pointSize, hs.data());
}

void FontManager::unloadFont(entt::id_type id, int pointSize)
{
    const FontKey key{ id, pointSize };
    auto iter = m_fonts.find(key);
    if (iter != m_fonts.end()) {
        spdlog::debug("成功卸载字体: id = {} ({}pt)", id, pointSize);
        m_fonts.erase(iter); // unique_ptr 处理 TTF_CloseFont
    } else {
        spdlog::warn("尝试卸载不存在的字体: id = {} ({}pt)", id, pointSize);
    }
}

void FontManager::clearFonts()
{
    if (!m_fonts.empty()) {
        spdlog::debug("正在清除所有 {} 个缓存的字体。", m_fonts.size());
        m_fonts.clear(); // unique_ptr 处理删除
    }
}

} // namespace engine::resource
