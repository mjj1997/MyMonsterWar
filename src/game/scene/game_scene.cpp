#include "game_scene.h"
#include "../component/enemy_component.h"
#include "../loader/entity_builder_mw.h"
#include "../system/follow_path_system.h"
#include "../system/remove_dead_system.h"

#include "../../engine/component/render_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"
#include "../../engine/core/context.h"
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
    // 初始化系统
    m_movementSystem = std::make_unique<engine::system::MovementSystem>();
    m_renderSystem = std::make_unique<engine::system::RenderSystem>();
    m_animationSystem = std::make_unique<engine::system::AnimationSystem>();
    m_ySortSystem = std::make_unique<engine::system::YSortSystem>();

    m_followPathSystem = std::make_unique<game::system::FollowPathSystem>();
    m_removeDeadSystem = std::make_unique<game::system::RemoveDeadSystem>();

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
    m_movementSystem->update(m_registry, deltaTime);
    m_ySortSystem->update(m_registry); // 调用顺序要放到移动系统之后
    m_animationSystem->update(m_registry, deltaTime);

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

void GameScene::onEnemyArriveBase(const game::defs::EnemyArriveBaseEvent& event)
{
    spdlog::info("敌人到达基地");
    // TODO: 处理敌人到达基地的逻辑
}

void GameScene::createTestEnemy()
{
    // 每个起点创建一个敌人
    for (auto startpointId : m_startpointIds) {
        auto position = m_pathNodes.at(startpointId).m_position;

        auto enemy = m_registry.create();
        m_registry.emplace<engine::component::TransformComponent>(enemy, position);
        m_registry.emplace<engine::component::VelocityComponent>(enemy, glm::vec2(0.0F));
        m_registry.emplace<game::component::EnemyComponent>(enemy, startpointId, 100.0F);

        auto sprite = engine::component::Sprite{ "assets/textures/Enemy/wolf.png",
                                                 engine::utils::Rect{ 0, 0, 192, 192 } };
        // 设置精灵组件时, 需设置偏移量以调整中心点位置(否则会默认以左上角为中心点)
        m_registry.emplace<engine::component::SpriteComponent>(enemy,
                                                               std::move(sprite),
                                                               glm::vec2(192.0F),
                                                               glm::vec2{ -96.0F, -128.0F });
        // 暂定主战斗图层编号为 10
        m_registry.emplace<engine::component::RenderComponent>(enemy, 10);
    }
}

} // namespace game::scene
