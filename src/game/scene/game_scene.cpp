#include "game_scene.h"

#include "../../engine/core/context.h"
#include "../../engine/loader/level_loader.h"
#include "../../engine/system/animation_system.h"
#include "../../engine/system/movement_system.h"
#include "../../engine/system/render_system.h"
#include "../../engine/system/y_sort_system.h"

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
    /* 不调用 setEntityBuilder，则使用默认的 BasicEntityBuilder */
    if (!levelLoader.loadLevel("assets/maps/level1.tmj", this)) {
        spdlog::error("加载关卡失败");
        return false;
    }

    return true;
}

} // namespace game::scene
