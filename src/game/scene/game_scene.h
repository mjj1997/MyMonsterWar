#pragma once

#include "../../engine/scene/scene_base.h"

namespace game::scene {

class GameScene final : public engine::scene::SceneBase
{
public:
    GameScene(engine::core::Context& context, engine::scene::SceneManager& sceneManager);
    ~GameScene();

    void init() override;
    void clean() override;

private:
    // --- 测试输入回调事件 ---
    void attack();
    void jump();
};

} // namespace game::scene
