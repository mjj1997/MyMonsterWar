#pragma once

#include <entt/entity/fwd.hpp>

namespace engine::core {
class Context;
}

namespace engine::ui {
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

private:
    // --- 构造函数传入的外部组件引用 ---
    entt::registry& m_registry;
    engine::ui::UiManager& m_uiManager;
    engine::core::Context& m_context;
};

} // namespace game::ui
