#include "animation_keyframe_event_system.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

namespace game::system {

AnimationKeyframeEventSystem::AnimationKeyframeEventSystem(entt::registry& registry,
                                                           entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{
    m_dispatcher.sink<engine::utils::AnimationKeyframeEvent>()
        .connect<&AnimationKeyframeEventSystem::handleKeyframeEvent>(this);
}

AnimationKeyframeEventSystem::~AnimationKeyframeEventSystem()
{
    m_dispatcher.disconnect(this);
}

void AnimationKeyframeEventSystem::handleKeyframeEvent(
    const engine::utils::AnimationKeyframeEvent& event)
{
    if (!m_registry.valid(event.m_entity)) {
        return;
    }

    // 根据不同的事件 ID，调用不同的处理函数
}

} // namespace game::system
