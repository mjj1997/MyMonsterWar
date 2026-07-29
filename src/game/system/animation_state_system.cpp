#include "animation_state_system.h"
#include "../component/blocked_by_component.h"
#include "../component/enemy_component.h"
#include "../component/player_component.h"
#include "../defs/tags.h"

#include <entt/entity/registry.hpp>
#include <entt/signal/dispatcher.hpp>
#include <spdlog/spdlog.h>

using namespace entt::literals;

namespace game::system {

AnimationStateSystem::AnimationStateSystem(entt::registry& registry, entt::dispatcher& dispatcher)
    : m_registry{ registry }
    , m_dispatcher{ dispatcher }
{
    m_dispatcher.sink<engine::utils::FinishAnimationEvent>()
        .connect<&AnimationStateSystem::finishAnimation>(this);
}

AnimationStateSystem::~AnimationStateSystem()
{
    m_dispatcher.disconnect(this);
}

void AnimationStateSystem::finishAnimation(const engine::utils::FinishAnimationEvent& event)
{
    if (!m_registry.valid(event.m_entity)) {
        return;
    }

    /* --- 敌人动画结束逻辑 --- */
    if (m_registry.all_of<game::component::EnemyComponent>(event.m_entity)) {
        // 如果敌人被阻挡，则返回 idle 动画
        if (auto* blockedByComponent = m_registry.try_get<game::component::BlockedByComponent>(
                event.m_entity);
            blockedByComponent != nullptr) {
            m_dispatcher.enqueue(
                engine::utils::PlayAnimationEvent{ event.m_entity, "idle"_hs, true });
            spdlog::info("敌人 ID: {} 行动动画结束，拥有被阻挡组件，返回 idle 动画",
                         entt::to_integral(event.m_entity));
        } else { // 如果敌人没有被阻挡，则返回 walk 动画
            m_dispatcher.enqueue(
                engine::utils::PlayAnimationEvent{ event.m_entity, "walk"_hs, true });
            spdlog::info("敌人 ID: {} 行动动画结束，没有被阻挡组件，返回 walk 动画",
                         entt::to_integral(event.m_entity));
        }

        // 移除动作锁定标签
        m_registry.remove<game::defs::ActionLockedTag>(event.m_entity);
        return;
    }

    /* --- 玩家动画结束逻辑 --- */
    // 玩家动画结束，直接返回 idle 动画
    if (m_registry.all_of<game::component::PlayerComponent>(event.m_entity)) {
        m_dispatcher.enqueue(engine::utils::PlayAnimationEvent{ event.m_entity, "idle"_hs, true });
        spdlog::info("玩家 ID: {} 行动动画结束，返回 idle 动画", entt::to_integral(event.m_entity));
        return;
    }
}

} // namespace game::system
