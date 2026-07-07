#include "ui_label.h"
#include "../render/text_renderer.h"

#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

namespace engine::ui {

UiLabel::UiLabel(engine::render::TextRenderer& textRenderer,
                 std::string_view text,
                 std::string_view fontPath,
                 int fontSize,
                 engine::utils::FColor textColor,
                 glm::vec2 localPosition)
    : UiElementBase{ localPosition }
    , m_textRenderer{ textRenderer }
    , m_text{ text }
    , m_fontPath{ fontPath }
    , m_fontId{ entt::hashed_string(fontPath.data()) }
    , m_fontSize{ fontSize }
    , m_textColor{ textColor }
{
    m_size = m_textRenderer.getTextSize(m_text, m_fontId, m_fontSize);
    spdlog::trace("UiLabel 构造完成");
}

void UiLabel::render(engine::core::Context& context)
{
    if (!m_isVisible || m_text.empty()) {
        return; // 如果不可见或文本为空，不绘制
    }

    // 渲染自身
    m_textRenderer.drawUiText(m_text, m_fontId, m_fontSize, screenPosition(), m_textColor);

    // 渲染子元素（调用基类的 render 方法）
    UiElementBase::render(context);
}

void UiLabel::setText(std::string_view text)
{
    m_text = text;
    m_size = m_textRenderer.getTextSize(m_text, m_fontId, m_fontSize);
}

void UiLabel::setFontPath(std::string_view fontPath)
{
    m_fontPath = fontPath;
    m_fontId = entt::hashed_string(fontPath.data());
    m_size = m_textRenderer.getTextSize(m_text, m_fontId, m_fontSize);
}

void UiLabel::setFontSize(int fontSize)
{
    m_fontSize = fontSize;
    m_size = m_textRenderer.getTextSize(m_text, m_fontId, m_fontSize);
}

void UiLabel::setTextColor(engine::utils::FColor textColor)
{
    m_textColor = textColor;
    /* 颜色变化, 不需要更新尺寸 */
}

} // namespace engine::ui
