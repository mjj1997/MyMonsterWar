#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::factory {
class EntityFactory;
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
    entt::registry& m_registry;
    entt::dispatcher& m_dispatcher;
    game::factory::EntityFactory& m_entityFactory;
};

} // namespace game::system
