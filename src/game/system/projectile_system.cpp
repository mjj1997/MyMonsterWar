#include "projectile_system.h"

namespace game::system {

ProjectileSystem::ProjectileSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{}

ProjectileSystem::~ProjectileSystem() {}

} // namespace game::system
