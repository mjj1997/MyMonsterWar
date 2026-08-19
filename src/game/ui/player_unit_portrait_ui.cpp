#include "player_unit_portrait_ui.h"
#include "../data/entity_blueprint.h"
#include "../data/game_stats.h"
#include "../data/session_data.h"
#include "../data/ui_config.h"
#include "../factory/blueprint_manager.h"

#include "../../engine/core/context.h"
#include "../../engine/core/game_state.h"
#include "../../engine/ui/ui_button.h"
#include "../../engine/ui/ui_image.h"
#include "../../engine/ui/ui_label.h"
#include "../../engine/ui/ui_manager.h"
#include "../../engine/ui/ui_panel.h"

#include <entt/core/hashed_string.hpp>
#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::ui {

PlayerUnitPortraitUi::PlayerUnitPortraitUi(entt::registry& registry,
                                           engine::ui::UiManager& uiManager,
                                           engine::core::Context& context)
    : m_registry{ registry }
    , m_uiManager{ uiManager }
    , m_context{ context }
{
    spdlog::trace("PlayerUnitPortraitUi 构造完成");
}

PlayerUnitPortraitUi::~PlayerUnitPortraitUi() = default;

void PlayerUnitPortraitUi::createPortraitUi()
{
    if (!m_uiManager.init(m_context.gameState().logicalSize())) {
        return;
    }

    // 获取 UI 配置数据、会话数据、蓝图管理器等上下文数据
    auto uiConfig = m_registry.ctx().get<std::shared_ptr<game::data::UiConfig>>();
    auto sessionData = m_registry.ctx().get<std::shared_ptr<game::data::SessionData>>();
    auto blueprintManager = m_registry.ctx().get<std::shared_ptr<game::factory::BlueprintManager>>();

    // 获取玩家单位面板间隔、角色数据、角色数量
    float padding{ uiConfig->playerUnitPanelPadding() };
    auto& playerUnits = sessionData->playerUnits();
    size_t unitNum{ playerUnits.size() };

    // --- 在屏幕下方创建一个锚定面板 UI 条，用于显示角色肖像 ---
    // 获取窗口大小和角色肖像框大小
    auto windowsSize = m_context.gameState().logicalSize();
    auto frameSize = uiConfig->playerUnitPanelFrameSize();
    // 根据角色数量、角色肖像框大小、间隔计算锚定面板的位置和大小
    auto pos = glm::vec2{ 0.0F, windowsSize.y - frameSize.y - 2 * padding };
    auto size = glm::vec2{ unitNum * frameSize.x + (unitNum + 1) * padding,
                           frameSize.y + 2 * padding };
    auto anchorPanel = std::make_unique<engine::ui::UiPanel>(pos, size);
    // 设置背景色
    anchorPanel->setBackgroundColor(engine::utils::FColor{ 0.1F, 0.1F, 0.1F, 0.1F });
    // 设置ID，以后即可根据ID找到该面板
    anchorPanel->setId("anchor_panel"_hs);

    // 依次添加角色肖像，每个肖像显示由四部分依次叠加：肖像、肖像框、职业图标、花费，可以通过一个肖像框面板定位（位于上层锚定面板之中）
    int index{ 0 };
    for (auto& [id, playerUnitData] : playerUnits) {
        auto portrait = uiConfig->portrait(id);
        auto frame = uiConfig->portraitFrame(playerUnitData.m_rarity);
        auto icon = uiConfig->icon(playerUnitData.m_classId);
        auto cost = blueprintManager->getPlayerClassBlueprint(playerUnitData.m_classId)
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
                                                  uiConfig->playerUnitPanelFontPath(),
                                                  uiConfig->playerUnitPanelFontSize(),
                                                  engine::utils::FColor::yellow(),
                                                  uiConfig->playerUnitPanelFontOffset()));
        // 最后往肖像框面板中添加一个灰色的遮盖面板，花费不足以支持该角色出击时显示
        auto coverPanel = std::make_unique<engine::ui::UiPanel>(glm::vec2(0.0F), frameSize);
        coverPanel->setBackgroundColor(engine::utils::FColor{ 0.0F, 0.0F, 0.0F, 0.2F });
        coverPanel->setId("cover_panel"_hs);
        framePanel->addChild(std::move(coverPanel));

        // 将肖像框面板添加到锚定面板中，并使用花费作为排序键
        anchorPanel->addChild(std::move(framePanel), cost);
        ++index;
    }

    // 将锚定面板添加到 UI 管理器中
    m_uiManager.addElement(std::move(anchorPanel));

    // 移动以后，想要再次获取锚定面板，需要根据 ID 获取
    m_anchorPanel = static_cast<engine::ui::UiPanel*>(
        m_uiManager.rootElement()->getChildById("anchor_panel"_hs));

    // 对锚定面板中的子元素（肖像框面板）进行排序
    m_anchorPanel->sortChildrenByOrderIndex();
    // 按顺序排列锚定面板中的子元素（肖像框面板）的位置
    arrangePortraitUi();
}

void PlayerUnitPortraitUi::arrangePortraitUi()
{
    // 获取 UI 配置数据
    auto uiConfig = m_registry.ctx().get<std::shared_ptr<game::data::UiConfig>>();
    // 获取玩家单位面板的间隔、大小
    auto padding = uiConfig->playerUnitPanelPadding();
    auto frameSize = uiConfig->playerUnitPanelFrameSize();

    // 遍历锚定面板中的所有子元素，依次设置其位置
    for (size_t i{ 0 }; i < m_anchorPanel->children().size(); ++i) {
        auto& child = m_anchorPanel->children().at(i);
        child->setLocalPosition(glm::vec2{ padding + i * (frameSize.x + padding), padding });
    }

    // 更新锚定面板的大小
    m_anchorPanel->setSize(
        glm::vec2{ padding + m_anchorPanel->children().size() * (frameSize.x + padding),
                   frameSize.y + 2 * padding });
}

void PlayerUnitPortraitUi::updatePortraitCover()
{
    // 获取场景内的游戏统计数据
    auto& gameStats = m_registry.ctx().get<game::data::GameStats&>();

    // 获取锚定面板中的所有子元素（肖像框面板）
    auto& framePanels = m_anchorPanel->children();
    for (auto& framePanel : framePanels) {
        // 获取肖像框面板中的遮盖面板
        auto coverPanel = framePanel->getChildById("cover_panel"_hs);
        // 设置遮盖面板的可见性(肖像框面板的排列顺序值已经设置为出击所需的 cost)
        if (coverPanel != nullptr) {
            coverPanel->setVisible(gameStats.m_costAvailable < framePanel->orderIndex());
        }
    }
}

} // namespace game::ui
