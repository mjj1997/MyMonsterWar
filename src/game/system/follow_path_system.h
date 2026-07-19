#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

#include <unordered_map>

namespace game::data {
struct PathNode;
}

namespace game::system {

/**
 * @brief 路径跟随系统, 根据路径节点更新敌人实体的速度和目标路径节点
 */
class FollowPathSystem
{
public:
    void update(entt::registry& registry,
                entt::dispatcher& dispatcher,
                std::unordered_map<int, game::data::PathNode>& pathNodes);
};

} // namespace game::system
