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

private:
    /* 拆分逻辑的函数，在 update 中调用 */
    /// @brief 处理攻击计时器
    void updateAttackTimer(entt::registry& registry, float deltaTime);
    // TODO: 处理其它计时器
};

} // namespace game::system
