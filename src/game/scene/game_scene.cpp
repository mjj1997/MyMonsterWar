#include "game_scene.h"

#include "../../engine/audio/audio_player.h"
#include "../../engine/component/animation_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"
#include "../../engine/core/context.h"
#include "../../engine/resource/resource_manager.h"
#include "../../engine/system/animation_system.h"
#include "../../engine/system/movement_system.h"
#include "../../engine/system/render_system.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"

#include <entt/core/hashed_string.hpp>
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

    spdlog::info("GameScene 构造完成");
}

GameScene::~GameScene() = default;

void GameScene::init()
{
    // 测试资源管理器
    testResourceManager();

    SceneBase::init();
}

void GameScene::clean()
{
    SceneBase::clean();
}

void GameScene::testResourceManager()
{
    // 载入资源
    m_context.resourceManager().loadTexture("assets/textures/Buildings/Castle.png"_hs);
    // 播放音乐
    m_context.audioPlayer().playMusic("battle_bgm"_hs);

    // 测试UI元素（使用载入的资源）
    m_uiManager->addElement(
        std::make_unique<engine::ui::UiImage>("assets/textures/Buildings/Castle.png"_hs));
    m_uiManager->addElement(
        std::make_unique<engine::ui::UiLabel>(m_context.textRenderer(),
                                              "Hello, World!",
                                              "assets/fonts/VonwaonBitmap-16px.ttf"));
}

void GameScene::testECS()
{
    auto entity = m_registry.create();
    // 变换、速度、精灵组件
    m_registry.emplace<engine::component::TransformComponent>(entity, glm::vec2(100.0F));
    m_registry.emplace<engine::component::VelocityComponent>(entity, glm::vec2(10.0F));
    m_registry.emplace<engine::component::SpriteComponent>(
        entity,
        engine::component::Sprite("assets/textures/Units/Archer.png",
                                  engine::utils::Rect{ 0.0F, 0.0F, 192.0F, 192.0F }));

    // 动画组件 (单一动画 -> 动画map -> AnimationComponent)
    auto animation = engine::component::Animation{
        { engine::component::AnimationFrame{ engine::utils::Rect{ 0.0F, 0.0F, 192.0F, 192.0F },
                                             100 },
          engine::component::AnimationFrame{ engine::utils::Rect{ 192.0F, 0.0F, 192.0F, 192.0F },
                                             100 },
          engine::component::AnimationFrame{ engine::utils::Rect{ 384.0F, 0.0F, 192.0F, 192.0F },
                                             100 },
          engine::component::AnimationFrame{ engine::utils::Rect{ 576.0F, 0.0F, 192.0F, 192.0F },
                                             100 },
          engine::component::AnimationFrame{ engine::utils::Rect{ 768.0F, 0.0F, 192.0F, 192.0F },
                                             100 },
          engine::component::AnimationFrame{ engine::utils::Rect{ 960.0F, 0.0F, 192.0F, 192.0F },
                                             100 } }
    };
    auto animationMap = std::unordered_map<entt::id_type, engine::component::Animation>{
        { "idle"_hs, std::move(animation) }
    };
    m_registry.emplace<engine::component::AnimationComponent>(entity,
                                                              std::move(animationMap),
                                                              "idle"_hs);
}

} // namespace game::scene
