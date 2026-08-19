#pragma once

#include <entt/entity/fwd.hpp>

namespace engine::core {
class Context;
}

namespace engine::ui {
class UiPanel;
class UiManager;
} // namespace engine::ui

namespace game::ui {

/**
 * @brief 玩家单位肖像 UI
 * 
 * 负责管理玩家单位肖像 UI 的创建、更新和排列。
 */
class PlayerUnitPortraitUi
{
public:
    /**
     * @brief 构造函数
     * @param registry 注册表
     * @param uiManager UI管理器
     * @param context 引擎上下文
     */
    PlayerUnitPortraitUi(entt::registry& registry,
                         engine::ui::UiManager& uiManager,
                         engine::core::Context& context);
    ~PlayerUnitPortraitUi();

    void update(float deltaTime);

    engine::ui::UiPanel* anchorPanel() const { return m_anchorPanel; }

private:
    ///< @brief 创建位于画面下方的玩家单位肖像 UI
    void createPortraitUi();
    ///< @brief 排列位于画面下方的玩家单位肖像UI (肖像增/减时调用)
    void arrangePortraitUi();
    ///< @brief 更新肖像遮盖
    void updatePortraitCover();

    // --- 构造函数传入的外部组件引用 ---
    entt::registry& m_registry;
    engine::ui::UiManager& m_uiManager;
    engine::core::Context& m_context;

    ///< @brief 保存玩家单位肖像 UI 的根面板(非拥有指针)，方便使用
    engine::ui::UiPanel* m_anchorPanel;
};

} // namespace game::ui
