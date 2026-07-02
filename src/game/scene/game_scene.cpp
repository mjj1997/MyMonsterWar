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
    static int count{ 0 };
    ++count;
    m_sceneNum = count;
    spdlog::info("场景编号：{}", m_sceneNum);

    // 注册输入事件处理回调函数
    auto& inputManager = m_context.inputManager();
    inputManager.actionSignal("mouseLeftClick").connect<&GameScene::pushScene>(this); // 鼠标左键点击
    inputManager.actionSignal("mouseRightClick").connect<&GameScene::popScene>(this); // 鼠标右键点击
    inputManager.actionSignal("jump").connect<&GameScene::replaceScene>(this);        // J 键
    inputManager.actionSignal("pause").connect<&GameScene::quit>(this);               // P 键

    SceneBase::init();
}

void GameScene::clean()
{
    // 反注册输入事件处理回调函数 (谁连接，谁负责断开)
    auto& inputManager = m_context.inputManager();
    inputManager.actionSignal("mouseLeftClick").disconnect<&GameScene::pushScene>(this);
    inputManager.actionSignal("mouseRightClick").disconnect<&GameScene::popScene>(this);
    inputManager.actionSignal("jump").disconnect<&GameScene::replaceScene>(this);
    inputManager.actionSignal("pause").disconnect<&GameScene::quit>(this);

    SceneBase::clean();
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
