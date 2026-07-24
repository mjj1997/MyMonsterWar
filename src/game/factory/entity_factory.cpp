#include "entity_factory.h"

namespace game::factory {

EntityFactory::EntityFactory(entt::registry& registry, BlueprintManager& blueprintManager)
    : m_registry{ registry }
    , m_blueprintManager{ blueprintManager }
{}

} // namespace game::factory
