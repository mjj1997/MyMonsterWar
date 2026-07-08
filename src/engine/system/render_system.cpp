#include "render_system.h"
#include "../component/sprite_component.h"
#include "../component/transform_component.h"
#include "../render/camera.h"
#include "../render/renderer.h"

#include <entt/entity/registry.hpp>

namespace engine::system {

void RenderSystem::update(entt::registry& registry,
                          render::Renderer& renderer,
                          const render::Camera& camera)
{
    auto view = registry.view<component::TransformComponent, component::SpriteComponent>();
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
