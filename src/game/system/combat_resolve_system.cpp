#include "combat_resolve_system.h"

namespace game::system {

CombatResolveSystem::CombatResolveSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{}

CombatResolveSystem::~CombatResolveSystem() {}

} // namespace game::system
