#include "effect_system.h"
namespace game::system {

EffectSystem::EffectSystem(entt::registry& registry,
                           entt::dispatcher& dispatcher,
                           game::factory::EntityFactory& entityFactory)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
    , m_entityFactory{ entityFactory }
{
}

EffectSystem::~EffectSystem()
{
}

} // namespace game::system
