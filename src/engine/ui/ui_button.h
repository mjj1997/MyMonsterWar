#pragma once

#include "ui_interactive_element_base.h"

#include <functional>

namespace engine::ui {

/**
 * @brief 可交互的按钮 UI 元素
 * 
 * 继承自 UiInteractiveElementBase，用于创建可交互的按钮。
 * 支持三种状态：正常、悬停、按下。
 * 支持回调函数，当按钮被点击时调用。
 */
class UiButton final : public UiInteractiveElementBase
{
public:
    /**
     * @brief 构造函数
     * @param context 引擎上下文
     * @param normalImagePath 正常状态的图片路径
     * @param hoverImagePath 悬停状态的图片路径
     * @param pressedImagePath 按下状态的图片路径
     * @param localPosition 局部位置
     * @param size 大小
     * @param callback 回调函数
     */
    UiButton(engine::core::Context& context,
             std::string_view normalImagePath,
             std::string_view hoverImagePath,
             std::string_view pressedImagePath,
             glm::vec2 localPosition = { 0.0F, 0.0F },
             glm::vec2 size = { 0.0F, 0.0F },
             std::function<void()> callback = nullptr);

    ~UiButton() override = default;

    /* --- 重写基类方法 ---*/
    void clicked() override;
    void hoverEntered() override;
    void hoverLeft() override;

    /*  --- getters & setters --- */
    ///< @brief 设置点击回调函数
    void setClickedCallback(std::function<void()> callback)
    {
        m_clickedCallback = std::move(callback);
    }
    ///< @brief 获取点击回调函数
    std::function<void()> clickedCallback() const { return m_clickedCallback; }

    ///< @brief 设置悬停进入回调函数
    void setHoverEnteredCallback(std::function<void()> callback)
    {
        m_hoverEnteredCallback = std::move(callback);
    }
    ///< @brief 获取悬停进入回调函数
    std::function<void()> hoverEnteredCallback() const { return m_hoverEnteredCallback; }

    ///< @brief 设置悬停离开回调函数
    void setHoverLeftCallback(std::function<void()> callback)
    {
        m_hoverLeftCallback = std::move(callback);
    }
    ///< @brief 获取悬停离开回调函数
    std::function<void()> hoverLeftCallback() const { return m_hoverLeftCallback; }

private:
    std::function<void()> m_clickedCallback;      ///< @brief 点击回调函数
    std::function<void()> m_hoverEnteredCallback; ///< @brief 悬停进入回调函数
    std::function<void()> m_hoverLeftCallback;    ///< @brief 悬停离开回调函数
};

} // namespace engine::ui
