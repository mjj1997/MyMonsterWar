#include "movement_system.h"
#include "../component/transform_component.h"
#include "../component/velocity_component.h"

#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

namespace engine::system {

void MovementSystem::update(entt::registry& registry, float deltaTime)
{
    spdlog::trace("MovementSystem::update");

    // 获取感兴趣的实体 view
    auto view = registry.view<engine::component::VelocityComponent,
                              engine::component::TransformComponent>();

    // 遍历实体，获取组件并执行相关逻辑
    for (auto entity : view) {
        const auto& velocityComponent = view.get<engine::component::VelocityComponent>(entity);
        auto& transformComponent = view.get<engine::component::TransformComponent>(entity);

        // 更新位置
        transformComponent.m_position += velocityComponent.m_velocity * deltaTime;
    }
}

} // namespace engine::system
