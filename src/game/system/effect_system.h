#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::factory {
class EntityFactory;
}

namespace game::defs {
struct EnemyDeadEffectEvent;
}

namespace game::system {

/**
 * @brief 特效系统，处理所有特效的创建
 */
class EffectSystem
{
public:
    EffectSystem(entt::registry& registry,
                 entt::dispatcher& dispatcher,
                 game::factory::EntityFactory& entityFactory);
    ~EffectSystem();

private:
    /* --- 回调函数 --- */
    ///< @brief 处理敌人死亡特效事件的函数
    void handleEnemyDeadEffectEvent(const game::defs::EnemyDeadEffectEvent& event);

    // TODO: 未来添加其他特效事件回调函数

    entt::registry& m_registry;
    entt::dispatcher& m_dispatcher;
    game::factory::EntityFactory& m_entityFactory;
};

} // namespace game::system
