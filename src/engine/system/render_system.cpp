#include "render_system.h"
#include "../component/render_component.h"
#include "../component/sprite_component.h"
#include "../component/transform_component.h"
#include "../render/camera.h"
#include "../render/renderer.h"

#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

namespace engine::system {

void RenderSystem::update(entt::registry& registry,
                          render::Renderer& renderer,
                          const render::Camera& camera)
{
    spdlog::trace("RenderSystem::update");

    // 对渲染组件进行排序 (需要自定义渲染组件的比较运算符)
    registry.sort<component::RenderComponent>(
        [](const auto& lhs, const auto& rhs) { return lhs < rhs; });

    // 执行渲染，注意排序的渲染组件必须放在最前面
    auto view = registry.view<component::RenderComponent,
                              component::TransformComponent,
                              component::SpriteComponent>();
    view.use<engine::component::RenderComponent>(); // 强制使用渲染组件排序
    for (auto entity : view) {
        const auto& transformComponent = view.get<component::TransformComponent>(entity);
        const auto& spriteComponent = view.get<component::SpriteComponent>(entity);

        // 位置 = 变换组件的位置 + 精灵组件的偏移量
        auto position = transformComponent.m_position + spriteComponent.m_offset;
        // 尺寸 = 精灵组件的尺寸 * 变换组件的缩放比例
        auto size = spriteComponent.m_size * transformComponent.m_scale;

        renderer.drawSprite(camera,
                            spriteComponent.m_sprite,
                            position,
                            size,
                            transformComponent.m_rotation);
    }
}

} // namespace engine::system
