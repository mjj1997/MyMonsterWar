#include "game_rule_system.h"
#include "../data/game_stats.h"

#include <entt/entity/registry.hpp>
#include <spdlog/spdlog.h>

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

void GameRuleSystem::handleEnemyArriveBaseEvent(const game::defs::EnemyArriveBaseEvent& event)
{
    spdlog::info("敌人到达基地");
    auto& gameStats = m_registry.ctx().get<game::data::GameStats&>();
    ++gameStats.m_arrivedEnemyCount;
    --gameStats.m_baseHp;
    if (gameStats.m_baseHp <= 0) {
        spdlog::warn("基地被摧毁");
        // TODO: 切换场景
    }
}

} // namespace game::system
