#pragma once

#include "../../engine/scene/scene_base.h"
#include "../../engine/system/fwd.h"

namespace game::scene {

class GameScene final : public engine::scene::SceneBase
{
public:
    explicit GameScene(engine::core::Context& context);
    ~GameScene() override;

    void init() override;
    void update(float deltaTime) override;
    void render() override;
    void clean() override;

private:
    [[nodiscard]] bool loadLevel();

    std::unique_ptr<engine::system::MovementSystem> m_movementSystem;
    std::unique_ptr<engine::system::RenderSystem> m_renderSystem;
    std::unique_ptr<engine::system::AnimationSystem> m_animationSystem;
};

} // namespace game::scene
