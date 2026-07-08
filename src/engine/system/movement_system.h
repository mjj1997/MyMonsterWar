#pragma once

#include <entt/entity/fwd.hpp>

namespace engine::system {

/**
 * @brief 移动系统
 * 
 * 负责更新实体的移动组件，并同步到变换组件。
 */
class MovementSystem
{
public:
    /**
     * @brief 更新所有拥有移动和变换组件的实体
     * @param registry entt::registry 的引用
     * @param deltaTime 增量时间
     */
    void update(entt::registry& registry, float deltaTime);
};

} // namespace engine::system
