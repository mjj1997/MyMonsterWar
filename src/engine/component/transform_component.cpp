#include "transform_component.h"
#include "../object/game_object.h"
#include "sprite_component.h"

namespace engine::component {

void TransformComponent::setScale(glm::vec2 scale)
{
    m_scale = scale;
    if (m_owner != nullptr) {
        // 应用缩放时应同步更新Sprite偏移量
        auto* spriteComponent = m_owner->getComponent<SpriteComponent>();
        if (spriteComponent != nullptr) {
            spriteComponent->updateOffset();
        }
    }
}

} // namespace engine::component
