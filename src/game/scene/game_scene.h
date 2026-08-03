#pragma once

#include "../data/path_node.h"
#include "../defs/events.h"
#include "../system/fwd.h"

#include "../../engine/scene/scene_base.h"
#include "../../engine/system/fwd.h"

namespace game::factory {
class EntityFactory;
class BlueprintManager;
} // namespace game::factory

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
    [[nodiscard]] bool initEventConnections();
    [[nodiscard]] bool initInputConnections();
    [[nodiscard]] bool initEntityFactory();
    [[nodiscard]] bool initSystems();

    // 事件回调函数
    void onEnemyArriveBase(const game::defs::EnemyArriveBaseEvent& event);

    // 测试函数
    void createTestEnemy();
    bool createTestPlayerMelee();
    bool createTestPlayerRanged();
    bool createTestPlayerHealer();
    bool clearAllPlayers();

    std::unique_ptr<engine::system::MovementSystem> m_movementSystem;
    std::unique_ptr<engine::system::RenderSystem> m_renderSystem;
    std::unique_ptr<engine::system::AnimationSystem> m_animationSystem;
    std::unique_ptr<engine::system::YSortSystem> m_ySortSystem;
    std::unique_ptr<engine::system::AudioSystem> m_audioSystem;

    std::unique_ptr<game::system::FollowPathSystem> m_followPathSystem;
    std::unique_ptr<game::system::RemoveDeadSystem> m_removeDeadSystem;
    std::unique_ptr<game::system::BlockSystem> m_blockSystem;
    std::unique_ptr<game::system::SetTargetSystem> m_setTargetSystem;
    std::unique_ptr<game::system::TimerSystem> m_timerSystem;
    std::unique_ptr<game::system::AttackStarterSystem> m_attackStarterSystem;
    std::unique_ptr<game::system::AnimationStateSystem> m_animationStateSystem;
    std::unique_ptr<game::system::OrientationSystem> m_orientationSystem;
    std::unique_ptr<game::system::AnimationKeyframeEventSystem> m_animationKeyframeEventSystem;
    std::unique_ptr<game::system::CombatResolveSystem> m_combatResolveSystem;

    std::unordered_map<int, game::data::PathNode> m_pathNodes; // 路径节点ID -> 路径节点
    std::vector<int> m_startpointIds;                          // 起点ID列表

    std::unique_ptr<game::factory::EntityFactory> m_entityFactory; // 实体工厂, 负责创建和管理实体
    /* 管理数据的实例很可能同时被多个场景使用,因此使用共享指针 */
    std::shared_ptr<game::factory::BlueprintManager> m_blueprintManager; // 蓝图管理器, 负责管理蓝图数据
};

} // namespace game::scene
