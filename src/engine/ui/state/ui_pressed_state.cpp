#include "ui_pressed_state.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../ui_interactive_element_base.h"
#include "ui_hover_state.h"
#include "ui_normal_state.h"

#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace engine::ui::state {

UiPressedState::UiPressedState(engine::ui::UiInteractiveElementBase* owner)
    : UiStateBase{ owner }
{
    m_owner->context()
        .inputManager()
        .actionSink("mouse_left"_hs, engine::input::ActionState::Released)
        .connect<&UiPressedState::onMouseReleased>(this);
}

UiPressedState::~UiPressedState()
{
    m_owner->context()
        .inputManager()
        .actionSink("mouse_left"_hs, engine::input::ActionState::Released)
        .disconnect<&UiPressedState::onMouseReleased>(this);
}

void UiPressedState::enter()
{
    // 设置 UI 为按下状态的图片
    m_owner->setCurrentImage("pressed"_hs);
    // 播放按下时的音效
    m_owner->playSound("ui_click"_hs);

    spdlog::debug("切换到按下状态");
}

void UiPressedState::onMouseReleased()
{
    const auto& inputManager = m_owner->context().inputManager();
    const auto& mousePos = inputManager.logicalMousePosition();
    if (!m_owner->isPointInside(mousePos)) {
        // 松开鼠标时，如果鼠标不在 UI 元素内，切换到正常状态
        m_owner->setNextState(std::make_unique<UiNormalState>(m_owner));
    } else { // 松开鼠标时，如果鼠标在 UI 元素内，触发点击事件
        m_owner->setNextState(std::make_unique<UiHoverState>(m_owner));
        // 触发点击事件
        m_owner->clicked();
    }
}

} // namespace engine::ui::state
