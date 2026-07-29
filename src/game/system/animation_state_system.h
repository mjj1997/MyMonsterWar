#pragma once

#include "../../engine/utils/events.h"

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace game::system {

/**
 * @brief 动画状态系统，用于处理动画播放完成后的切换
 */
class AnimationStateSystem
{
public:
    AnimationStateSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~AnimationStateSystem();

    /* 系统可以没有更新方法，只专门处理事件回调 */

private:
    /* --- 回调函数 --- */
    /// @brief 完成动画播放事件的处理函数
    void finishAnimation(const engine::utils::FinishAnimationEvent& event);

    /* --- 将依赖保存为成员变量，方便在回调函数中使用 --- */
    entt::registry& m_registry;
    entt::dispatcher& m_dispatcher;
};

} // namespace game::system
