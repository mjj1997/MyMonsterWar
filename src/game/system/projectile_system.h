#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::defs {
struct EmitProjectileEvent;
}

namespace game::factory {
class EntityFactory;
}

namespace game::system {

/**
     * @brief 投射物系统
     * 1. 响应发射投射物事件，创建投射物
     * 2. 更新投射物飞行状态，并发送攻击事件和播放音效事件
     */
class ProjectileSystem
{
public:
    ProjectileSystem(entt::registry& registry,
                     entt::dispatcher& dispatcher,
                     game::factory::EntityFactory& entityFactory);
    ~ProjectileSystem();

private:
    /* --- 回调函数 --- */
    /// @brief 处理发射投射物事件的函数
    void handleEmitProjectileEvent(const game::defs::EmitProjectileEvent& event);

    entt::registry& m_registry;
    entt::dispatcher& m_dispatcher;
    ///< @brief 需要传入实体工厂引用，负责创建投射物实体
    game::factory::EntityFactory& m_entityFactory;
};

} // namespace game::system
