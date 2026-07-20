#include "follow_path_system.h"
#include "../component/enemy_component.h"
#include "../data/path_node.h"
#include "../defs/events.h"
#include "../defs/tags.h"

#include "../../engine/component/transform_component.h"
#include "../../engine/component/velocity_component.h"
#include "../../engine/utils/math.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <glm/geometric.hpp>
#include <spdlog/spdlog.h>

namespace game::system {

void FollowPathSystem::update(entt::registry& registry,
                              entt::dispatcher& dispatcher,
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

        // 如果距离小于阈值, 则切换到下一个节点(阈值不要太小,不然敌人速度快的话可能造成震荡)
        if (glm::length(direction) < 5.0F) {
            // 如果下一个节点 ID 列表为空, 代表到达终点, 则发送信号并添加删除标签
            auto size = targetPathNode.m_nextNodeIds.size();
            if (size == 0) {
                spdlog::info("到达终点");

                // 发送敌人到达基地的信号并添加删除标签
                dispatcher.enqueue<game::defs::EnemyArriveBaseEvent>(); // 具体做什么, 由回调函数处理
                registry.emplace<game::defs::DeadTag>(entity);          // 用于延时删除

                continue;
            }

            // 随机选择下一个节点
            auto targetIndex = engine::utils::randomInt(0, size - 1);
            enemyComponent.m_targetPathNodeId = targetPathNode.m_nextNodeIds.at(targetIndex);

            // 更新目标节点与方向矢量
            targetPathNode = pathNodes.at(enemyComponent.m_targetPathNodeId);
            direction = targetPathNode.m_position - transformComponent.m_position;
        }

        // 更新速度组件
        velocityComponent.m_velocity = glm::normalize(direction) * enemyComponent.m_speed;
    }
}

} // namespace game::system
