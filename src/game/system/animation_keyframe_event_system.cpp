#include "animation_keyframe_event_system.h"
#include "../component/blocked_by_component.h"
#include "../component/enemy_component.h"
#include "../component/player_component.h"
#include "../component/target_component.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

namespace game::system {

AnimationKeyframeEventSystem::AnimationKeyframeEventSystem(entt::registry& registry,
                                                           entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{
    m_dispatcher.sink<engine::utils::AnimationKeyframeEvent>()
        .connect<&AnimationKeyframeEventSystem::handleKeyframeEvent>(this);
}

AnimationKeyframeEventSystem::~AnimationKeyframeEventSystem()
{
    m_dispatcher.disconnect(this);
}

void AnimationKeyframeEventSystem::handleKeyframeEvent(
    const engine::utils::AnimationKeyframeEvent& event)
{
    if (!m_registry.valid(event.m_entity)) {
        return;
    }

    // 根据不同的事件 ID，调用不同的处理函数
}

void AnimationKeyframeEventSystem::handleHitEvent(const engine::utils::AnimationKeyframeEvent& event)
{
    /* 玩家击中事件：攻击或治疗当前目标 */
    if (m_registry.all_of<game::component::PlayerComponent>(event.m_entity)) {
        // 击中时，可能目标已经解除锁定，因此需要检查
        if (auto* targetComponent = m_registry.try_get<game::component::TargetComponent>(
                event.m_entity);
            targetComponent != nullptr) {
        }
        return;
    }

    /* 敌人击中事件：对阻挡者造成伤害 */
    if (m_registry.all_of<game::component::EnemyComponent>(event.m_entity)) {
        // 击中时，可能阻挡者已经解除锁定，因此需要检查
        if (auto* blockedByComponent = m_registry.try_get<game::component::BlockedByComponent>(
                event.m_entity);
            blockedByComponent != nullptr) {
        }
        return;
    }
}

} // namespace game::system
