#pragma once

#include "../../engine/scene/scene_base.h"

namespace game::scene {

class GameScene final : public engine::scene::SceneBase
{
public:
    explicit GameScene(engine::core::Context& context);
    ~GameScene() override;

    void init() override;
    void clean() override;

private:
    // --- 测试输入回调事件 ---
    void attack();
    void jump();
};

} // namespace game::scene
