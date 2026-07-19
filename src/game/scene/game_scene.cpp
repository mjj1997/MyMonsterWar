#include "game_scene.h"
#include "../loader/entity_builder_mw.h"
#include "../system/follow_path_system.h"

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

    spdlog::info("GameScene 构造完成");
}

GameScene::~GameScene() = default;

void GameScene::init()
{
    if (!loadLevel()) {
        return;
    }

    SceneBase::init();
}

void GameScene::update(float deltaTime)
{
    auto& dispatcher = m_context.dispatcher();

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

} // namespace game::scene
