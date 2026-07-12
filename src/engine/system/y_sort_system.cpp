#include "y_sort_system.h"
#include "../component/render_component.h"
#include "../component/transform_component.h"

#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

namespace engine::system {

void YSortSystem::update(entt::registry& registry)
{
    spdlog::trace("YSortSystem::update");

    // 让渲染组件的深度 = 变换组件的 y 坐标
    auto view = registry.view<component::RenderComponent, const component::TransformComponent>();
    for (auto entity : view) {
        auto& renderComponent = registry.get<component::RenderComponent>(entity);
        const auto& transformComponent = registry.get<const component::TransformComponent>(entity);

        renderComponent.m_depth = transformComponent.m_position.y;
    }
}

} // namespace engine::system
