#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::system {

/**
 * @brief 游戏规则系统
 * 
 * 负责处理游戏规则，如 cost 更新、敌人到达基地等。
 */
class GameRuleSystem
{
public:
    GameRuleSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~GameRuleSystem();

private:
    entt::registry& m_registry;
    entt::dispatcher& m_dispatcher;
};

} // namespace game::system
