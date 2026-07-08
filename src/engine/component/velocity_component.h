#pragma once

#include <glm/vec2.hpp>

namespace engine::component {

/**
 * @brief 速度组件。
 */
struct VelocityComponent
{
    glm::vec2 m_velocity{ 0.0F, 0.0F }; ///< @brief 速度
};

} // namespace engine::component
