#include "ui_image.h"
#include "../core/context.h"
#include "../render/renderer.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

UiImage::UiImage(std::string_view texturePath,
                 glm::vec2 localPosition,
                 glm::vec2 size,
                 std::optional<engine::utils::Rect> sourceRect,
                 bool isFlipped)
    : UiElementBase{ localPosition, size }
    , m_image{ texturePath, sourceRect, isFlipped }
{
    if (m_image.textureId() == entt::null) {
        spdlog::warn("创建了一个空纹理 ID 的 UiImage 元素。");
    }

    spdlog::trace("UiImage 构造成功。");
}

UiImage::UiImage(entt::id_type textureId,
                 glm::vec2 localPosition,
                 glm::vec2 size,
                 std::optional<engine::utils::Rect> sourceRect,
                 bool isFlipped)
    : UiElementBase{ localPosition, size }
    , m_image{ textureId, sourceRect, isFlipped }
{
    if (m_image.textureId() == entt::null) {
        spdlog::warn("创建了一个空纹理 ID 的 UiImage 元素。");
    }

    spdlog::trace("UiImage 构造成功。");
}

UiImage::UiImage(const engine::render::Image& image, glm::vec2 localPosition, glm::vec2 size)
    : UiElementBase{ localPosition, size }
    , m_image{ image }
{
    if (m_image.textureId() == entt::null) {
        spdlog::warn("创建了一个空纹理 ID 的 UiImage 元素。");
    }

    spdlog::trace("UiImage 构造成功。");
}

void UiImage::render(engine::core::Context& context)
{
    if (!m_isVisible || m_image.textureId() == entt::null) {
        return; // 如果不可见或纹理 ID 为空，不绘制
    }

    // 渲染自身
    const auto& screenPos = screenPosition();
    if (m_size.x == 0.0F && m_size.y == 0.0F) { // 如果尺寸为 0，则使用纹理的原始尺寸
        context.renderer().drawUiImage(m_image, screenPos);
    } else { // 否则，使用指定的尺寸
        context.renderer().drawUiImage(m_image, screenPos, m_size);
    }

    // 渲染子元素（调用基类的 render 方法）
    UiElementBase::render(context);
}

} // namespace engine::ui
