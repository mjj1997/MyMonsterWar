#include "game_rule_system.h"
#include "../data/game_stats.h"

#include <entt/entity/registry.hpp>

namespace game::system {

GameRuleSystem::GameRuleSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{}

GameRuleSystem::~GameRuleSystem() {}

void GameRuleSystem::update(float deltaTime)
{
    // 更新可用 cost
    auto& gameStats = m_registry.ctx().get<game::data::GameStats&>();
    gameStats.m_costAvailable += gameStats.m_costGenPerSecond * deltaTime;

    // TODO: 可能的 buff 效果
}

} // namespace game::system
