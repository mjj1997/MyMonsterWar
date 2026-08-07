#include "effect_system.h"
#include "../defs/events.h"
#include "../factory/entity_factory.h"

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

void EffectSystem::handleEnemyDeadEffectEvent(const game::defs::EnemyDeadEffectEvent& event)
{
    m_entityFactory.createEnemyDeadEffect(event.m_classId, event.m_position, event.m_isFlipped);
}

} // namespace game::system
