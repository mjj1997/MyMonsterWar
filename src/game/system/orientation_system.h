#pragma once

#include <entt/entity/fwd.hpp>

namespace game::system {

/**
* @brief 朝向系统，用于正确处理角色的朝向（面向左或者右）
*/
class OrientationSystem
{
public:
    void update(entt::registry& registry);
};

} // namespace game::system
