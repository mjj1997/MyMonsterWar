#pragma once

#include "../../engine/utils/events.h"

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
    /* --- 回调函数 --- */
    /// @brief 处理动画关键帧事件的函数
    void handleKeyframeEvent(const engine::utils::AnimationKeyframeEvent& event);

    /* 根据不同的事件 ID，拆分出的处理函数 */
    /// @brief 处理击中事件的函数
    void handleHitEvent(const engine::utils::AnimationKeyframeEvent& event);

    entt::registry& m_registry;
    entt::dispatcher& m_dispatcher;
};

} // namespace game::system
