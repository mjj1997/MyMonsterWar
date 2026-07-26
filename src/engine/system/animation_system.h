#pragma once

#include <entt/entity/fwd.hpp>
#include <entt/signal/fwd.hpp>

namespace engine::utils {
struct PlayAnimationEvent;
}

namespace engine::system {

/**
 * @brief 动画系统
 * 
 * 负责更新实体的动画组件，并同步到精灵组件。
 */
class AnimationSystem
{
public:
    AnimationSystem(entt::registry& registry, entt::dispatcher& dispatcher);
    ~AnimationSystem();

    /**
     * @brief 更新所有拥有动画组件和精灵组件的实体
     * @param registry entt::registry 的引用
     * @param deltaTime 增量时间
     */
    void update(entt::registry& registry, float deltaTime);

private:
    /* --- 回调函数 --- */
    /// @brief 播放动画事件的处理函数
    void playAnimation(const engine::utils::PlayAnimationEvent& event);

    /* --- 将依赖保存为成员变量，方便在回调函数中使用 --- */
    entt::registry& m_registry;
    entt::dispatcher& m_dispatcher;
};

} // namespace engine::system
