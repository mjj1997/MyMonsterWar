#include "game_scene.h"

namespace game::scene {

GameScene::GameScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager)
    : SceneBase{ "GameScene", context, sceneManager }
{}

GameScene::~GameScene() = default;

} // namespace game::scene