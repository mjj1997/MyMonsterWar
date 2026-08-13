#include "ui_hover_state.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../ui_interactive_element_base.h"
#include "ui_normal_state.h"
#include "ui_pressed_state.h"

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace engine::ui::state {

void UiHoverState::enter()
{
    // 设置 UI 为悬停状态的图片
    m_owner->setImage("hover"_hs);
}

void UiHoverState::update(float deltaTime, engine::core::Context& context)
{
    const auto& inputManager = context.inputManager();
    const auto& mousePos = inputManager.logicalMousePosition();
    if (!m_owner->isPointInside(mousePos)) { // 如果鼠标不在 UI 元素内，切换到正常状态
        m_owner->hoverLeft();
        m_owner->setNextState(std::make_unique<UiNormalState>(m_owner));
    }
}

bool UiHoverState::onMousePressed()
{
    m_owner->setNextState(std::make_unique<UiPressedState>(m_owner));
    return true;
}

} // namespace engine::ui::state
