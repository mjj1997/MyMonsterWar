#pragma once

#include "../../engine/scene/scene_base.h"

namespace game::scene {

class GameScene final : public engine::scene::SceneBase
{
public:
    GameScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager);
    ~GameScene();
};

} // namespace game::scene