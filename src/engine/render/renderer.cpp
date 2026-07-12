#include "renderer.h"
#include "../resource/resource_manager.h"
#include "camera.h"
#include "image.h"

#include <SDL3/SDL.h>
#include <spdlog/spdlog.h>

#include <stdexcept>

namespace engine::render {

Renderer::Renderer(SDL_Renderer* sdlRenderer, engine::resource::ResourceManager* resourceManager)
    : m_sdlRenderer{ sdlRenderer }
    , m_resourceManager{ resourceManager }
{
    spdlog::trace("构造 Renderer...");

    if (m_sdlRenderer == nullptr) {
        throw std::runtime_error("Renderer 构造失败: 提供的 SDL_Renderer 指针为空。");
    }
    if (m_resourceManager == nullptr) {
        // ResourceManager 是 drawSprite 所必需的
        throw std::runtime_error("Renderer 构造失败: 提供的 ResourceManager 指针为空。");
    }

    setDrawColor(0, 0, 0, 255);

    spdlog::trace("Renderer 构造成功。");
}

void Renderer::drawSprite(const Camera& camera,
                          const component::Sprite& sprite,
                          glm::vec2 position,
                          glm::vec2 size,
                          float rotation)
{
    SDL_Texture* texture{ m_resourceManager->getTexture(sprite.m_textureId, sprite.m_texturePath) };
    if (texture == nullptr) {
        spdlog::error("无法为 ID {} 获取纹理。", sprite.m_textureId);
        return;
    }

    // 应用相机变换
    const glm::vec2 positionScreen{ camera.worldToScreen(position) };

    // 计算目标矩形，注意 position 是精灵的左上角坐标
    const SDL_FRect destRect{
        .x = positionScreen.x, .y = positionScreen.y, .w = size.x, .h = size.y
    };

    // 视口裁剪：如果精灵超出视口，则不绘制
    if (!Renderer::isRectInViewport(camera, destRect)) {
        // spdlog::info("精灵超出视口范围, ID: {}", sprite.textureId());
        return;
    }

    const SDL_FRect srcRect{ .x = sprite.m_sourceRect.position.x,
                             .y = sprite.m_sourceRect.position.y,
                             .w = sprite.m_sourceRect.size.x,
                             .h = sprite.m_sourceRect.size.y };

    // 执行绘制(默认旋转中心为精灵的中心点)
    if (!SDL_RenderTextureRotated(m_sdlRenderer,
                                  texture,
                                  &srcRect,
                                  &destRect,
                                  rotation,
                                  nullptr,
                                  sprite.m_isFlipped ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("渲染旋转纹理失败(ID: {}): {}", sprite.m_textureId, SDL_GetError());
    }
}

void Renderer::drawUiImage(const Image& image,
                           const glm::vec2& position,
                           const std::optional<glm::vec2>& size)
{
    SDL_Texture* texture{ m_resourceManager->getTexture(image.textureId()) };
    if (texture == nullptr) {
        spdlog::error("无法为 ID {} 获取纹理。", image.textureId());
        return;
    }

    auto srcRect = getImageSrcRect(image);
    if (!srcRect.has_value()) {
        spdlog::error("无法获取 Image 对象的源矩形, ID: {}", image.textureId());
        return;
    }

    SDL_FRect destRect{
        .x = position.x, .y = position.y, .w = 0, .h = 0
    }; // 首先确定目标矩形的左上角坐标
    if (size.has_value()) {
        // 如果提供了尺寸，则使用提供的尺寸
        destRect.w = size.value().x;
        destRect.h = size.value().y;
    } else {
        // 如果未提供尺寸，则使用纹理的原始尺寸
        destRect.w = srcRect.value().w;
        destRect.h = srcRect.value().h;
    }

    // 执行绘制(未考虑UI旋转)
    if (!SDL_RenderTextureRotated(m_sdlRenderer,
                                  texture,
                                  &srcRect.value(),
                                  &destRect,
                                  0.0,
                                  nullptr,
                                  image.isFlipped() ? SDL_FLIP_HORIZONTAL : SDL_FLIP_NONE)) {
        spdlog::error("渲染 UI Image 失败 (ID: {}): {}", image.textureId(), SDL_GetError());
    }
}

void Renderer::drawUiFilledRect(const engine::utils::Rect& rect, const engine::utils::FColor& color)
{
    setDrawColorFloat(color.r, color.g, color.b, color.a);

    const SDL_FRect sdlRect{
        .x = rect.position.x, .y = rect.position.y, .w = rect.size.x, .h = rect.size.y
    };
    if (!SDL_RenderFillRect(m_sdlRenderer, &sdlRect)) {
        spdlog::error("绘制填充矩形失败：{}", SDL_GetError());
    }

    setDrawColor(0, 0, 0, 255);
}

void Renderer::present()
{
    SDL_RenderPresent(m_sdlRenderer);
}

void Renderer::clearScreen()
{
    setDrawColorFloat(m_backgroundColor.r,
                      m_backgroundColor.g,
                      m_backgroundColor.b,
                      m_backgroundColor.a);

    if (!SDL_RenderClear(m_sdlRenderer)) {
        spdlog::error("清除渲染器失败：{}", SDL_GetError());
    }
}

void Renderer::setDrawColor(Uint8 red, Uint8 green, Uint8 blue, Uint8 alpha)
{
    if (!SDL_SetRenderDrawColor(m_sdlRenderer, red, green, blue, alpha)) {
        spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
    }
}

void Renderer::setDrawColorFloat(float red, float green, float blue, float alpha)
{
    if (!SDL_SetRenderDrawColorFloat(m_sdlRenderer, red, green, blue, alpha)) {
        spdlog::error("设置渲染绘制颜色失败：{}", SDL_GetError());
    }
}

std::optional<SDL_FRect> Renderer::getImageSrcRect(const Image& image)
{
    SDL_Texture* texture{ m_resourceManager->getTexture(image.textureId()) };
    if (texture == nullptr) {
        spdlog::error("无法为 ID {} 获取纹理。", image.textureId());
        return std::nullopt;
    }

    auto srcRect = image.sourceRect();
    if (srcRect.has_value()) {
        if (srcRect.value().size.x <= 0 || srcRect.value().size.y <= 0) {
            spdlog::error("源矩形尺寸无效, ID: {}, path: {}",
                          image.textureId(),
                          image.texturePath());
            return std::nullopt;
        }
        return SDL_FRect{ .x = srcRect.value().position.x,
                          .y = srcRect.value().position.y,
                          .w = srcRect.value().size.x,
                          .h = srcRect.value().size.y };
    } else {
        SDL_FRect defaultRect{ .x = 0.0F, .y = 0.0F, .w = 0.0F, .h = 0.0F };
        if (!SDL_GetTextureSize(texture, &defaultRect.w, &defaultRect.h)) {
            spdlog::error("无法获取纹理尺寸, ID: {}, path: {}",
                          image.textureId(),
                          image.texturePath());
            return std::nullopt;
        }
        return defaultRect;
    }
}

bool Renderer::isRectInViewport(const Camera& camera, const SDL_FRect& rect)
{
    const glm::vec2 viewportSize{ camera.viewportSize() };
    // 相当于 AABB 碰撞检测
    return rect.x + rect.w >= 0 && rect.x <= viewportSize.x && rect.y + rect.h >= 0
           && rect.y <= viewportSize.y;
}

} // namespace engine::render
