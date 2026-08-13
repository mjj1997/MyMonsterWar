#include "ui_normal_state.h"
#include "../../core/context.h"
#include "../../input/input_manager.h"
#include "../ui_interactive_element_base.h"
#include "ui_hover_state.h"

#include <entt/core/hashed_string.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace engine::ui::state {

void UiNormalState::enter()
{
    // 设置 UI 为正常状态的图片
    m_owner->setCurrentImage("normal"_hs);

    spdlog::debug("切换到正常状态");
}

void UiNormalState::update(float deltaTime, engine::core::Context& context)
{
    const auto& inputManager = context.inputManager();
    const auto& mousePos = inputManager.logicalMousePosition();
    if (m_owner->isPointInside(mousePos)) { // 如果鼠标在 UI 元素内，切换到悬停状态
        // 播放悬停时的音效
        m_owner->playSound("ui_hover"_hs);
        m_owner->setNextState(std::make_unique<UiHoverState>(m_owner));
    }
}

} // namespace engine::ui::state
