#include "game_scene.h"

#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/utils/events.h"

#include <entt/signal/dispatcher.hpp>
#include <entt/signal/sigh.hpp>
#include <spdlog/spdlog.h>

namespace game::scene {

GameScene::GameScene(engine::core::Context& context)
    : SceneBase{ "GameScene", context }
{}

GameScene::~GameScene() = default;

void GameScene::init()
{
    // 注册输入回调事件 (J,K 键)
    auto& inputManager = m_context.inputManager();
    inputManager.actionSignal("attack").connect<&GameScene::attack>(this); // 默认绑定 Pressed
    inputManager.actionSignal("jump", engine::input::ActionState::Released)
        .connect<&GameScene::jump>(this);
}

void GameScene::clean()
{
    // 断开输入回调事件 (谁连接，谁负责断开)
    auto& inputManager = m_context.inputManager();
    inputManager.actionSignal("attack").disconnect<&GameScene::attack>(this);
    inputManager.actionSignal("jump", engine::input::ActionState::Released)
        .disconnect<&GameScene::jump>(this);
}

void GameScene::pushScene()
{
    spdlog::info("发出 pushScene signal, 压入场景");
    emitPushSceneSignal(std::make_unique<GameScene>(m_context));
}

void GameScene::popScene()
{
    spdlog::info("发出 popScene signal, 弹出场景");
    emitPopSceneSignal();
}

void GameScene::replaceScene()
{
    spdlog::info("发出 replaceScene signal, 替换场景");
    emitReplaceSceneSignal(std::make_unique<GameScene>(m_context));
}

void GameScene::quit()
{
    spdlog::info("发出 quit signal, 退出游戏");
    emitQuitSignal();
}

} // namespace game::scene
