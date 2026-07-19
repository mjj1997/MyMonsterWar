#include "follow_path_system.h"
#include "../component/enemy_component.h"
#include "../data/path_node.h"

#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"

#include <entt/entity/registry.hpp>
#include <glm/geometric.hpp>
#include <spdlog/spdlog.h>

namespace game::system {

void FollowPathSystem::update(entt::registry& registry,
                              std::unordered_map<int, game::data::PathNode>& pathNodes)
{
    spdlog::trace("FollowPathSystem::update");

    // 筛选依据: 速度组件, 变换组件, 敌人组件
    auto view = registry.view<engine::component::VelocityComponent,
                              engine::component::TransformComponent,
                              game::component::EnemyComponent>();
    for (auto entity : view) {
        auto& velocityComponent = view.get<engine::component::VelocityComponent>(entity);
        auto& transformComponent = view.get<engine::component::TransformComponent>(entity);
        auto& enemyComponent = view.get<game::component::EnemyComponent>(entity);

        // 获取目标路径节点
        auto targetPathNode = pathNodes.at(enemyComponent.m_targetPathNodeId);

        // 计算当前位置到目标位置的向量
        glm::vec2 direction{ targetPathNode.m_position - transformComponent.m_position };

        // 更新速度组件
        velocityComponent.m_velocity = glm::normalize(direction) * enemyComponent.m_speed;
    }
}

} // namespace game::system
