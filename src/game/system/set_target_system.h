#pragma once

#include <entt/entity/fwd.hpp>

namespace game::system {

/**
 * @brief 设置目标系统，用于设置角色的攻击目标
 */
class SetTargetSystem
{
public:
    void update(entt::registry& registry);

private:
    /* 拆分逻辑的函数，在 update 中调用 */
    /// @brief 处理有目标的角色
    void updateEntityWithTarget(entt::registry& registry);
};

} // namespace game::system
