#include "animation_keyframe_event_system.h"
#include "../component/blocked_by_component.h"
#include "../component/enemy_component.h"
#include "../component/player_component.h"
#include "../component/projectile_component.h"
#include "../component/stats_component.h"
#include "../component/target_component.h"
#include "../defs/events.h"
#include "../defs/tags.h"

#include "../../engine/component/transform_component.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>

using namespace entt::literals;

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
    if (event.m_eventId == "hit"_hs) {
        handleHitEvent(event);
    }
    // TODO: 其他事件 ID 的处理函数
}

void AnimationKeyframeEventSystem::handleHitEvent(const engine::utils::AnimationKeyframeEvent& event)
{
    /* 玩家击中事件：攻击或治疗当前目标 */
    if (m_registry.all_of<game::component::PlayerComponent>(event.m_entity)) {
        // 击中时，可能目标已经解除锁定，因此需要检查
        if (auto* targetComponent = m_registry.try_get<game::component::TargetComponent>(
                event.m_entity);
            targetComponent != nullptr) {
            const auto& statsComponent = m_registry.get<game::component::StatsComponent>(
                event.m_entity);
            // 根据玩家的类型，发送攻击或治疗事件
            if (m_registry.all_of<game::defs::HealerTag>(event.m_entity)) {
                m_dispatcher.enqueue(game::defs::HealEvent{
                    event.m_entity, targetComponent->m_entity, statsComponent.m_atk });
            } else {
                m_dispatcher.enqueue(game::defs::AttackEvent{
                    event.m_entity, targetComponent->m_entity, statsComponent.m_atk });
            }
            // 发送播放“hit”音效事件
            m_dispatcher.enqueue(engine::utils::PlaySoundEvent{ event.m_entity, "hit"_hs });
        }
        return;
    }

    /* 敌人击中事件：对阻挡者造成伤害 */
    if (m_registry.all_of<game::component::EnemyComponent>(event.m_entity)) {
        // 击中时，可能阻挡者已经解除锁定，因此需要检查
        if (auto* blockedByComponent = m_registry.try_get<game::component::BlockedByComponent>(
                event.m_entity);
            blockedByComponent != nullptr) {
            const auto& statsComponent = m_registry.get<game::component::StatsComponent>(
                event.m_entity);
            // 发送攻击事件
            m_dispatcher.enqueue(game::defs::AttackEvent{
                event.m_entity, blockedByComponent->m_entity, statsComponent.m_atk });
        }
        // NOTE: 只有远程敌人才有目标组件，但远程攻击动画事件 ID 为 “emit”，不在这里处理
        // NOTE: 敌人击中事件不播放音效，未来可按需添加
        return;
    }
}

void AnimationKeyframeEventSystem::handleEmitEvent(const engine::utils::AnimationKeyframeEvent& event)
{
    /* 发射事件：从角色身上找到拥有的投射物 ID，并执行发射投射物事件 */
    // 确认角色实体有效
    if (!m_registry.valid(event.m_entity)) {
        return;
    }

    // 确认角色的目标组件依然存在，且目标组件指向的实体也有效
    auto target = m_registry.try_get<game::component::TargetComponent>(event.m_entity);
    if (target == nullptr || !m_registry.valid(target->m_entity)) {
        return;
    }

    // 获取角色的变换组件、属性组件、投射物 ID 组件
    const auto [transform,
                stats,
                projectileId] = m_registry.get<engine::component::TransformComponent,
                                               game::component::StatsComponent,
                                               game::component::ProjectileIdComponent>(
        event.m_entity);
    // 获取目标实体的变换组件
    const auto targetTransform = m_registry.get<engine::component::TransformComponent>(
        target->m_entity);

    // 发送发射投射物事件
    m_dispatcher.enqueue(
        game::defs::EmitProjectileEvent{ .m_id = projectileId.m_projectileId,
                                         .m_target = target->m_entity,
                                         .m_startPosition = transform.m_position,
                                         .m_targetPosition = targetTransform.m_position,
                                         .m_damage = stats.m_atk });

    // 发送播放“emit”音效事件
    m_dispatcher.enqueue(engine::utils::PlaySoundEvent{ event.m_entity, "emit"_hs });
}

} // namespace game::system
