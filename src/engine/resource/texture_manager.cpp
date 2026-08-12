#include "texture_manager.h"

#include <SDL3_image/SDL_image.h> // 用于 IMG_LoadTexture， IMG_Init, IMG_Quit
#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

#include <stdexcept> // 用于 std::runtime_error

namespace engine::resource {

TextureManager::TextureManager(SDL_Renderer* sdlRenderer)
    : m_sdlRenderer{ sdlRenderer }
{
    if (sdlRenderer == nullptr) {
        // 关键错误，无法继续，抛出异常 （它将由catch语句捕获（位于GameApp），并进行处理）
        throw std::runtime_error{ "TextureManager 构造失败：渲染器指针为空。" };
    }
    // SDL3中不再需要手动调用IMG_Init/IMG_Quit
    spdlog::trace("TextureManager 构造成功。");
}

SDL_Texture* TextureManager::loadTexture(entt::id_type texturePathId, std::string_view texturePath)
{
    // 检查是否已加载该纹理
    auto iter = m_textures.find(texturePathId);
    if (iter != m_textures.end()) {
        return iter->second.get();
    }

    // 如果未加载，则尝试加载纹理
    SDL_Texture* texture = IMG_LoadTexture(m_sdlRenderer, texturePath.data());
    if (texture == nullptr) {
        spdlog::error("加载纹理失败：'{}': {}", texturePath, SDL_GetError());
        return nullptr;
    }

    // 设置纹理缩放模式为最近邻插值
    if (!SDL_SetTextureScaleMode(texture, SDL_SCALEMODE_NEAREST)) {
        spdlog::warn("无法设置纹理缩放模式为最近邻插值");
    }

    // 使用带有自定义删除器的 unique_ptr 存储加载的纹理
    m_textures.emplace(texturePathId, std::unique_ptr<SDL_Texture, SDLTextureDeletor>{ texture });
    spdlog::debug("成功加载并缓存纹理：{}", texturePath);

    return texture;
}

SDL_Texture* TextureManager::loadTexture(entt::hashed_string hashedTexturePath)
{
    return loadTexture(hashedTexturePath.value(), hashedTexturePath.data());
}

SDL_Texture* TextureManager::getTexture(entt::id_type texturePathId, std::string_view texturePath)
{
    // 查找现有纹理
    auto iter = m_textures.find(texturePathId);
    if (iter != m_textures.end()) {
        return iter->second.get();
    }

    // 如果未找到纹理，尝试加载纹理
    if (texturePath.empty()) {
        spdlog::error("纹理 '{}' 未找到缓存，且未提供文件路径，返回 nullptr。", texturePathId);
        return nullptr;
    }

    spdlog::info("纹理 '{}' 未找到缓存，尝试加载。", texturePath);
    return loadTexture(texturePathId, texturePath);
}

SDL_Texture* TextureManager::getTexture(entt::hashed_string hashedTexturePath)
{
    return getTexture(hashedTexturePath.value(), hashedTexturePath.data());
}

glm::vec2 TextureManager::getTextureSize(entt::id_type texturePathId, std::string_view texturePath)
{
    // 获取纹理
    SDL_Texture* texture{ getTexture(texturePathId, texturePath) };
    if (texture == nullptr) {
        spdlog::error("无法获取纹理：{}", texturePath);
        return glm::vec2(0.0F);
    }

    // 获取纹理尺寸
    glm::vec2 size;
    if (!SDL_GetTextureSize(texture, &size.x, &size.y)) {
        spdlog::error("无法查询纹理尺寸：{}", texturePath);
        return glm::vec2(0.0F);
    }
    return size;
}

glm::vec2 TextureManager::getTextureSize(entt::hashed_string hashedTexturePath)
{
    return getTextureSize(hashedTexturePath.value(), hashedTexturePath.data());
}

void TextureManager::unloadTexture(entt::id_type texturePathId)
{
    auto iter = m_textures.find(texturePathId);
    if (iter != m_textures.end()) {
        m_textures.erase(iter); // unique_ptr 通过自定义删除器自动释放纹理
        spdlog::debug("成功卸载纹理：texturePathId = {}", texturePathId);
    } else {
        spdlog::warn("尝试卸载不存在的纹理：texturePathId = {}", texturePathId);
    }
}

void TextureManager::clearTextures()
{
    if (!m_textures.empty()) {
        spdlog::debug("正在清楚所有 {} 个缓存的纹理。", m_textures.size());
        m_textures.clear();
    }
}

} // namespace engine::resource
