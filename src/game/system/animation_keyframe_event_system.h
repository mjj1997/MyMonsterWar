#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::system {

/**
* @brief 动画关键帧事件系统，用于处理各种动画关键帧事件
*/
class AnimationKeyframeEventSystem
{
public:
    AnimationKeyframeEventSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~AnimationKeyframeEventSystem();

private:
    entt::registry& m_registry;
    entt::dispatcher& m_dispatcher;
};

} // namespace game::system
