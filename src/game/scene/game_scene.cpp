#include "game_scene.h"
#include "../component/enemy_component.h"
#include "../component/player_component.h"
#include "../data/entity_blueprint.h"
#include "../factory/blueprint_manager.h"
#include "../factory/entity_factory.h"
#include "../loader/entity_builder_mw.h"
#include "../system/block_system.h"
#include "../system/follow_path_system.h"
#include "../system/remove_dead_system.h"

#include "../../engine/component/render_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"
#include "../../engine/core/context.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/loader/level_loader.h"
#include "../../engine/system/animation_system.h"
#include "../../engine/system/movement_system.h"
#include "../../engine/system/render_system.h"
#include "../../engine/system/y_sort_system.h"

#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::scene {

GameScene::GameScene(engine::core::Context& context)
    : SceneBase{ "GameScene", context }
{
    auto& dispatcher = m_context.dispatcher();

    // 初始化系统
    m_movementSystem = std::make_unique<engine::system::MovementSystem>();
    m_renderSystem = std::make_unique<engine::system::RenderSystem>();
    m_animationSystem = std::make_unique<engine::system::AnimationSystem>(m_registry, dispatcher);
    m_ySortSystem = std::make_unique<engine::system::YSortSystem>();

    m_followPathSystem = std::make_unique<game::system::FollowPathSystem>();
    m_removeDeadSystem = std::make_unique<game::system::RemoveDeadSystem>();
    m_blockSystem = std::make_unique<game::system::BlockSystem>();

    spdlog::info("GameScene 构造完成");
}

GameScene::~GameScene() = default;

void GameScene::init()
{
    if (!loadLevel()) {
        return;
    }
    if (!initEventConnections()) {
        spdlog::error("初始化事件连接失败");
        return;
    }
    if (!initInputConnections()) {
        spdlog::error("初始化输入连接失败");
        return;
    }
    if (!initEntityFactory()) {
        spdlog::error("初始化实体工厂失败");
        return;
    }
    createTestEnemy();

    SceneBase::init();
}

void GameScene::update(float deltaTime)
{
    auto& dispatcher = m_context.dispatcher();

    // 每一帧最先清理死亡实体(要在 dispatcher 处理完事件后再清理, 因此放在下一帧开头)
    m_removeDeadSystem->update(m_registry);

    // 注意系统更新顺序, 路径跟随系统要放到移动系统之前
    m_followPathSystem->update(m_registry, dispatcher, m_pathNodes);
    m_blockSystem->update(m_registry, dispatcher);
    m_movementSystem->update(m_registry, deltaTime);
    m_ySortSystem->update(m_registry); // 调用顺序要放到移动系统之后
    m_animationSystem->update(deltaTime);

    SceneBase::update(deltaTime);
}

void GameScene::render()
{
    m_renderSystem->update(m_registry, m_context.renderer(), m_context.camera());

    SceneBase::render();
}

void GameScene::clean()
{
    // 断开所有事件连接
    auto& dispatcher = m_context.dispatcher();
    dispatcher.disconnect(this);

    // 断开输入信号连接
    auto& inputManager = m_context.inputManager();
    inputManager.actionSink("mouse_right"_hs).disconnect<&GameScene::createTestPlayerMelee>(this);
    inputManager.actionSink("mouse_left"_hs).disconnect<&GameScene::createTestPlayerRanged>(this);
    inputManager.actionSink("pause"_hs).disconnect<&GameScene::clearAllPlayers>(this);

    SceneBase::clean();
}

bool GameScene::loadLevel()
{
    engine::loader::LevelLoader levelLoader;
    levelLoader.setEntityBuilder(std::make_unique<game::loader::EntityBuilderMW>(levelLoader,
                                                                                 m_context,
                                                                                 m_registry,
                                                                                 m_pathNodes,
                                                                                 m_startpointIds));
    if (!levelLoader.loadLevel("assets/maps/level1.tmj", this)) {
        spdlog::error("加载关卡失败");
        return false;
    }

    return true;
}

bool GameScene::initEventConnections()
{
    auto& dispatcher = m_context.dispatcher();
    // 连接敌人到达基地事件
    dispatcher.sink<game::defs::EnemyArriveBaseEvent>().connect<&GameScene::onEnemyArriveBase>(this);

    return true;
}

bool GameScene::initInputConnections()
{
    auto& inputManager = m_context.inputManager();
    inputManager.actionSink("mouse_right"_hs).connect<&GameScene::createTestPlayerMelee>(this);
    inputManager.actionSink("mouse_left"_hs).connect<&GameScene::createTestPlayerRanged>(this);
    inputManager.actionSink("pause"_hs).connect<&GameScene::clearAllPlayers>(this);
    return true;
}

bool GameScene::initEntityFactory()
{
    // 如果蓝图管理器为空，则创建一个（将来可能由构造函数传入）
    if (m_blueprintManager == nullptr) {
        m_blueprintManager = std::make_shared<game::factory::BlueprintManager>(
            m_context.resourceManager());
        if (!m_blueprintManager->loadEnemyClassBlueprints("assets/data/enemy_data.json")
            || !m_blueprintManager->loadPlayerClassBlueprints("assets/data/player_data.json")) {
            spdlog::error("加载蓝图失败");
            return false;
        }
    }

    m_entityFactory = std::make_unique<game::factory::EntityFactory>(m_registry,
                                                                     *m_blueprintManager);
    spdlog::info("实体工厂初始化完成");
    return true;
}

void GameScene::onEnemyArriveBase(const game::defs::EnemyArriveBaseEvent& event)
{
    spdlog::info("敌人到达基地");
    // TODO: 处理敌人到达基地的逻辑
}

void GameScene::createTestEnemy()
{
    // 每个起点创建一批敌人
    for (auto startpointId : m_startpointIds) {
        auto position = m_pathNodes.at(startpointId).m_position;

        m_entityFactory->createEnemyUnit("wolf"_hs, position, startpointId);
        m_entityFactory->createEnemyUnit("slime"_hs, position, startpointId);
        m_entityFactory->createEnemyUnit("goblin"_hs, position, startpointId);
        m_entityFactory->createEnemyUnit("dark_witch"_hs, position, startpointId);
    }
}

bool GameScene::createTestPlayerMelee()
{
    auto position = m_context.inputManager().logicalMousePosition();
    m_entityFactory->createPlayerUnit("warrior"_hs, position);
    spdlog::info("创建战士: 位置: {}, {}", position.x, position.y);
    return true;
}

bool GameScene::createTestPlayerRanged()
{
    auto position = m_context.inputManager().logicalMousePosition();
    m_entityFactory->createPlayerUnit("archer"_hs, position);
    spdlog::info("创建弓箭手: 位置: {}, {}", position.x, position.y);
    return true;
}

bool GameScene::clearAllPlayers()
{
    auto view = m_registry.view<game::component::PlayerComponent>();
    for (auto entity : view) {
        m_registry.destroy(entity);
    }
    return true;
}

} // namespace game::scene
