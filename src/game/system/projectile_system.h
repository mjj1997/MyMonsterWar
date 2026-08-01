#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::system {

/**
     * @brief 投射物系统
     * 1. 响应发射投射物事件，创建投射物
     * 2. 更新投射物飞行状态，并发送攻击事件和播放音效事件
     */
class ProjectileSystem
{
public:
    ProjectileSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~ProjectileSystem();

private:
    entt::registry& m_registry;
    entt::dispatcher& m_dispatcher;
};

} // namespace game::system
