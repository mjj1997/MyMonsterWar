#pragma once

#include <entt/entity/fwd.hpp>

namespace engine::system {

/**
 * @brief 动画系统
 * 
 * 负责更新实体的动画组件，并同步到精灵组件。
 */
class AnimationSystem
{
public:
    /**
     * @brief 更新所有拥有动画组件和精灵组件的实体
     * @param registry entt::registry 的引用
     * @param deltaTime 增量时间
     */
    void update(entt::registry& registry, float deltaTime);
};

} // namespace engine::system
