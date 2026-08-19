#pragma once

#include "../data/path_node.h"
#include "../system/fwd.h"

#include "../../engine/scene/scene_base.h"
#include "../../engine/system/fwd.h"

namespace engine::ui {
class UiElementBase;
}

namespace game::factory {
class EntityFactory;
class BlueprintManager;
} // namespace game::factory

namespace game::data {
class SessionData;
class UiConfig;
struct GameStats;
} // namespace game::data

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
    [[nodiscard]] bool initSessionData();
    [[nodiscard]] bool initUiConfig();
    [[nodiscard]] bool loadLevel();
    [[nodiscard]] bool initEventConnections();
    [[nodiscard]] bool initInputConnections();
    [[nodiscard]] bool initEntityFactory();
    [[nodiscard]] bool initRegistryContext();
    [[nodiscard]] bool initSystems();

    // 测试函数
    void testSessionData();
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
    std::unique_ptr<game::system::ProjectileSystem> m_projectileSystem;
    std::unique_ptr<game::system::HealthBarSystem> m_healthBarSystem;
    std::unique_ptr<game::system::EffectSystem> m_effectSystem;
    std::unique_ptr<game::system::GameRuleSystem> m_gameRuleSystem;

    std::unordered_map<int, game::data::PathNode> m_pathNodes; // 路径节点ID -> 路径节点
    std::vector<int> m_startpointIds;                          // 起点ID列表
    game::data::GameStats m_gameStats;                         // 场景内的游戏统计数据

    std::unique_ptr<game::factory::EntityFactory> m_entityFactory; // 实体工厂, 负责创建和管理实体
    /* 管理数据的实例很可能同时被多个场景使用,因此使用共享指针 */
    std::shared_ptr<game::factory::BlueprintManager> m_blueprintManager; // 蓝图管理器, 负责管理蓝图数据
    std::shared_ptr<game::data::SessionData> m_sessionData; // 会话数据, 负责管理跨关卡传递的数据
    std::shared_ptr<game::data::UiConfig> m_uiConfig;       // UI 配置数据，负责管理 UI 数据

    // --- 跨场景数据 ---
    int m_level{ 1 }; // 当前关卡号
};

} // namespace game::scene
