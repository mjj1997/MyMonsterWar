#include "resource_manager.h"
#include "audio_manager.h"
#include "font_manager.h"
#include "texture_manager.h"

#include <entt/core/hashed_string.hpp>
#include <nlohmann/json.hpp>
#include <spdlog/spdlog.h>

#include <fstream>

namespace engine::resource {

ResourceManager::ResourceManager(SDL_Renderer* sdlRenderer)
    // --- 初始化各个子系统 --- (如果出现错误会抛出异常，由上层捕获)
    : m_textureManager{ std::make_unique<TextureManager>(sdlRenderer) }
    , m_audioManager{ std::make_unique<AudioManager>() }
    , m_fontManager{ std::make_unique<FontManager>() }
{
    spdlog::trace("ResourceManager 构造成功。");
}

ResourceManager::~ResourceManager() = default;

void ResourceManager::clear()
{
    m_textureManager->clearTextures();
    m_audioManager->clearAudio();
    m_fontManager->clearFonts();
    spdlog::trace("ResourceManager 中的资源通过 clear() 清空。");
}

void ResourceManager::loadResources(std::string_view filePath)
{
    std::filesystem::path path{ filePath };
    if (!std::filesystem::exists(path)) {
        spdlog::warn("资源映射文件不存在: {}", filePath);
        return;
    }

    std::ifstream file{ path };
    nlohmann::json json;
    file >> json;
    try {
        if (json.contains("texture")) {
            for (const auto& [key, value] : json.at("texture").items()) {
                loadTexture(entt::hashed_string(key.c_str()), value.get<std::string>());
            }
        }
        if (json.contains("sound")) {
            for (const auto& [key, value] : json.at("sound").items()) {
                loadSound(entt::hashed_string(key.c_str()), value.get<std::string>());
            }
        }
        if (json.contains("music")) {
            for (const auto& [key, value] : json.at("music").items()) {
                loadMusic(entt::hashed_string(key.c_str()), value.get<std::string>());
            }
        }
        if (json.contains("font")) {
            for (const auto& [key, value] : json.at("font").items()) {
                loadFont(entt::hashed_string(key.c_str()),
                         value.get<int>(),
                         value.get<std::string>());
            }
        }
    } catch (const nlohmann::json::exception& e) {
        spdlog::error("加载资源文件失败: {}", e.what());
    }
}

// --- 纹理接口实现 ---
SDL_Texture* ResourceManager::loadTexture(entt::id_type id, std::string_view filePath)
{
    return m_textureManager->loadTexture(id, filePath);
}

SDL_Texture* ResourceManager::loadTexture(entt::hashed_string hs)
{
    return m_textureManager->loadTexture(hs);
}

SDL_Texture* ResourceManager::getTexture(entt::id_type id, std::string_view filePath)
{
    return m_textureManager->getTexture(id, filePath);
}

SDL_Texture* ResourceManager::getTexture(entt::hashed_string hs)
{
    return m_textureManager->getTexture(hs);
}

void ResourceManager::unloadTexture(entt::id_type id)
{
    m_textureManager->unloadTexture(id);
}

glm::vec2 ResourceManager::getTextureSize(entt::id_type id, std::string_view filePath)
{
    return m_textureManager->getTextureSize(id, filePath);
}

glm::vec2 ResourceManager::getTextureSize(entt::hashed_string hs)
{
    return m_textureManager->getTextureSize(hs);
}

void ResourceManager::clearTextures()
{
    m_textureManager->clearTextures();
}

// --- 音频接口实现 ---
MIX_Audio* ResourceManager::loadSound(entt::id_type id, std::string_view filePath)
{
    return m_audioManager->loadSound(id, filePath);
}

MIX_Audio* ResourceManager::loadSound(entt::hashed_string hs)
{
    return m_audioManager->loadSound(hs);
}

MIX_Audio* ResourceManager::getSound(entt::id_type id, std::string_view filePath)
{
    return m_audioManager->getSound(id, filePath);
}

MIX_Audio* ResourceManager::getSound(entt::hashed_string hs)
{
    return m_audioManager->getSound(hs);
}

void ResourceManager::unloadSound(entt::id_type id)
{
    m_audioManager->unloadSound(id);
}

void ResourceManager::clearSounds()
{
    m_audioManager->clearSounds();
}

MIX_Audio* ResourceManager::loadMusic(entt::id_type id, std::string_view filePath)
{
    return m_audioManager->loadMusic(id, filePath);
}

MIX_Audio* ResourceManager::loadMusic(entt::hashed_string hs)
{
    return m_audioManager->loadMusic(hs);
}

MIX_Audio* ResourceManager::getMusic(entt::id_type id, std::string_view filePath)
{
    return m_audioManager->getMusic(id, filePath);
}

MIX_Audio* ResourceManager::getMusic(entt::hashed_string hs)
{
    return m_audioManager->getMusic(hs);
}

void ResourceManager::unloadMusic(entt::id_type id)
{
    m_audioManager->unloadMusic(id);
}

void ResourceManager::clearMusics()
{
    m_audioManager->clearMusics();
}

MIX_Mixer* ResourceManager::mixer() const
{
    return m_audioManager->mixer();
}

// --- 字体接口实现 ---
TTF_Font* ResourceManager::loadFont(entt::id_type id, int pointSize, std::string_view filePath)
{
    return m_fontManager->loadFont(id, pointSize, filePath);
}

TTF_Font* ResourceManager::loadFont(entt::hashed_string hs, int pointSize)
{
    return m_fontManager->loadFont(hs, pointSize);
}

TTF_Font* ResourceManager::getFont(entt::id_type id, int pointSize, std::string_view filePath)
{
    return m_fontManager->getFont(id, pointSize, filePath);
}

TTF_Font* ResourceManager::getFont(entt::hashed_string hs, int pointSize)
{
    return m_fontManager->getFont(hs, pointSize);
}

void ResourceManager::unloadFont(entt::id_type id, int pointSize)
{
    m_fontManager->unloadFont(id, pointSize);
}

void ResourceManager::clearFonts()
{
    m_fontManager->clearFonts();
}

} // namespace engine::resource
