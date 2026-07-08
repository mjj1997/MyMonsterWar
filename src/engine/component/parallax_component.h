#pragma once

#include <glm/vec2.hpp>

namespace engine::component {

/**
 * @brief 视差组件
 * 
 * 包含滚动速度因子、是否重复和是否隐藏。（需要和 Sprite 配合使用）
 */
struct ParallaxComponent
{
    /**
     * @brief 构造函数
     * @param scrollFactor 控制背景相对于相机移动速度的因子。
     * @param repeat 是否重复。
     * @param isHidden 是否隐藏。
     */
    explicit ParallaxComponent(glm::vec2 scrollFactor,
                               glm::bvec2 repeat = glm::bvec2(true),
                               bool isHidden = false)
        : m_scrollFactor{ scrollFactor }
        , m_repeat{ repeat }
        , m_isHidden{ isHidden }
    {}

    ///< @brief 滚动速度因子 (0=静止, 1=随相机移动, <1=比相机慢)
    glm::vec2 m_scrollFactor{ 0.0F, 0.0F };
    glm::bvec2 m_repeat{ true, true }; ///< @brief 是否沿着X和Y轴周期性重复
    bool m_isHidden{ false };          ///< @brief 是否隐藏（不渲染）
};

} // namespace engine::component
