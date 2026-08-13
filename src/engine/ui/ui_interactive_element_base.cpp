#include "ui_interactive_element_base.h"
#include "../audio/audio_player.h"
#include "../core/context.h"
#include "../render/renderer.h"
#include "../resource/resource_manager.h"

#include <spdlog/spdlog.h>

namespace engine::ui {

UiInteractiveElementBase::UiInteractiveElementBase(engine::core::Context& context,
                                                   glm::vec2 localPosition,
                                                   glm::vec2 size)
    : UiElementBase{ localPosition, size }
    , m_context{ context }
{
    spdlog::trace("UIInteractiveElementBase 构造完成");
}

void UiInteractiveElementBase::update(float deltaTime, engine::core::Context& context)
{
    // 先更新子 UI 元素（调用基类的 update 方法）
    UiElementBase::update(deltaTime, context);

    // 再更新自己（状态）
    if (m_currentState != nullptr && m_isInteractive) {
        if (m_nextState) {
            setCurrentState(std::move(m_nextState));
            m_nextState.reset();
        }
        m_currentState->update(deltaTime, context);
    }
}

void UiInteractiveElementBase::render(engine::core::Context& context)
{
    if (!m_isVisible) {
        return;
    }

    // 先渲染当前状态对应的图片
    if (m_currentImageId != entt::null) {
        m_context.renderer().drawUiImage(m_images[m_currentImageId], screenPosition(), m_size);
    }

    // 再渲染子 UI 元素（调用基类的 render 方法）
    UiElementBase::render(context);
}

void UiInteractiveElementBase::addImage(entt::id_type nameId, engine::render::Image image)
{
    // 可交互 UI 元素必须有一个 size 用于交互检测，因此如果参数列表中没有指定，则用图片大小作为 size
    if (m_size.x == 0.0F && m_size.y == 0.0F) {
        m_size = m_context.resourceManager().getTextureSize(image.texturePathId());
    }

    // 添加图片
    m_images.emplace(nameId, std::move(image));
}

void UiInteractiveElementBase::setCurrentImage(entt::id_type nameId)
{
    if (auto iter = m_images.find(nameId); iter != m_images.end()) {
        m_currentImageId = nameId;
    } else {
        spdlog::warn("Image '{}' 未找到。", nameId);
    }
}

void UiInteractiveElementBase::addSound(entt::id_type nameId, entt::hashed_string hashedPath)
{
    m_sounds.emplace(nameId, hashedPath.value());
}

void UiInteractiveElementBase::playSound(entt::id_type nameId)
{
    if (auto iter = m_sounds.find(nameId); iter != m_sounds.end()) {
        m_context.audioPlayer().playSound(iter->second);
    } else {
        spdlog::warn("Sound '{}' 未找到。", nameId);
    }
}

void UiInteractiveElementBase::setNextState(std::unique_ptr<engine::ui::state::UiStateBase> state)
{
    m_nextState = std::move(state);
}

void UiInteractiveElementBase::setCurrentState(std::unique_ptr<engine::ui::state::UiStateBase> state)
{
    if (state == nullptr) {
        spdlog::error("尝试设置空的状态！");
        return;
    }

    m_currentState = std::move(state);
    m_currentState->enter();
}

} // namespace engine::ui
