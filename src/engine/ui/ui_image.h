#pragma once

#include "../render/sprite.h"
#include "ui_element_base.h"

#include <optional>
#include <string_view>

namespace engine::ui {

/**
 * @brief 用于显示纹理或部分纹理的 UI 元素
 *
 * Image 添加了渲染图像的功能。
 */
class UiImage final : public UiElementBase
{
public:
    /**
     * @brief 构造一个 UiImage 对象。（通过纹理路径构造）
     *
     * @param texturePath 要显示的纹理路径。
     * @param localPosition UiImage 的局部位置  
     * @param size UiImage 的大小（如果为 {0.0F, 0.0F}，则使用纹理的原始尺寸）
     * @param sourceRect 可选：要绘制的纹理区域（如果未指定，则绘制整个纹理）
     * @param isFlipped 可选：精灵是否应该水平翻转。
     */
    explicit UiImage(std::string_view texturePath,
                     glm::vec2 localPosition = { 0.0F, 0.0F },
                     glm::vec2 size = { 0.0F, 0.0F },
                     std::optional<engine::utils::Rect> sourceRect = std::nullopt,
                     bool isFlipped = false);

    /**
     * @brief 构造一个 UiImage 对象。（通过纹理 ID 构造）
     *
     * @param textureId 要显示的纹理 ID
     * @param localPosition UiImage 的局部位置
     * @param size UiImage 的大小（如果为 {0.0F, 0.0F}，则使用纹理的原始尺寸）
     * @param sourceRect 可选：要绘制的纹理区域（如果未指定，则绘制整个纹理）
     * @param isFlipped 可选：精灵是否应该水平翻转。
     * @note 用此方法，需确保对应 ID 的纹理已经加载到 ResourceManager 中，因此不需要再提供纹理路径。
     */
    explicit UiImage(entt::id_type textureId,
                     glm::vec2 localPosition = { 0.0F, 0.0F },
                     glm::vec2 size = { 0.0F, 0.0F },
                     std::optional<engine::utils::Rect> sourceRect = std::nullopt,
                     bool isFlipped = false);

    void render(engine::core::Context& context) override;

    // --- Setters & Getters ---
    const engine::render::Sprite& sprite() const { return m_sprite; }
    void setSprite(engine::render::Sprite sprite) { m_sprite = std::move(sprite); }

    entt::id_type textureId() const { return m_sprite.textureId(); }
    void setTexture(std::string_view texturePath) { m_sprite.setTexture(texturePath); }

    const std::optional<engine::utils::Rect>& sourceRect() const { return m_sprite.sourceRect(); }
    void setSourceRect(std::optional<engine::utils::Rect> sourceRect)
    {
        m_sprite.setSourceRect(std::move(sourceRect));
    }

    bool isFlipped() const { return m_sprite.isFlipped(); }
    void setFlipped(bool flipped) { m_sprite.setFlipped(flipped); }

private:
    engine::render::Sprite m_sprite;
};

} // namespace engine::ui
