#include "animation_state_system.h"

namespace game::system {

AnimationStateSystem::AnimationStateSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{}

AnimationStateSystem::~AnimationStateSystem() {}

} // namespace game::system
