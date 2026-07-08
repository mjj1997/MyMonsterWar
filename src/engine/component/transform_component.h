#pragma once

#include <glm/vec2.hpp>

namespace engine::component {

/**
 * @brief 变换组件，包含位置、缩放和旋转。
 */
struct TransformComponent
{
    /**
     * @brief 构造函数
     * @param position 位置
     * @param scale 缩放，默认值为 (1.0F, 1.0F)
     * @param rotation 旋转角度，默认值为 0
     */
    explicit TransformComponent(glm::vec2 position,
                                glm::vec2 scale = glm::vec2(1.0F),
                                float rotation = 0.0F)
        : m_position{ position }
        , m_scale{ scale }
        , m_rotation{ rotation }
    {}

    glm::vec2 m_position{ 0.0F, 0.0F }; ///< @brief 位置
    glm::vec2 m_scale{ 1.0F, 1.0F };    ///< @brief 缩放
    float m_rotation{ 0.0F };           ///< @brief 旋转（度）
};

} // namespace engine::component
