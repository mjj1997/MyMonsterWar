#pragma once

#include <entt/entity/entity.hpp>

namespace game::component {

/**
 * @brief 目标组件，包含目标实体，表示当前锁定的攻击对象
 */
struct TargetComponent
{
    entt::entity m_entity{ entt::null };
};

} // namespace game::component
