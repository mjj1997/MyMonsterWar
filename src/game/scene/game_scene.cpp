#include "game_scene.h"

#include "../../engine/audio/audio_player.h"
#include "../../engine/core/context.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"

#include <entt/core/hashed_string.hpp>

using namespace entt::literals;

namespace game::scene {

GameScene::GameScene(engine::core::Context& context)
    : SceneBase{ "GameScene", context }
{}

GameScene::~GameScene() = default;

void GameScene::init()
{
    // 测试资源管理器
    testResourceManager();

    SceneBase::init();
}

void GameScene::clean()
{
    SceneBase::clean();
}

void GameScene::testResourceManager()
{
    // 载入资源
    m_context.resourceManager().loadTexture("assets/textures/Buildings/Castle.png"_hs);
    // 播放音乐
    m_context.audioPlayer().playMusic("battle_bgm"_hs);

    // 测试UI元素（使用载入的资源）
    m_uiManager->addElement(
        std::make_unique<engine::ui::UiImage>("assets/textures/Buildings/Castle.png"_hs));
    m_uiManager->addElement(
        std::make_unique<engine::ui::UiLabel>(m_context.textRenderer(),
                                              "Hello, World!",
                                              "assets/fonts/VonwaonBitmap-16px.ttf"));
}

} // namespace game::scene
