#include "ui_button.h"
#include "state/ui_normal_state.h"

#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace engine::ui {

UiButton::UiButton(engine::core::Context& context,
                   engine::render::Image normalImage,
                   engine::render::Image hoverImage,
                   engine::render::Image pressedImage,
                   glm::vec2 localPosition,
                   glm::vec2 size,
                   std::function<void()> clickedCallback,
                   std::function<void()> hoverEnteredCallback,
                   std::function<void()> hoverLeftCallback)
    : UiInteractiveElementBase{ context, localPosition, size }
    , m_clickedCallback{ std::move(clickedCallback) }
    , m_hoverEnteredCallback{ std::move(hoverEnteredCallback) }
    , m_hoverLeftCallback{ std::move(hoverLeftCallback) }
{
    // 添加各状态对应的图片
    addImage("normal"_hs, std::move(normalImage));
    addImage("hover"_hs, std::move(hoverImage));
    addImage("pressed"_hs, std::move(pressedImage));

    // 设置默认状态为"normal"
    setCurrentState(std::make_unique<engine::ui::state::UiNormalState>(this));

    spdlog::trace("UiButton 构造完成");
}

void UiButton::clicked()
{
    if (m_clickedCallback) {
        m_clickedCallback();
    }
}

void UiButton::hoverEntered()
{
    if (m_hoverEnteredCallback) {
        m_hoverEnteredCallback();
    }
}

void UiButton::hoverLeft()
{
    if (m_hoverLeftCallback) {
        m_hoverLeftCallback();
    }
}

} // namespace engine::ui
