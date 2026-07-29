#pragma once

#include <entt/entity/fwd.hpp>

namespace game::system {

/**
 * @brief 计时器系统，用于更新所有包含计时器的组件，
 * 并在满足条件时添加必要的标签（如攻击冷却完成后，添加攻击就绪标签）
 * 
 */
class TimerSystem
{
public:
    void update(entt::registry& registry, float deltaTime);
};

} // namespace game::system
