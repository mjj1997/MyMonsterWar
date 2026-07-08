#include "ui_button.h"
#include "state/ui_normal_state.h"

#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace engine::ui {

UiButton::UiButton(engine::core::Context& context,
                   std::string_view normalSpritePath,
                   std::string_view hoverSpritePath,
                   std::string_view pressedSpritePath,
                   glm::vec2 localPosition,
                   glm::vec2 size,
                   std::function<void()> callback)
    : UiInteractiveElementBase{ context, localPosition, size }
    , m_callback{ std::move(callback) }
{
    // 添加各状态对应的精灵
    addSprite("normal"_hs, engine::render::Sprite{ normalSpritePath });
    addSprite("hover"_hs, engine::render::Sprite{ hoverSpritePath });
    addSprite("pressed"_hs, engine::render::Sprite{ pressedSpritePath });

    // 设置默认状态为"normal"
    setCurrentState(std::make_unique<engine::ui::state::UiNormalState>(this));

    // 添加各状态对应的音效
    addSound("hover"_hs, "assets/audio/button_hover.wav"_hs);
    addSound("pressed"_hs, "assets/audio/button_click.wav"_hs);

    spdlog::trace("UiButton 构造完成");
}

void UiButton::clicked()
{
    if (m_callback) {
        m_callback();
    }
}

} // namespace engine::ui
