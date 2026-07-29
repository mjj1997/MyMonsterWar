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
};

} // namespace game::system
