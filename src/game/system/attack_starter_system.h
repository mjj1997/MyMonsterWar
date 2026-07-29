#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::system {

/**
 * @brief 攻击发起者系统，用于启动角色的攻击动作
 */
class AttackStarterSystem
{
public:
    void update(entt::registry& registry, entt::dispatcher& dispatcher);

private:
    /* 拆分逻辑的函数，在 update 中调用 */
    /// @brief 处理被阻挡的敌人角色
    void updateEnemyBlocked(entt::registry& registry, entt::dispatcher& dispatcher);
};

} // namespace game::system
