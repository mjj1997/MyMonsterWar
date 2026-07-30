#include "animation_keyframe_event_system.h"

namespace game::system {

AnimationKeyframeEventSystem::AnimationKeyframeEventSystem(entt::registry& registry,
                                                           entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{
}

AnimationKeyframeEventSystem::~AnimationKeyframeEventSystem()
{
}

} // namespace game::system
