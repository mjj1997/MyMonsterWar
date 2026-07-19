#pragma once

#include "../data/path_node.h"
#include "../system/fwd.h"

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
    std::unique_ptr<engine::system::YSortSystem> m_ySortSystem;
    std::unique_ptr<game::system::FollowPathSystem> m_followPathSystem;

    std::unordered_map<int, game::data::PathNode> m_pathNodes; // 路径节点ID -> 路径节点
    std::vector<int> m_startpointIds;                          // 起点ID列表
};

} // namespace game::scene
