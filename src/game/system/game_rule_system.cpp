#include "game_rule_system.h"

#include <entt/entity/registry.hpp>

namespace game::system {

GameRuleSystem::GameRuleSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{}

GameRuleSystem::~GameRuleSystem() {}

} // namespace game::system
