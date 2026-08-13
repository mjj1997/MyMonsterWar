#include "game_scene.h"
#include "../component/player_component.h"
#include "../component/stats_component.h"
#include "../data/entity_blueprint.h"
#include "../data/session_data.h"
#include "../data/ui_config.h"
#include "../defs/tags.h"
#include "../factory/blueprint_manager.h"
#include "../factory/entity_factory.h"
#include "../loader/entity_builder_mw.h"
#include "../system/animation_keyframe_event_system.h"
#include "../system/animation_state_system.h"
#include "../system/attack_starter_system.h"
#include "../system/block_system.h"
#include "../system/combat_resolve_system.h"
#include "../system/effect_system.h"
#include "../system/follow_path_system.h"
#include "../system/health_bar_system.h"
#include "../system/orientation_system.h"
#include "../system/projectile_system.h"
#include "../system/remove_dead_system.h"
#include "../system/set_target_system.h"
#include "../system/timer_system.h"

#include "../../engine/component/render_component.h"
#include "../../engine/component/sprite_component.h"
#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"
#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/input/input_manager.h"
#include "../../engine/loader/level_loader.h"
#include "../../engine/system/animation_system.h"
#include "../../engine/system/audio_system.h"
#include "../../engine/system/movement_system.h"
#include "../../engine/system/render_system.h"
#include "../../engine/system/y_sort_system.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"

#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::scene {

GameScene::GameScene(engine::core::Context& context)
    : SceneBase{ "GameScene", context }
{
    spdlog::info("GameScene 构造完成");
}

GameScene::~GameScene() = default;

void GameScene::init()
{
    if (!initSessionData()) {
        spdlog::error("初始化会话数据失败");
        return;
    }
    if (!initUiConfig()) {
        spdlog::error("初始化 UI 配置数据失败");
        return;
    }
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
    if (!initSystems()) { // 需要在可能的依赖模块（如实体工厂）初始化完成后再调用
        spdlog::error("初始化系统失败");
        return;
    }

    testSessionData();
    createTestEnemy();
    createPlayerUnitPortraitUi();

    SceneBase::init();
}

void GameScene::update(float deltaTime)
{
    auto& dispatcher = m_context.dispatcher();

    // 每一帧最先清理死亡实体(要在 dispatcher 处理完事件后再清理, 因此放在下一帧开头)
    m_removeDeadSystem->update(m_registry);

    /* 注意系统更新顺序 */
    m_timerSystem->update(m_registry, deltaTime);
    // 注意阻挡要发生在移动之前，否则敌人本帧会先移动一段距离再被停下，表现会“滑一下”
    m_blockSystem->update(m_registry, dispatcher);
    m_setTargetSystem->update(m_registry);
    // 路径跟随系统要放到移动系统之前
    m_followPathSystem->update(m_registry, dispatcher, m_pathNodes);
    // 朝向系统要放到阻挡、设置目标、路径跟随系统之后
    m_orientationSystem->update(m_registry);
    m_attackStarterSystem->update(m_registry, dispatcher);
    m_projectileSystem->update(deltaTime);
    m_movementSystem->update(m_registry, deltaTime);
    m_ySortSystem->update(m_registry); // 调用顺序要放到移动系统之后
    m_animationSystem->update(deltaTime);

    SceneBase::update(deltaTime);
}

void GameScene::render()
{
    auto& renderer = m_context.renderer();
    auto& camera = m_context.camera();

    // 注意渲染顺序，保证血量条遮盖角色
    m_renderSystem->update(m_registry, m_context.renderer(), m_context.camera());
    m_healthBarSystem->update(m_registry, renderer, camera);

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
    inputManager.actionSink("move_left"_hs).disconnect<&GameScene::createTestPlayerHealer>(this);
    inputManager.actionSink("pause"_hs).disconnect<&GameScene::clearAllPlayers>(this);

    SceneBase::clean();
}

bool GameScene::initSessionData()
{
    if (m_sessionData == nullptr) {
        m_sessionData = std::make_shared<game::data::SessionData>();
        if (!m_sessionData->loadDefaultData()) {
            spdlog::error("初始化会话数据失败");
            return false;
        }
    }

    m_level = m_sessionData->level();
    return true;
}

bool GameScene::initUiConfig()
{
    if (m_uiConfig == nullptr) {
        m_uiConfig = std::make_shared<game::data::UiConfig>();
        if (!m_uiConfig->loadFromFile("assets/data/ui_config.json")) {
            spdlog::error("加载 UI 配置数据失败");
            return false;
        }
    }

    return true;
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
    inputManager.actionSink("move_left"_hs).connect<&GameScene::createTestPlayerHealer>(this);
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
            || !m_blueprintManager->loadPlayerClassBlueprints("assets/data/player_data.json")
            || !m_blueprintManager->loadProjectileBlueprints("assets/data/projectile_data.json")) {
            spdlog::error("加载蓝图失败");
            return false;
        }
    }

    m_entityFactory = std::make_unique<game::factory::EntityFactory>(m_registry,
                                                                     *m_blueprintManager);
    spdlog::info("实体工厂初始化完成");
    return true;
}

bool GameScene::initSystems()
{
    auto& dispatcher = m_context.dispatcher();

    // 初始化系统
    m_movementSystem = std::make_unique<engine::system::MovementSystem>();
    m_renderSystem = std::make_unique<engine::system::RenderSystem>();
    m_animationSystem = std::make_unique<engine::system::AnimationSystem>(m_registry, dispatcher);
    m_ySortSystem = std::make_unique<engine::system::YSortSystem>();
    m_audioSystem = std::make_unique<engine::system::AudioSystem>(m_registry, m_context);

    m_followPathSystem = std::make_unique<game::system::FollowPathSystem>();
    m_removeDeadSystem = std::make_unique<game::system::RemoveDeadSystem>();
    m_blockSystem = std::make_unique<game::system::BlockSystem>();
    m_setTargetSystem = std::make_unique<game::system::SetTargetSystem>();
    m_timerSystem = std::make_unique<game::system::TimerSystem>();
    m_attackStarterSystem = std::make_unique<game::system::AttackStarterSystem>();
    m_animationStateSystem = std::make_unique<game::system::AnimationStateSystem>(m_registry,
                                                                                  dispatcher);
    m_orientationSystem = std::make_unique<game::system::OrientationSystem>();
    m_animationKeyframeEventSystem
        = std::make_unique<game::system::AnimationKeyframeEventSystem>(m_registry, dispatcher);
    m_combatResolveSystem = std::make_unique<game::system::CombatResolveSystem>(m_registry,
                                                                                dispatcher);
    m_projectileSystem = std::make_unique<game::system::ProjectileSystem>(m_registry,
                                                                          dispatcher,
                                                                          *m_entityFactory);
    m_healthBarSystem = std::make_unique<game::system::HealthBarSystem>();
    m_effectSystem = std::make_unique<game::system::EffectSystem>(m_registry,
                                                                  dispatcher,
                                                                  *m_entityFactory);

    spdlog::info("系统初始化完成");
    return true;
}

void GameScene::createPlayerUnitPortraitUi()
{
    if (!m_uiManager->init(m_context.gameState().logicalSize())) {
        return;
    }

    float padding{ m_uiConfig->playerUnitPanelPadding() };
    auto& playerUnits = m_sessionData->playerUnits();
    size_t unitNum{ playerUnits.size() };

    // --- 在屏幕下方创建一个锚定面板 UI 条，用于显示角色肖像 ---
    // 获取窗口大小和角色肖像框大小
    auto windowsSize = m_context.gameState().logicalSize();
    auto frameSize = m_uiConfig->playerUnitPanelFrameSize();
    // 根据角色数量、角色肖像框大小、间隔计算锚定面板的位置和大小
    auto pos = glm::vec2{ 0.0F, windowsSize.y - frameSize.y - 2 * padding };
    auto size = glm::vec2{ unitNum * frameSize.x + (unitNum + 1) * padding,
                           frameSize.y + 2 * padding };
    auto anchorPanel = std::make_unique<engine::ui::UiPanel>(pos, size);
    // 设置背景色
    anchorPanel->setBackgroundColor(engine::utils::FColor{ 0.1F, 0.1F, 0.1F, 0.1F });
    // 设置ID，以后即可根据ID找到该面板
    anchorPanel->setId("unit_panel"_hs);

    // 依次添加角色肖像，每个肖像显示由四部分依次叠加：肖像、肖像框、职业图标、花费，可以通过一个肖像框面板定位（位于上层锚定面板之中）
    int index{ 0 };
    for (auto& [id, playerUnitData] : playerUnits) {
        auto portrait = m_uiConfig->portrait(id);
        auto frame = m_uiConfig->portraitFrame(playerUnitData.m_rarity);
        auto icon = m_uiConfig->icon(playerUnitData.m_classId);
        auto cost = m_blueprintManager->getPlayerClassBlueprint(playerUnitData.m_classId)
                        .m_player.m_cost;
        // 只有稀有度对cost有影响
        cost = static_cast<int>(
            std::round(engine::utils::statModify(cost, 1, playerUnitData.m_rarity)));

        // 创建每个肖像的肖像框面板
        auto framePos = glm::vec2{ padding + index * (frameSize.x + padding), padding };
        auto framePanel = std::make_unique<engine::ui::UiPanel>(framePos, frameSize);
        framePanel->setId(id);

        // 依次往肖像框面板中添加四个元素，为了能够交互，将肖像框设置为按钮，并绑定点击事件
        framePanel->addChild(
            std::make_unique<engine::ui::UiImage>(portrait, glm::vec2(0.0F), frameSize));
        framePanel->addChild(std::make_unique<engine::ui::UiButton>(m_context,
                                                                    frame,
                                                                    frame,
                                                                    frame,
                                                                    glm::vec2(0.0F),
                                                                    frameSize
                                                                    // TODO: 添加点击事件回调函数
                                                                    ));
        framePanel->addChild(
            std::make_unique<engine::ui::UiImage>(icon, glm::vec2(0.0F), frameSize / 2.0F));
        framePanel->addChild(
            std::make_unique<engine::ui::UiLabel>(m_context.textRenderer(),
                                                  std::to_string(cost),
                                                  m_uiConfig->playerUnitPanelFontPath(),
                                                  m_uiConfig->playerUnitPanelFontSize(),
                                                  engine::utils::FColor::yellow(),
                                                  m_uiConfig->playerUnitPanelFontOffset()));
        // 最后往肖像框面板中添加一个灰色的遮盖面板，花费不足以支持该角色出击时显示
        auto coverPanel = std::make_unique<engine::ui::UiPanel>(glm::vec2(0.0F), frameSize);
        coverPanel->setBackgroundColor(engine::utils::FColor{ 0.0F, 0.0F, 0.0F, 0.2F });
        coverPanel->setId("cover_panel"_hs);
        framePanel->addChild(std::move(coverPanel));

        // 将肖像框面板添加到锚定面板中，并使用花费作为排序键
        anchorPanel->addChild(std::move(framePanel), cost);
        ++index;
    }

    // 对锚定面板中的子元素（肖像框面板）进行排序
    anchorPanel->sortChildrenByOrderIndex();
    // 按顺序排列锚定面板中的子元素（肖像框面板）的位置
    arrangePlayerUnitPortraitUi(anchorPanel.get(), frameSize, padding);

    m_uiManager->addElement(std::move(anchorPanel));
}

void GameScene::arrangePlayerUnitPortraitUi(engine::ui::UiElementBase* anchorPanel,
                                            glm::vec2 frameSize,
                                            float padding)
{
    // 遍历锚定面板中的所有子元素，依次设置其位置
    for (size_t i{ 0 }; i < anchorPanel->children().size(); ++i) {
        auto& child = anchorPanel->children().at(i);
        child->setLocalPosition(glm::vec2{ padding + i * (frameSize.x + padding), padding });
    }

    // 更新锚定面板的大小
    anchorPanel->setSize(
        glm::vec2{ padding + anchorPanel->children().size() * (frameSize.x + padding),
                   frameSize.y + 2 * padding });
}

void GameScene::onEnemyArriveBase(const game::defs::EnemyArriveBaseEvent& event)
{
    spdlog::info("敌人到达基地");
    // TODO: 处理敌人到达基地的逻辑
}

void GameScene::testSessionData()
{
    spdlog::info("--- 测试会话数据 ---");
    spdlog::info("当前关卡号: {}", m_level);
    spdlog::info("当前得分: {}", m_sessionData->score());
    spdlog::info("是否通关: {}", m_sessionData->isLevelClear());
    for (const auto& playerUnit : m_sessionData->playerUnits()) {
        spdlog::info("玩家角色名: {}, 职业: {}, 等级: {}, 稀有度: {}",
                     playerUnit.second.m_name,
                     playerUnit.second.m_className,
                     playerUnit.second.m_lv,
                     playerUnit.second.m_rarity);
    }
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
    auto entity = m_entityFactory->createPlayerUnit("warrior"_hs, position);
    // 让玩家处于受伤状态（治疗师不会锁定满血目标）
    m_registry.emplace<game::defs::InjuredTag>(entity);
    auto& stats = m_registry.get<game::component::StatsComponent>(entity);
    stats.m_hp = stats.m_maxHp / 2;
    spdlog::info("创建战士: 位置: {}, {}", position.x, position.y);
    return true;
}

bool GameScene::createTestPlayerRanged()
{
    auto position = m_context.inputManager().logicalMousePosition();
    auto entity = m_entityFactory->createPlayerUnit("archer"_hs, position);
    // 让玩家处于受伤状态（治疗师不会锁定满血目标）
    m_registry.emplace<game::defs::InjuredTag>(entity);
    auto& stats = m_registry.get<game::component::StatsComponent>(entity);
    stats.m_hp = stats.m_maxHp / 2;
    spdlog::info("创建弓箭手: 位置: {}, {}", position.x, position.y);
    return true;
}

bool GameScene::createTestPlayerHealer()
{
    auto position = m_context.inputManager().logicalMousePosition();
    m_entityFactory->createPlayerUnit("witch"_hs, position);
    spdlog::info("创建治疗师: 位置: {}, {}", position.x, position.y);
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
