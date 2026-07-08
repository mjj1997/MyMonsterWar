#include "scene_base.h"
#include "../core/context.h"
#include "../core/game_state.h"
#include "../render/camera.h"
#include "../ui/ui_manager.h"
#include "../utils/events.h"

#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

#include <algorithm> // for std::remove_if

namespace engine::scene {

SceneBase::SceneBase(std::string_view name, engine::core::Context& context)
    : m_sceneName{ name }
    , m_context{ context }
    , m_uiManager{ std::make_unique<engine::ui::UiManager>() }
{
    spdlog::trace("场景 '{}' 构造完成。", m_sceneName);
}

SceneBase::~SceneBase() = default;

void SceneBase::init()
{
    m_isInitialized = true; // 子类应该最后调用父类的 init 方法
    spdlog::trace("场景 '{}' 初始化完成。", m_sceneName);
}

void SceneBase::update(float deltaTime)
{
    if (!m_isInitialized) {
        return;
    }

    // 更新 UI 管理器
    m_uiManager->update(deltaTime, m_context);
}

void SceneBase::render()
{
    if (!m_isInitialized) {
        return;
    }

    // 渲染 UI 管理器
    m_uiManager->render(m_context);
}

void SceneBase::handleInput()
{
    if (!m_isInitialized) {
        return;
    }

    // 处理 UI 管理器的输入
    if (m_uiManager->handleInput(m_context)) {
        return; // 如果 UI 管理器处理了输入，就直接返回。不需要继续让游戏对象处理输入
    }
}

void SceneBase::clean()
{
    if (!m_isInitialized) {
        return;
    }

    m_isInitialized = false; // 清理完成后，设置场景为未初始化
    spdlog::trace("场景 '{}' 清理完成。", m_sceneName);
}

void SceneBase::emitPushSceneSignal(std::unique_ptr<SceneBase>&& scene)
{
    m_context.dispatcher().trigger<engine::utils::PushSceneEvent>(
        engine::utils::PushSceneEvent{ std::move(scene) });
}

void SceneBase::emitPopSceneSignal()
{
    m_context.dispatcher().trigger<engine::utils::PopSceneEvent>();
}

void SceneBase::emitReplaceSceneSignal(std::unique_ptr<SceneBase>&& scene)
{
    m_context.dispatcher().trigger<engine::utils::ReplaceSceneEvent>(
        engine::utils::ReplaceSceneEvent{ std::move(scene) });
}

void SceneBase::emitQuitSignal()
{
    m_context.dispatcher().trigger<engine::utils::QuitEvent>();
}

} // namespace engine::scene
