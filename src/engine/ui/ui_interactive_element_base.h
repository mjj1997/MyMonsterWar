#pragma once

#include "../render/image.h" // 需要引入头文件而不是前置声明（map容器创建时可能会检查内部元素是否有析构定义）
#include "state/ui_state_base.h"
#include "ui_element_base.h"

#include <string_view>
#include <unordered_map>

namespace engine::ui {

/**
 * @brief 可交互 UI 元素的基类，继承自 UiElementBase
 *
 * @note 定义了可交互 UI 元素的通用属性和行为。
 * @note 管理 UI 元素状态的切换和交互逻辑。
 * @note 提供事件处理、更新和渲染的虚方法。
 */
class UiInteractiveElementBase : public UiElementBase
{
public:
    /**
     * @brief 构造 UiInteractiveElementBase
     * @param context 引擎上下文
     * @param localPosition 初始局部位置
     * @param size 初始大小
     */
    explicit UiInteractiveElementBase(engine::core::Context& context,
                                      glm::vec2 localPosition = { 0.0F, 0.0F },
                                      glm::vec2 size = { 0.0F, 0.0F });

    ~UiInteractiveElementBase() override = default;

    // --- 禁用拷贝和移动语义 ---
    UiInteractiveElementBase(const UiInteractiveElementBase&) = delete;
    UiInteractiveElementBase& operator=(const UiInteractiveElementBase&) = delete;
    UiInteractiveElementBase(UiInteractiveElementBase&&) = delete;
    UiInteractiveElementBase& operator=(UiInteractiveElementBase&&) = delete;

    // --- 事件处理方法 ---
    virtual void clicked() {} ///< @brief 如果有点击事件，则重写该方法

    // --- 核心方法 ---
    bool handleInput(engine::core::Context& context) override;
    void render(engine::core::Context& context) override;

    ///< @brief 添加状态名称-图片对
    void addImage(entt::id_type nameId, engine::render::Image image);
    ///< @brief 通过状态名称 ID，设置当前显示的图片
    void setImage(entt::id_type nameId);
    ///< @brief 添加状态名称-音效对
    void addSound(entt::id_type nameId, entt::hashed_string hashedPath);
    ///< @brief 通过状态名称 ID，播放音效
    void playSound(entt::id_type nameId);

    // --- Getters and Setters ---
    ///< @brief 设置当前状态
    void setCurrentState(std::unique_ptr<engine::ui::state::UiStateBase> state);
    ///< @brief 获取当前状态
    engine::ui::state::UiStateBase* currentState() const { return m_currentState.get(); }

    ///< @brief 设置是否可交互
    void setInteractive(bool interactive) { m_isInteractive = interactive; }
    bool isInteractive() const { return m_isInteractive; } ///< @brief 获取是否可交互

protected:
    engine::core::Context& m_context; ///< @brief 可交互 UI 元素很可能需要其他引擎组件

    ///< @brief 状态和图片的映射，key 为状态名称 ID，value 为图片
    std::unordered_map<entt::id_type, engine::render::Image> m_images;
    ///< @brief 状态和音效的映射，key 为状态名称 ID，value 为音效文件 ID
    std::unordered_map<entt::id_type, entt::id_type> m_sounds;

    std::unique_ptr<engine::ui::state::UiStateBase> m_currentState; ///< @brief 当前状态
    entt::id_type m_currentImageId{ entt::null };                   ///< @brief 当前显示的图片 ID
    bool m_isInteractive{ true };                                   ///< @brief 是否可交互
};

} // namespace engine::ui
